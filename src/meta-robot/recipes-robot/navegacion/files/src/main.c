#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include "motors.h"
#include "sensors.h"
#include "fsm.h"
#include "leds.h"
#include "brushes.h"
#include "vacuum.h"

#define MODE_FILE "/tmp/robot_mode"

static volatile sig_atomic_t running = 1;

static void handle_signal(int signal_number)
{
    (void)signal_number;
    running = 0;
}

static int is_manual_mode(void)
{
    FILE *f = fopen(MODE_FILE, "r");

    if (!f) {
        return 0; // por defecto autónomo
    }

    char mode[32] = {0};

    if (fscanf(f, "%31s", mode) != 1) {
        fclose(f);
        return 0;
    }

    fclose(f);

    return strcmp(mode, "manual") == 0;
}

int main(void)
{
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    if (leds_init() != 0) {
        return 1;
    }

    led_system_set(1);
    led_autonomous_set(0);
    led_manual_set(0);
    led_obstacle_set(0);

    if (brushes_init() != 0) {
        led_system_set(0);
        leds_cleanup();
        return 1;
    }

    if (vacuum_init() != 0) {
        brushes_cleanup();
        led_system_set(0);
        leds_cleanup();
        return 1;
    }

    if (motors_init() != 0 || sensors_init() != 0) {
        vacuum_cleanup();
        brushes_cleanup();
        led_system_set(0);
        leds_cleanup();
        return 1;
    }

    fsm_init();

    sleep(3);

    brushes_on();
    vacuum_on();

    int manual_mode = is_manual_mode();

    if (manual_mode) {
        led_autonomous_set(0);
        led_manual_set(1);
    } else {
        led_autonomous_set(1);
        led_manual_set(0);
        fsm_update(EVENT_START);
    }

    while (running) {
        int new_manual_mode = is_manual_mode();

        if (new_manual_mode != manual_mode) {

            /*
             * Cambió de autónomo a manual.
             */
            if (new_manual_mode) {
                fsm_update(EVENT_STOP);

                led_autonomous_set(0);
                led_manual_set(1);
                led_obstacle_set(0);
            }

            /*
             * Cambió de manual a autónomo.
             */
            else {
                led_manual_set(0);
                led_autonomous_set(1);

                fsm_update(EVENT_START);
            }

            manual_mode = new_manual_mode;
        }

        /*
    * En modo autónomo, la FSM controla el LED de obstáculo.
    * En modo manual, el LED refleja directamente los sensores.
    */
    if (!manual_mode) {
        fsm_update(EVENT_NONE);
    }
    else {
        int front_obstacle = sensor_obstacle_detected();
        int side_obstacle = sensor_side_obstacle_detected();

        if (front_obstacle == 1 || side_obstacle == 1) {
            led_obstacle_set(1);
        }
        else {
            led_obstacle_set(0);
        }
    }

        usleep(10000);
    }

    fsm_update(EVENT_STOP);

    brushes_off();
    vacuum_off();

    led_obstacle_set(0);
    led_autonomous_set(0);
    led_manual_set(0);
    led_system_set(0);

    motors_cleanup();
    vacuum_cleanup();
    brushes_cleanup();
    leds_cleanup();

    return 0;
}