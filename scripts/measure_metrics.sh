#!/bin/sh
# ==============================================================================
# measure_metrics.sh
#
# Script para medir las métricas obligatorias del Proyecto I (CE-1113):
#   1. Tamaño del rootfs
#   2. Tiempo de arranque hasta que el servicio de control está operativo
#   3. Uso de RAM y CPU en operación normal
#
# USO:
#   1. Copiar este script a la Raspberry Pi (scp measure_metrics.sh root@<ip>:~/)
#   2. Ejecutarlo DIRECTAMENTE EN EL TARGET (Raspberry Pi), no en el host:
#        chmod +x measure_metrics.sh
#        ./measure_metrics.sh
#   3. Genera DOS archivos:
#        - metricas_reporte.txt  -> salida cruda de cada comando (evidencia)
#        - metricas_reporte.md   -> bloque ya formateado para pegar en
#                                   docs/metricas.md
#
# AJUSTA estas variables según el nombre real de tu servicio/binario antes
# de correr el script:
# ==============================================================================

SERVICE_NAME="robot-server.service"   # nombre real de la unidad systemd
BINARY_NAME="robot-server"            # nombre del proceso para buscar en /proc
AUDIO_FILE="/usr/share/robot/music/test.mp3"  # archivo de prueba para generar carga
BASE_URL="http://localhost"           # ajustar si el script corre en otra máquina

OUTPUT_TXT="metricas_reporte.txt"
OUTPUT_MD="metricas_reporte.md"

echo "==============================================" > "$OUTPUT_TXT"
echo " Reporte de métricas - $(date)" >> "$OUTPUT_TXT"
echo "==============================================" >> "$OUTPUT_TXT"

# ------------------------------------------------------------------------------
# 1. TAMAÑO DEL ROOTFS
# ------------------------------------------------------------------------------
echo "" >> "$OUTPUT_TXT"
echo "----- 1. Tamaño del rootfs -----" >> "$OUTPUT_TXT"
echo "Comando: df -h /" >> "$OUTPUT_TXT"
DF_OUTPUT=$(df -h / 2>&1)
echo "$DF_OUTPUT" >> "$OUTPUT_TXT"
echo "" >> "$OUTPUT_TXT"
echo "Nota: para el tamaño del rootfs generado por bitbake (sin montar en SD)," >> "$OUTPUT_TXT"
echo "correr en el HOST: du -sh build/tmp/deploy/images/raspberrypi4/*.rootfs.tar.*" >> "$OUTPUT_TXT"

# Extraer tamaño usado (columna 3 de la segunda línea de df -h /)
ROOTFS_USED=$(df -h / | awk 'NR==2 {print $3}')
ROOTFS_TOTAL=$(df -h / | awk 'NR==2 {print $2}')
ROOTFS_PCT=$(df -h / | awk 'NR==2 {print $5}')

# ------------------------------------------------------------------------------
# 2. TIEMPO DE ARRANQUE
# ------------------------------------------------------------------------------
echo "" >> "$OUTPUT_TXT"
echo "----- 2. Tiempo de arranque -----" >> "$OUTPUT_TXT"

BOOT_SUMMARY="No disponible (systemd-analyze no encontrado)"
CRITICAL_CHAIN_OUT="No disponible"

if command -v systemd-analyze > /dev/null 2>&1; then
    echo "Comando: systemd-analyze" >> "$OUTPUT_TXT"
    BOOT_SUMMARY=$(systemd-analyze 2>&1)
    echo "$BOOT_SUMMARY" >> "$OUTPUT_TXT"

    echo "" >> "$OUTPUT_TXT"
    echo "Comando: systemd-analyze blame (top 10)" >> "$OUTPUT_TXT"
    BLAME_OUT=$(systemd-analyze blame 2>&1 | head -n 10)
    echo "$BLAME_OUT" >> "$OUTPUT_TXT"

    echo "" >> "$OUTPUT_TXT"
    echo "Comando: systemd-analyze critical-chain $SERVICE_NAME" >> "$OUTPUT_TXT"
    CRITICAL_CHAIN_OUT=$(systemd-analyze critical-chain "$SERVICE_NAME" 2>&1)
    echo "$CRITICAL_CHAIN_OUT" >> "$OUTPUT_TXT"
