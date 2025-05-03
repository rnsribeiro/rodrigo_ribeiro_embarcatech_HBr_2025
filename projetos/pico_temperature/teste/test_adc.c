#include "unity.h"
#include "adc.h"

void setUp(void) {}
void tearDown(void) {}

void test_adc_to_celsius_known_value(void) {
    // Teste para 0.706V, que deve resultar em 27°C
    // 0.706V corresponde a (0.706 / 3.3) * 4095 ≈ 877 (valor ADC)
    uint16_t adc_val = 877;
    float expected = 27.0f;
    float result = adc_to_celsius(adc_val);
    
    // Verifica se o resultado está dentro da margem de erro (±0.1°C)
    TEST_ASSERT_FLOAT_WITHIN(0.1f, expected, result);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_adc_to_celsius_known_value);
    return UNITY_END();
}