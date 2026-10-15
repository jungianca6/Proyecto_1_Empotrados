#!/bin/sh

OUTPUT_DIR="/var/lib/robot"
OUTPUT_FILE="${OUTPUT_DIR}/metrics.json"
TEMP_FILE="${OUTPUT_DIR}/metrics.json.tmp"

mkdir -p "$OUTPUT_DIR"

# ============================================================
# 1. TIEMPO DE ARRANQUE
# ============================================================

# /proc/uptime contiene los segundos transcurridos desde el boot.
BOOT_SECONDS=$(awk '{printf "%.2f", $1}' /proc/uptime)

# Estado de los servicios importantes.
if systemctl is-active --quiet robot-server.service; then
    SERVER_ACTIVE=true
else
    SERVER_ACTIVE=false
fi

if systemctl is-active --quiet robot-wifi.service; then
    WIFI_ACTIVE=true
else
    WIFI_ACTIVE=false
fi


# ============================================================
# 2. NAVEGACION
# ============================================================

NAVIGATION_ACTIVE=false
NAVIGATION_SECONDS="null"

# La navegacion se inicia mediante /etc/init.d/app_robot.
# Esperamos un maximo de 30 segundos por si arranca unos instantes
# despues de este servicio.
i=0

while [ "$i" -lt 30 ]; do
    if pidof app_robot >/dev/null 2>&1; then
        NAVIGATION_ACTIVE=true
        NAVIGATION_SECONDS=$(awk '{printf "%.2f", $1}' /proc/uptime)
        break
    fi

    sleep 1
    i=$((i + 1))
done


# ============================================================
# 3. ROOTFS
# ============================================================

# df -kP entrega valores en KiB y un formato facil de procesar.
ROOTFS_TOTAL_KB=$(df -kP / | awk 'NR == 2 {print $2}')
ROOTFS_USED_KB=$(df -kP / | awk 'NR == 2 {print $3}')
ROOTFS_AVAILABLE_KB=$(df -kP / | awk 'NR == 2 {print $4}')

ROOTFS_TOTAL_MB=$(awk \
    -v value="$ROOTFS_TOTAL_KB" \
    'BEGIN {printf "%.2f", value / 1024}')

ROOTFS_USED_MB=$(awk \
    -v value="$ROOTFS_USED_KB" \
    'BEGIN {printf "%.2f", value / 1024}')

ROOTFS_AVAILABLE_MB=$(awk \
    -v value="$ROOTFS_AVAILABLE_KB" \
    'BEGIN {printf "%.2f", value / 1024}')


# ============================================================
# 4. RAM
# ============================================================

MEM_TOTAL_KB=$(awk '/^MemTotal:/ {print $2}' /proc/meminfo)
MEM_AVAILABLE_KB=$(awk '/^MemAvailable:/ {print $2}' /proc/meminfo)

MEM_USED_KB=$((MEM_TOTAL_KB - MEM_AVAILABLE_KB))

MEM_TOTAL_MB=$(awk \
    -v value="$MEM_TOTAL_KB" \
    'BEGIN {printf "%.2f", value / 1024}')

MEM_USED_MB=$(awk \
    -v value="$MEM_USED_KB" \
    'BEGIN {printf "%.2f", value / 1024}')

MEM_AVAILABLE_MB=$(awk \
    -v value="$MEM_AVAILABLE_KB" \
    'BEGIN {printf "%.2f", value / 1024}')


# ============================================================
# 5. CPU
# ============================================================

read_cpu()
{
    set -- $(grep '^cpu ' /proc/stat)

    USER_CPU=$2
    NICE_CPU=$3
    SYSTEM_CPU=$4
    IDLE_CPU=$5
    IOWAIT_CPU=$6
    IRQ_CPU=$7
    SOFTIRQ_CPU=$8
    STEAL_CPU=$9

    CPU_IDLE=$((IDLE_CPU + IOWAIT_CPU))

    CPU_TOTAL=$(( \
        USER_CPU + \
        NICE_CPU + \
        SYSTEM_CPU + \
        IDLE_CPU + \
        IOWAIT_CPU + \
        IRQ_CPU + \
        SOFTIRQ_CPU + \
        STEAL_CPU \
    ))
}

read_cpu

CPU_TOTAL_1=$CPU_TOTAL
CPU_IDLE_1=$CPU_IDLE

# Ventana de medicion de CPU.
sleep 2

read_cpu

CPU_TOTAL_2=$CPU_TOTAL
CPU_IDLE_2=$CPU_IDLE

DELTA_TOTAL=$((CPU_TOTAL_2 - CPU_TOTAL_1))
DELTA_IDLE=$((CPU_IDLE_2 - CPU_IDLE_1))

if [ "$DELTA_TOTAL" -gt 0 ]; then
    CPU_PERCENT=$(awk \
        -v total="$DELTA_TOTAL" \
        -v idle="$DELTA_IDLE" \
        'BEGIN {
            printf "%.2f", 100 * (total - idle) / total
        }')
else
    CPU_PERCENT="0.00"
fi


# ============================================================
# 6. ESTADO DEL PROCESO DE NAVEGACION
# ============================================================

NAVIGATION_RSS_KB="null"

NAV_PID=$(pidof app_robot 2>/dev/null | awk '{print $1}')

if [ -n "$NAV_PID" ] && [ -r "/proc/$NAV_PID/status" ]; then
    NAV_RSS=$(awk '/^VmRSS:/ {print $2}' "/proc/$NAV_PID/status")

    if [ -n "$NAV_RSS" ]; then
        NAVIGATION_RSS_KB="$NAV_RSS"
    fi
fi


# ============================================================
# 7. GENERAR JSON
# ============================================================

cat > "$TEMP_FILE" <<EOF
{
    "rootfs_used_mb": ${ROOTFS_USED_MB},
    "rootfs_total_mb": ${ROOTFS_TOTAL_MB},
    "rootfs_available_mb": ${ROOTFS_AVAILABLE_MB},
    "rootfs_limit_mb": 200,

    "boot_seconds": ${BOOT_SECONDS},
    "boot_limit_seconds": 15,

    "ram_used_mb": ${MEM_USED_MB},
    "ram_total_mb": ${MEM_TOTAL_MB},
    "ram_available_mb": ${MEM_AVAILABLE_MB},

    "cpu_percent": ${CPU_PERCENT},

    "server_active": ${SERVER_ACTIVE},
    "wifi_active": ${WIFI_ACTIVE},
    "navigation_active": ${NAVIGATION_ACTIVE},
    "navigation_ready_seconds": ${NAVIGATION_SECONDS},
    "navigation_rss_kb": ${NAVIGATION_RSS_KB}
}
EOF

# Reemplazo atomico para que el CGI nunca lea un archivo incompleto.
mv "$TEMP_FILE" "$OUTPUT_FILE"

chmod 0644 "$OUTPUT_FILE"

exit 0