cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Configuração para extensão VS Code (não editar)
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 1.5.1)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()

# Define a placa como Pico
set(PICO_BOARD/pico CACHE STRING "Board type")

# Inclui o SDK do Pico
include(pico_sdk_import.cmake)

# Nome do projeto
project(sensor_temp_internal C CXX ASM)

# Inicializa o SDK
pico_sdk_init()

# Adiciona o executável com os arquivos fonte
add_executable(sensor_temp_internal
    main.c
    inc/ssd1306_i2c.c
)

# Define nome e versão do programa
pico_set_program_name(sensor_temp_internal "sensor_temp_internal")
pico_set_program_version(sensor_temp_internal "0.1")

# Habilita/desabilita saída USB/UART
pico_enable_stdio_uart(sensor_temp_internal 0)
pico_enable_stdio_usb(sensor_temp_internal 1)

# Adiciona bibliotecas padrão e necessárias
target_link_libraries(sensor_temp_internal
    hardware_adc 
    hardware_pwm
    pico_stdlib
    hardware_i2c
)

# Define diretórios de inclusão
target_include_directories(sensor_temp_internal PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/inc
)

# Gera arquivos extras (ex.: .uf2)
pico_add_extra_outputs(sensor_temp_internal)