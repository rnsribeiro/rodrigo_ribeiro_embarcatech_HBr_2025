#include "hal_led.h"
#include "led_embutido.h"

static bool led_state = false;

void hal_led_init() {
    led_embutido_init();
}

void hal_led_toggle() {
    led_state = !led_state;
    led_embutido_set(led_state);
}
