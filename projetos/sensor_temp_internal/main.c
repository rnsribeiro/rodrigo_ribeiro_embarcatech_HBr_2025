#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "ssd1306.h"

// Definições de pinos para o I2C do OLED
const uint I2C_SDA = 14; // Pino SDA para I2C do OLED
const uint I2C_SCL = 15; // Pino SCL para I2C do OLED

// Função para configurar o ADC e o sensor de temperatura interno
void setup_temperature_sensor() {
    adc_init();                      // Inicializa o módulo ADC
    adc_set_temp_sensor_enabled(true); // Habilita o sensor de temperatura interno
    adc_select_input(4);             // Seleciona o canal ADC4 (sensor de temperatura interno)
}

// Função para ler a temperatura e convertê-la para graus Celsius
float read_temperature() {
    uint16_t raw = adc_read(); // Lê o valor bruto do ADC (0 a 4095)
    
    // Converte o valor bruto para tensão (0 a 3.3V)
    const float conversion_factor = 3.3f / 4095;
    float voltage = raw * conversion_factor;
    
    // Converte a tensão para temperatura em graus Celsius
    // Fórmula: T = 27 - (V - 0.706) / 0.001721
    float temperature = 27.0f - (voltage - 0.706f) / 0.001721f;
    
    return temperature;
}

// Função para atualizar o display OLED com a temperatura
void update_display(uint8_t *ssd, struct render_area *area, float temperature) {
    memset(ssd, 0, ssd1306_buffer_length); // Limpa o buffer
    char temp_str[32];
    sprintf(temp_str, "Temp: %.1f^C", temperature); // Formata a string: "Temp: 25.5^C" (será renderizado como "Temp: 25.5ºC")
    ssd1306_draw_string(ssd, 0, 16, temp_str); // Exibe a temperatura na linha 2 (centralizada verticalmente)
    render_on_display(ssd, area); // Atualiza o display
}

int main() {
    stdio_init_all(); // Inicializa comunicação serial via USB (opcional, para debug)

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

    // Configura o sensor de temperatura
    setup_temperature_sensor();

    float temperature; // Variável para armazenar a temperatura

    while (true) {
        // Lê a temperatura
        temperature = read_temperature();

        // Atualiza o display com a temperatura
        update_display(ssd, &frame_area, temperature);

        sleep_ms(1000); // Atualiza a cada 1 segundo
    }

    return 0;
}