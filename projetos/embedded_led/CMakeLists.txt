cmake_minimum_required(VERSION 3.13)

# Nome do projeto
project(embedded_led C CXX ASM)

# Defina o caminho do SDK do Pico
set(PICO_SDK_PATH "/home/rod/pico-sdk")

# Importe o SDK
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Inicialize o SDK
pico_sdk_init()

# Adicione o executável
add_executable(embedded_led
    app/main.c
    hal/hal_led.c
    drivers/led_embutido.c
)

# Inclua o diretório de headers
target_include_directories(embedded_led PRIVATE include)

# Link com bibliotecas necessárias
target_link_libraries(embedded_led
    pico_stdlib
    pico_cyw43_arch_none
)

# Geração de arquivos binários
pico_add_extra_outputs(embedded_led)
