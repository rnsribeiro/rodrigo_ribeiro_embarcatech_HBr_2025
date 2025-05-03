#include "adc.h"

float adc_to_celsius(uint16_t adc_val) {
    // Converte o valor ADC para tensão (Vref = 3.3V, 12 bits)
    float voltage = (adc_val * 3.3f) / 4095.0f;
    
    // Fórmula da documentação: T = 27 - ((V - 0.706) / 0.001721)
    float temperature = 27.0f - ((voltage - 0.706f) / 0.001721f);
    
    return temperature;
}