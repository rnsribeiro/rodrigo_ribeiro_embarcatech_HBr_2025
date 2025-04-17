#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "ssd1306.h"

// Definições de pinos
const uint I2C_SDA = 14; // Pino SDA para I2C do OLED
const uint I2C_SCL = 15; // Pino SCL para I2C do OLED
const uint BUTTON_A = 5; // Botão A (GPIO5)
const uint BUTTON_B = 6; // Botão B (GPIO6)

// Variáveis globais
volatile int counter = 0; // Contador decrescente (9 a 0)
volatile int button_b_count = 0; // Contagem de pressionamentos do Botão B
volatile bool is_counting = false; // Estado da contagem regressiva
volatile uint64_t last_time = 0; // Último tempo para controle de 1 segundo
volatile bool button_a_pressed = false; // Flag para Botão A
volatile bool button_b_pressed = false; // Flag para Botão B

// Função de callback para interrupções dos botões
void gpio_callback(uint gpio, uint32_t events) {
    // Botão A pressionado (borda de descida)
    if (gpio == BUTTON_A && (events & GPIO_IRQ_EDGE_FALL)) {
        button_a_pressed = true;
    }
    // Botão B pressionado (borda de descida)
    if (gpio == BUTTON_B && (events & GPIO_IRQ_EDGE_FALL) && is_counting) {
        button_b_pressed = true;
    }
}

/**
 * Atualiza o display OLED com o valor do contador e a contagem de Botão B
 */
void update_display(uint8_t *ssd, struct render_area *area, int counter, int b_count) {
    memset(ssd, 0, ssd1306_buffer_length); // Limpa o buffer
    char counter_str[16];
    char button_b_str[16];
    sprintf(counter_str, "Counter: %d", counter);
    sprintf(button_b_str, "B Clicks: %d", b_count);
    ssd1306_draw_string(ssd, 0, 0, counter_str); // Exibe contador na linha 1
    ssd1306_draw_string(ssd, 0, 16, button_b_str); // Exibe contagem de B na linha 2
    render_on_display(ssd, area); // Atualiza o display
}

int main() {
    stdio_init_all(); // Inicializa comunicação serial via USB

    // Inicializa I2C para o OLED
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Inicializa o display OLED SSD1306
    ssd1306_init();

    // Configura a área de renderização do OLED (128x64 pixels, 8 páginas)
    struct render_area frame_area = {
        start_column: 0,
        end_column: ssd1306_width - 1,
        start_page: 0,
        end_page: ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&frame_area);

    // Buffer para o OLED
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);

    // Configura os botões como entrada com pull-up
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);

    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);

    // Configura interrupções para os botões (borda de descida)
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled(BUTTON_B, GPIO_IRQ_EDGE_FALL, true);

    while (true) {
        // Verifica se o Botão A foi pressionado
        if (button_a_pressed) {
            counter = 9; // Reinicia o contador
            button_b_count = 0; // Zera a contagem de Botão B
            is_counting = true; // Ativa a contagem regressiva
            last_time = time_us_64() / 1000; // Registra o tempo inicial
            button_a_pressed = false; // Reseta a flag
            update_display(ssd, &frame_area, counter, button_b_count); // Atualiza o display
            sleep_ms(200); // Debounce
        }

        // Verifica se o Botão B foi pressionado (apenas se estiver contando)
        if (button_b_pressed && is_counting) {
            button_b_count++; // Incrementa a contagem de Botão B
            button_b_pressed = false; // Reseta a flag
            update_display(ssd, &frame_area, counter, button_b_count); // Atualiza o display
            sleep_ms(200); // Debounce
        }

        // Lógica da contagem regressiva
        if (is_counting) {
            uint64_t current_time = time_us_64() / 1000; // Tempo atual em milissegundos
            if (current_time - last_time >= 1000) { // Passou 1 segundo?
                counter--; // Decrementa o contador
                last_time = current_time; // Atualiza o último tempo
                update_display(ssd, &frame_area, counter, button_b_count); // Atualiza o display
                if (counter <= 0) {
                    is_counting = false; // Para a contagem quando atinge 0
                }
            }
        }

        sleep_ms(10); // Pequeno delay para evitar sobrecarga da CPU
    }

    return 0;
}