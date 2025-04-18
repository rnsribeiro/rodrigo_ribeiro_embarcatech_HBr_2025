#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "ssd1306.h"

// Definições de pinos
const uint I2C_SDA = 14; // Pino SDA para I2C do OLED
const uint I2C_SCL = 15; // Pino SCL para I2C do OLED
const uint VRX = 27;     // Pino de leitura do eixo X (GPIO27, ADC1)
const uint VRY = 26;     // Pino de leitura do eixo Y (GPIO26, ADC0)
const uint ADC_CHANNEL_X = 1; // Canal ADC para o eixo X (ADC1)
const uint ADC_CHANNEL_Y = 0; // Canal ADC para o eixo Y (ADC0)

// Função para configurar o joystick (ADC)
void setup_joystick() {
    adc_init();         // Inicializa o módulo ADC
    adc_gpio_init(VRX); // Configura o pino VRX (eixo X) para entrada ADC
    adc_gpio_init(VRY); // Configura o pino VRY (eixo Y) para entrada ADC
}

// Função para ler os valores dos eixos do joystick (X e Y)
void joystick_read_axis(uint16_t *vrx_value, uint16_t *vry_value) {
    adc_select_input(ADC_CHANNEL_X); // Seleciona o canal ADC para o eixo X
    sleep_us(2);                     // Pequeno delay para estabilidade
    *vrx_value = adc_read();         // Lê o valor do eixo X (0-4095)

    adc_select_input(ADC_CHANNEL_Y); // Seleciona o canal ADC para o eixo Y
    sleep_us(2);                     // Pequeno delay para estabilidade
    *vry_value = adc_read();         // Lê o valor do eixo Y (0-4095)
}

// Função para mapear valores ADC para posições do joystick
void map_joystick_position(uint16_t value, char *position, const char *left_down, const char *center, const char *right_up) {
    if (value < 1500) {
        strcpy(position, left_down); // Esquerda ou Baixo
    } else if (value > 2500) {
        strcpy(position, right_up);  // Direita ou Cima
    } else {
        strcpy(position, center);    // Centro
    }
}

// Função para atualizar o display OLED com as posições e valores do joystick em duas linhas
void update_display(uint8_t *ssd, struct render_area *area, const char *x_pos, const char *y_pos, uint16_t vrx_value, uint16_t vry_value) {
    memset(ssd, 0, ssd1306_buffer_length); // Limpa o buffer
    char x_str[32];
    char y_str[32];
    sprintf(x_str, "X %s %d", x_pos, vrx_value); // Ex.: "X DIREITA 4095"
    sprintf(y_str, "Y %s %d", y_pos, vry_value); // Ex.: "Y CENTRO 2048"
    ssd1306_draw_string(ssd, 0, 0, x_str);  // Exibe X na linha 1
    ssd1306_draw_string(ssd, 0, 16, y_str); // Exibe Y na linha 2
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
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&frame_area);

    // Buffer para o OLED
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);

    // Configura o joystick
    setup_joystick();

    uint16_t vrx_value, vry_value; // Valores brutos do ADC
    char x_position[16], y_position[16]; // Strings para armazenar as posições

    while (true) {
        // Lê os valores dos eixos
        joystick_read_axis(&vrx_value, &vry_value);

        // Mapeia os valores para posições
        map_joystick_position(vrx_value, x_position, "ESQUERDA", "CENTRO", "DIREITA");
        map_joystick_position(vry_value, y_position, "BAIXO", "CENTRO", "CIMA");

        // Atualiza o display com posições e valores
        update_display(ssd, &frame_area, x_position, y_position, vrx_value, vry_value);

        sleep_ms(100); // Pequeno delay para evitar sobrecarga da CPU
    }

    return 0;
}