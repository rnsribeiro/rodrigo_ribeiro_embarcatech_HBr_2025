Autor: **Rodrigo Nunes Sampaio Ribeiro**

Curso: Residência Tecnológica em Sistemas Embarcados

Instituição: EmbarcaTech - HBr

Campinas, março de 2025

---

# Projeto: Contador Decrescente - BitDogLab - Raspberry Pi Pico W

Este projeto implementa um contador decrescente controlado por interrupções, utilizando a placa BitDogLab (microcontrolador Raspberry Pi Pico W - RP2040), um display OLED SSD1306, e dois botões conectados aos pinos GPIO5 (Botão A) e GPIO6 (Botão B). O programa foi desenvolvido em linguagem C, utilizando o Pico SDK versão 1.5.1.

## Enunciado da Atividade

O objetivo é criar um programa em C que implemente um contador decrescente com as seguintes especificações:

- **Comportamento do Botão A (GPIO5)**:

  - Toda vez que o Botão A for pressionado, o contador decrescente é reiniciado com o valor 9 e exibido no display OLED.
  - O sistema entra no modo de contagem regressiva ativa, decrementando o contador de 1 em 1 a cada segundo até chegar a 0.

- **Comportamento do Botão B (GPIO6)**:

  - Durante a contagem regressiva (de 9 a 0), o programa registra quantas vezes o Botão B foi pressionado.
  - A quantidade de pressionamentos do Botão B é exibida no display OLED junto com o valor do contador.

- **Comportamento ao Atingir Zero**:

  - Quando o contador chega a 0, o sistema "congela", exibindo no display:
    - O valor 0 do contador.
    - A quantidade final de pressionamentos do Botão B registrados durante os 9 segundos da contagem.
  - Nesse estado, pressionamentos do Botão B são ignorados (não são acumulados).

- **Reinício**:

  - O sistema permanece parado até que o Botão A seja pressionado novamente, o que:
    - Reinicia o contador para 9.
    - Zera a contagem de pressionamentos do Botão B.
    - Retoma a contagem regressiva.

## Funcionamento do Projeto

O projeto foi implementado com base nos requisitos acima, utilizando os seguintes componentes e lógica:

### Hardware

- **Raspberry Pi Pico W**: Microcontrolador principal, baseado no chip RP2040.
- **Display OLED SSD1306**: Conectado via I2C (pinos SDA=GP14, SCL=GP15) para exibir o valor do contador e a contagem de pressionamentos do Botão B.
- **Botão A (GPIO5)**: Configurado como entrada com pull-up interno, usado para iniciar/reiniciar a contagem.
- **Botão B (GPIO6)**: Configurado como entrada com pull-up interno, usado para registrar pressionamentos durante a contagem.

### Software

O código foi estruturado nos seguintes arquivos:

- `main.c`: Contém a lógica principal do contador decrescente, configurando interrupções para os botões, controlando a contagem, e atualizando o display OLED.
- `ssd1306_i2c.c` **e** `ssd1306_i2c.h`: Driver para o display OLED SSD1306, responsável por inicializar o display e desenhar strings.
- `ssd1306_font.h`: Define a fonte usada para renderizar caracteres no display.
- `ssd1306.h`: Declarações das funções do driver SSD1306.
- `CMakeLists.txt`: Arquivo de configuração para compilação do projeto com o Pico SDK.

### Lógica de Funcionamento

1. **Inicialização**:

   - O programa inicializa o I2C para comunicação com o display OLED, configura os pinos dos botões como entradas com pull-up, e habilita interrupções de borda de descida para detectar pressionamentos.
   - O display OLED é configurado com uma área de renderização de 128x64 pixels.

2. **Interrupções**:

   - **Botão A (GPIO5)**: Quando pressionado, reinicia o contador para 9, zera a contagem de pressionamentos do Botão B, ativa a contagem regressiva, e atualiza o display.
   - **Botão B (GPIO6)**: Durante a contagem regressiva (enquanto o contador está entre 9 e 0), cada pressionamento incrementa a contagem de Botão B e atualiza o display. Fora desse período, os pressionamentos são ignorados.

3. **Contagem Regressiva**:

   - A contagem é controlada verificando o tempo com a função `time_us_64()`. A cada segundo, o contador é decrementado, e o display é atualizado com o novo valor do contador e a contagem de Botão B.
   - Quando o contador atinge 0, a contagem regressiva é desativada, e o sistema entra em um estado "congelado", exibindo o valor 0 e a quantidade final de pressionamentos do Botão B.

4. **Exibição no Display**:

   - O display OLED mostra duas linhas:
     - Linha 1: "Counter: X" (onde X é o valor do contador, de 9 a 0).
     - Linha 2: "B Clicks: Y" (onde Y é a quantidade de pressionamentos do Botão B).
   - O display é atualizado a cada segundo durante a contagem ou quando o Botão B é pressionado.

5. **Debounce**:

   - Um atraso de 200 ms é aplicado após cada interrupção de botão para evitar múltiplas detecções devido ao efeito de bouncing.

### Como Executar

1. **Configuração do Hardware**:

   - Não é necessário realizar nenhuma conexão manual, pois os pinos já estão conectados através da configuração da placa **BitDogLab**.
   - Alimente o Pico via USB ou outra fonte de 3,3V/5V.

2. **Compilação e Gravação**:

   - Clone o repositório do Pico SDK (versão 1.5.1) e configure a variável de ambiente `PICO_SDK_PATH`.
   - Copie os arquivos do projeto (`main.c`, `ssd1306_i2c.c`, `ssd1306_i2c.h`, `ssd1306_font.h`, `ssd1306.h`, `CMakeLists.txt`) para um diretório.
   - Crie um diretório `build`, execute `cmake ..` e `make` para compilar.
   - Grave o arquivo `.uf2` gerado no Raspberry Pi Pico.

3. **Teste**:

   - Pressione o Botão A para iniciar a contagem regressiva a partir de 9.
   - Durante os 9 segundos, pressione o Botão B várias vezes e observe a contagem de pressionamentos no display.
   - Quando o contador atingir 0, verifique se o display mostra o valor 0 e a quantidade final de pressionamentos do Botão B.
   - Pressione o Botão A novamente para reiniciar o processo.

## Estrutura do Projeto

```
countdown_project/
├── main.c
|-- ws2818b.pio
├── inc/
│   ├── ssd1306_i2c.c
│   ├── ssd1306_i2c.h
│   ├── ssd1306_font.h
│   ├── ssd1306.h
├── CMakeLists.txt
```

## Dependências

- **Pico SDK 1.5.1**: Fornece as bibliotecas para o RP2040 (`pico_stdlib`, `hardware_i2c`).
- **Ferramentas de Compilação**:
  - CMake (versão 3.13 ou superior).
  - GNU Arm Embedded Toolchain (ex.: 10.3-2021.10).
  - Compilador C (ex.: `arm-none-eabi-gcc`).

## Notas

- O projeto foi otimizado para usar interrupções, garantindo respostas rápidas aos pressionamentos dos botões.
- O display OLED usa o driver SSD1306 fornecido, com suporte para renderização de strings em uma fonte de 8x8 pixels.
- O código inclui proteção contra bouncing dos botões e consome poucos recursos do RP2040.

Para dúvidas ou sugestões, consulte a documentação do Pico SDK ou entre em contato com o desenvolvedor.