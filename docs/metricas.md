# Reporte de métricas de eficiencia

Este documento reporta las métricas obligatorias de eficiencia de recursos
sobre la imagen final del sistema. Los valores se obtienen ejecutando `scripts/measure_metrics.sh`
directamente en el target (Raspberry Pi 4) con la imagen completa corriendo.

_Última actualización: pendiente de primera corrida sobre el sistema integrado._

---

## 1. Tamaño del rootfs

**Herramienta usada:** `df -h` (en el target, sobre la partición raíz ya
desplegada) y `du` (en el host, sobre el artefacto `.rootfs.tar` generado
por bitbake).

**Justificación:** `df -h` refleja el uso real una vez desplegado en la
tarjeta SD, incluyendo overhead del sistema de archivos; `du` sobre el
artefacto en el host permite verificar el tamaño antes incluso de flashear
la imagen, útil para detectar crecimientos inesperados entre builds.

**Comandos:**
```bash
# En el target
df -h /

# En el host
du -sh build/tmp/deploy/images/raspberrypi4/*.rootfs.tar.*
```

**Resultado:** _(pendiente — completar tras correr `measure_metrics.sh`)_

**Presupuesto de referencia:** 200 MB
**¿Cumple?** _(pendiente)_
**Justificación si se excede:** _(pendiente, si aplica)_

---

## 2. Tiempo de arranque

**Herramienta usada:** `systemd-analyze` y `systemd-analyze critical-chain`.

**Justificación:** `systemd-analyze` es la herramienta estándar de systemd
para medir tiempos de arranque sin necesidad de instrumentación manual.
`critical-chain` permite aislar específicamente el tiempo hasta que el
servicio de control (`robot-server.service`) queda activo, que es la
métrica exacta pedida por la especificación — no el tiempo total de
arranque del sistema completo.

**Comandos:**
```bash
systemd-analyze
systemd-analyze blame
systemd-analyze critical-chain robot-server.service
```

**Resultado:** _(pendiente — completar tras correr `measure_metrics.sh`)_

**Presupuesto de referencia:** 15 s
**¿Cumple?** _(pendiente)_
**Justificación si se excede:** _(pendiente, si aplica)_

---

## 3. Uso de RAM y CPU en operación normal

**Herramienta usada:** `free -h`, `top -b -n 1`, `/proc/<PID>/status`.

**Justificación:** `free` da una vista rápida y estándar del consumo global
de RAM sin overhead adicional; `top -b -n 1` permite capturar una instantánea
reproducible de CPU/RAM por proceso; `/proc/<PID>/status` aísla el consumo
específico del proceso del servidor de control, que es el de interés directo
para el curso. Ninguna de estas herramientas requiere instalar paquetes
adicionales más allá de lo que ya trae una imagen mínima de Yocto.

**Procedimiento para generar la carga de "operación normal":**
1. Se activa el modo autónomo del robot (navegación).
2. Se reproduce un archivo MP3 de prueba (audio).
3. Se mantiene el panel web abierto, consumiendo datos de sensores/mapa
   mediante polling.
4. Tras ~15 segundos de estabilización con los tres subsistemas activos
   simultáneamente, se capturan las métricas.

**Comandos:**
```bash
free -h
top -b -n 1
cat /proc/<PID>/status | grep -E "VmRSS|VmSize"
```

**Resultado:**
- RAM total usada: _(pendiente)_ de _(pendiente)_ disponibles
- RAM específica del proceso del servidor (VmRSS): _(pendiente)_
- CPU observado durante la prueba: _(pendiente)_

---

## Paquetes adicionales agregados a la imagen mínima

| Paquete | Justificación |
|---|---|
| `uhttpd` | Servidor HTTP recomendado por el profesor, liviano (~22 KB), usado para servir el panel de control y ejecutar los CGI |
| `cjson` | Parseo y generación de JSON en los binarios CGI del servidor |
| _(completar)_ | _(completar conforme se agreguen más paquetes a `IMAGE_INSTALL`, ej. decodificador MP3, librerías de audio, etc.)_ |

---

## Cómo regenerar este reporte

Este documento se genera automáticamente (salvo las columnas de
interpretación/justificación) con el script `scripts/measure_metrics.sh`.
Para actualizarlo tras cambios en el sistema:

```bash
scp scripts/measure_metrics.sh root@<ip-raspberry>:~/
ssh root@<ip-raspberry>
chmod +x measure_metrics.sh
./measure_metrics.sh
cat metricas_reporte.md
```

Copiar el contenido de `metricas_reporte.md` a este archivo
(`docs/metricas.md`), y completar manualmente las secciones de
"¿Cumple?", justificaciones y la tabla de paquetes adicionales.