else
    echo "ADVERTENCIA: systemd-analyze no está disponible en esta imagen." >> "$OUTPUT_TXT"
    echo "Agregar 'systemd-analyze' o el paquete correspondiente a IMAGE_INSTALL," >> "$OUTPUT_TXT"
    echo "o reportar el tiempo de arranque de otra forma (ej. timestamp manual)." >> "$OUTPUT_TXT"
fi

# ------------------------------------------------------------------------------
# 3. USO DE RAM Y CPU EN OPERACIÓN NORMAL
# ------------------------------------------------------------------------------
echo "" >> "$OUTPUT_TXT"
echo "----- 3. Uso de RAM y CPU en operación normal -----" >> "$OUTPUT_TXT"
echo "" >> "$OUTPUT_TXT"
echo "Intentando generar carga normal (modo autónomo + audio + servidor)..." >> "$OUTPUT_TXT"

LOGIN_RESPONSE=$(curl -s -X POST "$BASE_URL/cgi-bin/login" \
    -d '{"username":"admin","password":"robot123"}')
TOKEN=$(echo "$LOGIN_RESPONSE" | grep -o '"token":"[^"]*"' | cut -d'"' -f4)

CARGA_GENERADA="No (no se pudo autenticar contra el servidor)"

if [ -n "$TOKEN" ]; then
    echo "Login OK, token obtenido. Activando modo autónomo y reproduciendo audio..." >> "$OUTPUT_TXT"
    curl -s -X POST "$BASE_URL/cgi-bin/mode" \
        -H "Authorization: $TOKEN" \
        -d '{"mode":"autonomous"}' > /dev/null

    curl -s -X POST "$BASE_URL/cgi-bin/audio_play" \
        -H "Authorization: $TOKEN" \
        -d "{\"file\":\"$AUDIO_FILE\"}" > /dev/null

    CARGA_GENERADA="Sí (modo autónomo activado + audio reproduciéndose)"
else
    echo "No se pudo autenticar contra el servidor (revisar si uhttpd está activo" >> "$OUTPUT_TXT"
    echo "y si las credenciales/puerto son correctos). Se continuará midiendo" >> "$OUTPUT_TXT"
    echo "el estado actual del sistema sin generar carga adicional." >> "$OUTPUT_TXT"
fi

echo "Esperando 15 segundos para estabilizar la carga..." >> "$OUTPUT_TXT"
sleep 15

echo "" >> "$OUTPUT_TXT"
echo "Comando: free -h" >> "$OUTPUT_TXT"
FREE_OUTPUT=$(free -h 2>&1)
echo "$FREE_OUTPUT" >> "$OUTPUT_TXT"

echo "" >> "$OUTPUT_TXT"
echo "Comando: top -b -n 1 (primeras 15 líneas)" >> "$OUTPUT_TXT"
TOP_OUTPUT=$(top -b -n 1 2>&1 | head -n 15)
echo "$TOP_OUTPUT" >> "$OUTPUT_TXT"

# Extraer RAM total usada y disponible de 'free -h' (línea "Mem:")
MEM_USED=$(echo "$FREE_OUTPUT" | awk '/^Mem:/ {print $3}')
MEM_TOTAL=$(echo "$FREE_OUTPUT" | awk '/^Mem:/ {print $2}')

# Extraer % de CPU en uso (idle) de la línea %Cpu(s) de top, si existe
CPU_LINE=$(echo "$TOP_OUTPUT" | grep -i "%Cpu")

