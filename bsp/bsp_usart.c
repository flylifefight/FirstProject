#include "bsp_usart.h"
#include "gd32f30x.h"
#include "string.h"

#define MAX_USART_BUFFER_NUM 200
uint8_t receiveBuffer[MAX_USART_BUFFER_NUM] = {0};
uint8_t transmitBuffer[MAX_USART_BUFFER_NUM] = {0};

/*!
    \brief      configure USART
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usart0_config(void)
{
    /* connect port to USARTx_Tx */
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);

    /* connect port to USARTx_Rx */
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
    
    /* configure USART */
    usart_deinit(USART0);
    usart_baudrate_set(USART0, 115200U);
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
    usart_enable(USART0);
   
    dma_parameter_struct dma_init_struct;
    /* deinitialize DMA channel3(USART0 tx) */
    dma_deinit(DMA0, DMA_CH3);
    dma_struct_para_init(&dma_init_struct);
    dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_addr = (uint32_t)(&transmitBuffer[0]);
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number = 0;
    dma_init_struct.periph_addr = ((uint32_t)&USART_DATA(USART0));
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA0, DMA_CH3, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH3);
    dma_memory_to_memory_disable(DMA0, DMA_CH3);
    /* enable DMA channel3 */
    dma_channel_enable(DMA0, DMA_CH3);
    /* deinitialize DMA channel4 (USART0 rx) */
    dma_deinit(DMA0, DMA_CH4);
    dma_struct_para_init(&dma_init_struct);
    dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.memory_addr = (uint32_t)(&receiveBuffer[0]);
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number = MAX_USART_BUFFER_NUM;
    dma_init_struct.periph_addr = ((uint32_t)&USART_DATA(USART0));
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA0, DMA_CH4, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH4);
    dma_memory_to_memory_disable(DMA0, DMA_CH4);
    /* enable DMA channel4 */
    dma_channel_enable(DMA0, DMA_CH4);
    
    //DMA发送完成中断
    nvic_irq_enable(DMA0_Channel3_IRQn, 1, 1);
    dma_interrupt_flag_clear(DMA0, DMA_CH3, DMA_INT_FLAG_FTF);
    dma_interrupt_enable(DMA0, DMA_CH3, DMA_INT_FTF);
    //串口接收空闲中断
    nvic_irq_enable(USART0_IRQn, 1, 1);
    usart_interrupt_enable(USART0, USART_INT_IDLE);
    usart_dma_transmit_config(USART0, USART_TRANSMIT_DMA_ENABLE);
    usart_dma_receive_config(USART0, USART_RECEIVE_DMA_ENABLE);
}

/*!
    \brief      configure the USART0 interrupt callback
    \param[in]  data:array_ptr num:length
    \param[out] success or failure
    \retval     none
*/
uint8_t usart0_transmit(uint8_t* data, uint16_t num)
{
    if(num > MAX_USART_BUFFER_NUM || dma_transfer_number_get(DMA0, DMA_CH3) != 0)
    {
        return 0;
    }
    
    memcpy(transmitBuffer, data, num);
    
    dma_channel_disable(DMA0, DMA_CH3);
    dma_memory_address_config(DMA0, DMA_CH3, (uint32_t)(&transmitBuffer[0]));
    dma_transfer_number_config(DMA0, DMA_CH3, num);
    dma_channel_enable(DMA0, DMA_CH3);
    return 1;
}    

/*!
    \brief      configure reset receive
    \param[in]  none
    \param[out] none
    \retval     none
*/
void reset_receive(void)
{
    dma_channel_disable(DMA0, DMA_CH4);
    /* deinitialize DMA channel4 (USART0 rx) */
    dma_memory_address_config(DMA0, DMA_CH4, (uint32_t)(&receiveBuffer[0]));
    dma_transfer_number_config(DMA0, DMA_CH4, MAX_USART_BUFFER_NUM);
    dma_channel_enable(DMA0, DMA_CH4);
}

/*!
    \brief      configure usart0 dma transmit complete
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usart0_transmit_interrupt_callback(void)
{
    if(RESET != dma_interrupt_flag_get(DMA0, DMA_CH3, DMA_INT_FLAG_FTF)) {
        //发送完成编码
        
        dma_interrupt_flag_clear(DMA0, DMA_CH3, DMA_INT_FLAG_FTF);
    }
}

/*!
    \brief      configure usart0 receive complete
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usart0_receive_interrupt_callback(void)
{
    if(RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_IDLE)){
        /* clear IDLE flag */
        usart_data_receive(USART0);
        //接收完成编码
        //接收啥发送啥
        uint16_t len = MAX_USART_BUFFER_NUM-dma_transfer_number_get(DMA0, DMA_CH4);
        reset_receive();
        usart0_transmit(receiveBuffer,len);
        usart_interrupt_flag_clear(USART0, USART_INT_FLAG_TC);
    }
}
