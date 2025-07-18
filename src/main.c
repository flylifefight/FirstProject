#include "gd32f30x.h"
#include <stdio.h>
#include "main.h"
#include "bsp_systick.h"
#include "bsp_adc.h"
#include "bsp_timer.h"
#include "bsp_usart.h"
#include "bsp_flash.h"

/*!
    \brief      configure the different system clocks
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* enable GPIOA clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* enable GPIOB clock */
    rcu_periph_clock_enable(RCU_GPIOB);
    /* enable ADC0 clock */
    rcu_periph_clock_enable(RCU_ADC2);
    /* enable timer0 clock */
    rcu_periph_clock_enable(RCU_TIMER0);
    /* enable timer1 clock */
    rcu_periph_clock_enable(RCU_TIMER1);
    /* enable timer2 clock */
    rcu_periph_clock_enable(RCU_TIMER2);
    /* enable USART clock */
    rcu_periph_clock_enable(RCU_USART0);
    /* enable AF clock */
    rcu_periph_clock_enable(RCU_AF);
    /* enable DMA0 clock */
    rcu_periph_clock_enable(RCU_DMA0);
    rcu_periph_clock_enable(RCU_DMA1);
    /* config ADC clock */
    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV4);
}


/**
    \brief        configure the nested vectored interrupt controller
    \param[in]  none
    \param[out] none
    \retval     none
  */
void nvic_config(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
}


/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* system clocks configuration */
    rcu_config();
    /* NVIC configuration */
    nvic_config();
    /* systick configuration */
    systick_config(); 
    
    adc_config();
    usart0_config();
    
    //timer初始化顺序不能变更
    timer2_config();
    timer1_config();
    timer0_config();
    
    while(1){
//        usart0_transmit("hello world!!!\n\r", 16);
//        delay_1ms(1000);
    }
}