echo "" >> "$OUTPUT_TXT"
PID=$(pidof "$BINARY_NAME" 2>/dev/null | awk '{print $1}')
PROC_MEM="No disponible"
if [ -n "$PID" ]; then
    echo "Comando: cat /proc/$PID/status (proceso $BINARY_NAME, PID $PID)" >> "$OUTPUT_TXT"
    PROC_STATUS=$(grep -E "VmRSS|VmSize|State" /proc/"$PID"/status 2>&1)
    echo "$PROC_STATUS" >> "$OUTPUT_TXT"
    PROC_MEM=$(echo "$PROC_STATUS" | awk '/VmRSS/ {print $2, $3}')

    echo "" >> "$OUTPUT_TXT"
    echo "Comando: cat /proc/$PID/stat (utime/stime en jiffies)" >> "$OUTPUT_TXT"
    cat /proc/"$PID"/stat >> "$OUTPUT_TXT" 2>&1
else
    echo "ADVERTENCIA: no se encontró un proceso llamado '$BINARY_NAME' corriendo." >> "$OUTPUT_TXT"
    echo "Ajusta la variable BINARY_NAME al inicio del script con el nombre real." >> "$OUTPUT_TXT"
fi

if [ -n "$TOKEN" ]; then
    curl -s -X POST "$BASE_URL/cgi-bin/audio_stop" \
        -H "Authorization: $TOKEN" > /dev/null
fi

echo "" >> "$OUTPUT_TXT"
echo "==============================================" >> "$OUTPUT_TXT"
echo " Fin del reporte. Revisar $OUTPUT_TXT" >> "$OUTPUT_TXT"
echo "==============================================" >> "$OUTPUT_TXT"


# ==============================================================================
# VALIDACIÓN: detectar mediciones vacías o fallidas antes de dar el reporte
# por terminado, para que no pase desapercibido un dato faltante.
# ==============================================================================

WARNINGS=""

if [ -z "$ROOTFS_USED" ] || [ -z "$ROOTFS_TOTAL" ]; then
    WARNINGS="${WARNINGS}- No se pudo leer el tamaño del rootfs (df -h / no devolvió datos).\n"
fi

if [ "$BOOT_SUMMARY" = "No disponible (systemd-analyze no encontrado)" ]; then
    WARNINGS="${WARNINGS}- systemd-analyze no está disponible; falta el tiempo de arranque.\n"
fi

if echo "$CRITICAL_CHAIN_OUT" | grep -qi "not found\|failed\|No disponible"; then
    WARNINGS="${WARNINGS}- No se encontró el servicio '$SERVICE_NAME' en systemd-analyze critical-chain. Revisa el valor de SERVICE_NAME.\n"
fi

if [ -z "$TOKEN" ]; then
    WARNINGS="${WARNINGS}- No se pudo autenticar contra el servidor web (login falló). La medición de RAM/CPU se hizo SIN la carga real de navegación/audio activada, por lo que no representa 'operación normal'.\n"
fi

if [ -z "$MEM_USED" ] || [ -z "$MEM_TOTAL" ]; then
    WARNINGS="${WARNINGS}- No se pudo leer el uso de RAM (free -h no devolvió datos).\n"
fi

if [ -z "$PID" ]; then
    WARNINGS="${WARNINGS}- No se encontró el proceso '$BINARY_NAME' corriendo. Revisa el valor de BINARY_NAME, o confirma que el servidor esté activo antes de correr el script.\n"
elif [ -z "$PROC_MEM" ]; then
    WARNINGS="${WARNINGS}- Se encontró el proceso '$BINARY_NAME' (PID $PID) pero no se pudo leer su VmRSS desde /proc.\n"
fi

echo "" >> "$OUTPUT_TXT"
echo "----- Validación del reporte -----" >> "$OUTPUT_TXT"
if [ -n "$WARNINGS" ]; then
    echo "ADVERTENCIAS DETECTADAS:" >> "$OUTPUT_TXT"
    printf "%b" "$WARNINGS" >> "$OUTPUT_TXT"
    echo "" >> "$OUTPUT_TXT"
    echo "El reporte se generó de todas formas, pero revisa estos puntos antes" >> "$OUTPUT_TXT"
    echo "de darlo por válido para la entrega." >> "$OUTPUT_TXT"
else
    echo "Sin advertencias: todas las métricas se capturaron correctamente." >> "$OUTPUT_TXT"
fi

