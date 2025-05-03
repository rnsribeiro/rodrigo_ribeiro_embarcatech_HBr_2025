#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"

// Definições de pinos para o I2C do OLED
const uint I2C_SDA = 14; // Pino SDA para I2C do OLED
const uint I2C_SCL = 15; // Pino SCL para I2C do OLED

// Constantes do Galton Board
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define NUM_CHANNELS 16
#define CHANNEL_WIDTH (SCREEN_WIDTH / NUM_CHANNELS) // 8 pixels por canal
#define PIN_ROWS 10
#define PIN_GRID_HEIGHT 32 // Altura da grade de pinos (metade superior, achatada)
#define TICK_MS 10 // Intervalo de tick em milissegundos
#define HISTOGRAM_SCALE 0.5 // Fator de achatamento do histograma

// Estrutura da bola
typedef struct {
    int x; // Posição horizontal (pixels)
    int y; // Posição vertical (pixels)
    bool active; // Estado (caindo ou não)
} Ball;

// Variáveis globais
Ball ball; // Única bola
int channels[NUM_CHANNELS] = {0}; // Contagem de bolas por canal
int total_balls = 0; // Contador total de bolas
float bias = 0.5; // Probabilidade de desvio (0.5 = equilibrado)
bool ball_in_progress = false; // Controla se uma bola está caindo
uint8_t ssd[ssd1306_buffer_length]; // Buffer para o OLED
struct render_area frame_area; // Área de renderização

// Função para escolha aleatória com viés
int random_direction() {
    return (rand() % 100) < (bias * 100) ? 1 : -1; // 1 = direita, -1 = esquerda
}

// Inicializa a bola
void init_ball(Ball *ball) {
    ball->x = SCREEN_WIDTH / 2; // Centro
    ball->y = 0;
    ball->active = true;
    ball_in_progress = true; // Marca que uma bola está caindo
}

// Desenha a bola como um quadrado 3x3
void draw_ball(uint8_t *ssd, int x, int y) {
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            int px = x + dx;
            int py = y + dy;
            if (px >= 0 && px < SCREEN_WIDTH && py >= 0 && py < SCREEN_HEIGHT) {
                ssd1306_set_pixel(ssd, px, py, true);
            }
        }
    }
}

// Desenha um pino como um quadrado 2x2
void draw_pin(uint8_t *ssd, int x, int y) {
    for (int dx = 0; dx <= 1; dx++) {
        for (int dy = 0; dy <= 1; dy++) {
            int px = x + dx;
            int py = y + dy;
            if (px >= 0 && px < SCREEN_WIDTH && py >= 0 && py < SCREEN_HEIGHT) {
                ssd1306_set_pixel(ssd, px, py, true);
            }
        }
    }
}

// Desenha os pinos em uma grade de pirâmide achatada (triângulo invertido)
void draw_pins(uint8_t *ssd) {
    for (int row = 0; row < PIN_ROWS; row++) {
        int num_pins = PIN_ROWS - row; // Linha i tem PIN_ROWS-row pinos (pirâmide)
        int y = row * (PIN_GRID_HEIGHT / PIN_ROWS); // Posição y achatada
        float spacing = (float)SCREEN_WIDTH / num_pins; // Espaçamento entre pinos
        for (int i = 0; i < num_pins; i++) {
            int x = (int)((i + 0.5) * spacing); // Centraliza pino no segmento
            draw_pin(ssd, x, y);
        }
    }
}

// Atualiza posição da bola
void update_ball(Ball *ball) {
    if (!ball->active) return;
    
    ball->y++; // Desce
    if (ball->y % (PIN_GRID_HEIGHT / PIN_ROWS) == 0 && ball->y > 0) { // Linha de pinos
        int row = (ball->y / (PIN_GRID_HEIGHT / PIN_ROWS)) - 1; // Linha atual (0 a 9)
        float spacing = (float)SCREEN_WIDTH / (PIN_ROWS - row); // Espaçamento dos pinos
        ball->x += random_direction() * (int)(spacing / 2); // Desvia proporcionalmente
        // Garante que a bola fique dentro dos limites
        if (ball->x < 0) ball->x = 0;
        if (ball->x >= SCREEN_WIDTH) ball->x = SCREEN_WIDTH - 1;
    }
    
    if (ball->y >= SCREEN_HEIGHT) { // Chegou ao fundo
        int channel = ball->x / CHANNEL_WIDTH;
        if (channel >= 0 && channel < NUM_CHANNELS) {
            channels[channel]++;
            total_balls++; // Incrementa apenas uma vez
        }
        ball->active = false; // Desativa a bola
        ball_in_progress = false; // Permite nova bola
    }
}

// Desenha histograma achatado proporcionalmente
void draw_histogram(uint8_t *ssd) {
    for (int i = 0; i < NUM_CHANNELS; i++) {
        int height = (int)(channels[i] * HISTOGRAM_SCALE); // Escala achatada
        if (height > SCREEN_HEIGHT - 10) height = SCREEN_HEIGHT - 10; // Limite
        for (int x = i * CHANNEL_WIDTH; x < (i + 1) * CHANNEL_WIDTH; x++) {
            ssd1306_draw_line(ssd, x, SCREEN_HEIGHT - 1, x, SCREEN_HEIGHT - height, true);
        }
    }
}

// Atualiza o display com bola, pinos, histograma e contador
void update_display(uint8_t *ssd, struct render_area *area) {
    memset(ssd, 0, ssd1306_buffer_length); // Limpa o buffer
    
    // Desenha pinos
    draw_pins(ssd);
    
    // Desenha a bola
    if (ball.active) {
        draw_ball(ssd, ball.x, ball.y);
    }
    
    // Desenha histograma
    draw_histogram(ssd);
    
    // Exibe contador
    char buf[32];
    sprintf(buf, "Bolas: %d", total_balls);
    ssd1306_draw_string(ssd, 0, 0, buf);
    
    // Atualiza o display
    render_on_display(ssd, area);
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
    frame_area.start_column = 0;
    frame_area.end_column = ssd1306_width - 1;
    frame_area.start_page = 0;
    frame_area.end_page = ssd1306_n_pages - 1;
    calculate_render_area_buffer_length(&frame_area);

    // Inicializa o buffer
    memset(ssd, 0, ssd1306_buffer_length);

    // Inicializa a primeira bola
    init_ball(&ball);

    while (true) {
        // Atualiza a bola
        if (ball.active) {
            update_ball(&ball);
        }

        // Inicia nova bola se nenhuma estiver caindo
        if (!ball_in_progress) {
            init_ball(&ball);
        }

        // Atualiza o display
        update_display(ssd, &frame_area);

        sleep_ms(TICK_MS); // Delay para simular tick

        // Exemplo de desbalanceamento (ativar após 100 bolas)
        if (total_balls == 100) {
            bias = 0.6; // 60% de chance de ir para a direita
        }
    }

    return 0;
}