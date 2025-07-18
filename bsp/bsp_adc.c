#include "bsp_adc.h"
#include "bsp_systick.h"
#include "gd32f30x.h"
#include "string.h"

#define MAX_ADC_COLLECT_NUM 600
uint16_t adcValue[MAX_ADC_COLLECT_NUM] = {0};
uint8_t adcCollectComplete = 0; //0:nocomplete  1:completed

/*!
    \brief      configure the ADC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_config(void)
{
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_3); 
    
    /* ADC_DMA_channel configuration */
    dma_parameter_struct dma_data_parameter;
    
    dma_deinit(DMA1, DMA_CH4);
    /* initialize DMA single data mode */
    dma_data_parameter.periph_addr  = (uint32_t)(&ADC_RDATA(ADC2));
    dma_data_parameter.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_data_parameter.memory_addr  = (uint32_t)(&adcValue[0]);
    dma_data_parameter.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_data_parameter.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_data_parameter.memory_width = DMA_MEMORY_WIDTH_16BIT;  
    dma_data_parameter.direction    = DMA_PERIPHERAL_TO_MEMORY;
    dma_data_parameter.number       = MAX_ADC_COLLECT_NUM;
    dma_data_parameter.priority     = DMA_PRIORITY_HIGH;
    dma_init(DMA1, DMA_CH4, &dma_data_parameter);
    
    nvic_irq_enable(DMA1_Channel3_Channel4_IRQn, 0, 0);
    
    dma_interrupt_flag_clear(DMA1, DMA_CH4, DMA_INT_FLAG_FTF);
    dma_interrupt_enable(DMA1, DMA_CH4, DMA_INT_FTF);
    
    
    /* ADC mode config */
    adc_mode_config(ADC_MODE_FREE); 
    /* ADC special function config */
    adc_special_function_config(ADC2, ADC_SCAN_MODE, ENABLE);
    adc_special_function_config(ADC2, ADC_CONTINUOUS_MODE, DISABLE);  
    /* ADC data alignment config */
    adc_data_alignment_config(ADC2, ADC_DATAALIGN_RIGHT);

    /* ADC channel length config */
    adc_channel_length_config(ADC2, ADC_REGULAR_CHANNEL, 1);
    /* ADC regular channel config */
    adc_regular_channel_config(ADC2, 0, ADC_CHANNEL_3, ADC_SAMPLETIME_1POINT5);
    /* ADC trigger config */
    adc_external_trigger_source_config(ADC2, ADC_REGULAR_CHANNEL, ADC2_EXTTRIG_REGULAR_T1_CH2); 
    /* ADC external trigger enable */
    adc_external_trigger_config(ADC2, ADC_REGULAR_CHANNEL, ENABLE);
    
    adc_enable(ADC2);
    
    delay_1ms(1);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC2);
    
    
}

/*!
    \brief      start ADC COLLECT
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_start_collect(void)
{
    
    dma_memory_address_config(DMA1, DMA_CH4, (uint32_t)(&adcValue[0]));
    dma_transfer_number_config(DMA1, DMA_CH4, MAX_ADC_COLLECT_NUM);
    
    dma_channel_enable(DMA1, DMA_CH4);
    adc_dma_mode_enable(ADC2);
    adcCollectComplete = 0;
}


/*!
    \brief      ADC COLLECT Complete Callback
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_collect_complete_callback(void)
{
    adcCollectComplete = 1;
    adc_dma_mode_disable(ADC2);
    dma_channel_disable(DMA1, DMA_CH4);
}

/*!
    \brief      ADC Value Get
    \param[in]  g_value:array_ptr num:length
    \param[out] none
    \retval     none
*/
void adc_value_get(uint16_t* g_value, uint16_t num)
{
    if(num > MAX_ADC_COLLECT_NUM) 
    {
        memcpy((uint8_t*)g_value, (uint8_t*)adcValue, MAX_ADC_COLLECT_NUM*2);
    }
    else
    {
        memcpy((uint8_t*)g_value, (uint8_t*)adcValue, num*2);
    }
}


