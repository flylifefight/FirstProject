#ifndef BSP_ADC_H
#define BSP_ADC_H

#include <stdint.h>

void adc_value_get(uint16_t* g_value, uint16_t num);
void adc_collect_complete_callback(void);
void adc_start_collect(void);
void adc_config(void);

#endif /* BSP_ADC_H */