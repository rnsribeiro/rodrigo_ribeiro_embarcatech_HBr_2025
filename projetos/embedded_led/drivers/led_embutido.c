#include "led_embutido.h"
#include "pico/cyw43_arch.h"

void led_embutido_init() {
    if (cyw43_arch_init()) {
        // Erro de inicialização
        while (true) {}
    }
}

void led_embutido_set(bool estado) {
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, estado);
}