# ==============================================================================
# GENERAR BLOQUE MARKDOWN LISTO PARA docs/metricas.md
# ==============================================================================

FECHA=$(date '+%Y-%m-%d %H:%M')

if [ -n "$WARNINGS" ]; then
    MD_WARNINGS=$(printf "> **Advertencias detectadas durante la medición** — revisar antes de dar\n> el reporte por válido para la entrega:\n>\n%s" "$(printf "%b" "$WARNINGS" | sed 's/^/> /')")
else
    MD_WARNINGS="> Sin advertencias: todas las métricas se capturaron correctamente."
fi

cat > "$OUTPUT_MD" << MD_EOF
# Reporte de métricas de eficiencia

_Generado automáticamente por \`scripts/measure_metrics.sh\` el ${FECHA}._

${MD_WARNINGS}

## 1. Tamaño del rootfs

**Herramienta usada:** \`df -h\` (en el target) y \`du\` (en el host, sobre el
artefacto generado por bitbake).
**Justificación:** \`df -h\` refleja el uso real una vez desplegado en la
tarjeta SD; \`du\` sobre el \`.rootfs.tar\` en el host permite verificarlo
antes de flashear.

**Comando:** \`df -h /\`

\`\`\`
${DF_OUTPUT}
\`\`\`

**Resultado:** ${ROOTFS_USED} usados de ${ROOTFS_TOTAL} (${ROOTFS_PCT})
**Presupuesto de referencia:** 200 MB
**¿Cumple?** _(completar manualmente: Sí/No, y justificar si se excede)_

---

## 2. Tiempo de arranque

**Herramienta usada:** \`systemd-analyze\` y \`systemd-analyze critical-chain\`.
**Justificación:** herramienta estándar de systemd; \`critical-chain\` aísla
el tiempo hasta que el servicio de control (\`${SERVICE_NAME}\`) queda activo,
que es la métrica exacta pedida por la especificación.

**Comando:** \`systemd-analyze\`

\`\`\`
${BOOT_SUMMARY}
\`\`\`

**Comando:** \`systemd-analyze critical-chain ${SERVICE_NAME}\`

\`\`\`
${CRITICAL_CHAIN_OUT}
\`\`\`

**Presupuesto de referencia:** 15 s
**¿Cumple?** _(completar manualmente: Sí/No, y justificar si se excede)_

---

## 3. Uso de RAM y CPU en operación normal

**Herramienta usada:** \`free -h\`, \`top -b -n 1\`, \`/proc/<PID>/status\`.
**Justificación:** combina una vista global del sistema con el detalle
específico del proceso del servidor de control, sin dependencias adicionales
en la imagen mínima.

**Carga generada durante la medición:** ${CARGA_GENERADA}

**Comando:** \`free -h\`

\`\`\`
${FREE_OUTPUT}
\`\`\`

**Comando:** \`top -b -n 1\` (primeras líneas)

\`\`\`
${TOP_OUTPUT}
\`\`\`

**Resumen:**
- RAM usada: ${MEM_USED} de ${MEM_TOTAL}
- RAM específica del proceso \`${BINARY_NAME}\` (VmRSS): ${PROC_MEM}
- Línea de CPU global: ${CPU_LINE}

---

## Paquetes adicionales agregados a la imagen mínima

| Paquete | Justificación |
|---|---|
| \`uhttpd\` | Servidor HTTP recomendado por el profesor, liviano (~22 KB) |
| \`cjson\` | Parseo/generación de JSON en los CGI |
| _(completar)_ | _(completar)_ |

MD_EOF

echo "Listo."
echo "Reporte crudo guardado en: $OUTPUT_TXT"
echo "Bloque markdown listo para docs/metricas.md guardado en: $OUTPUT_MD"
echo ""
if [ -n "$WARNINGS" ]; then
    echo "ADVERTENCIAS DETECTADAS (ver detalle en $OUTPUT_TXT y $OUTPUT_MD):"
    printf "%b" "$WARNINGS"
else
    echo "Sin advertencias: todas las métricas se capturaron correctamente."
fi