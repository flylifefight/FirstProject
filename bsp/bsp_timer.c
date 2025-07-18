#include "bsp_timer.h"
#include "gd32f30x.h"

uint16_t timer2PulseWidth = 20; //default 10us

/*!
    \brief      configure the TIMER0 peripheral: 2KHz interrupt
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer0_config(void)
{
    timer_parameter_struct timer_initpara;
	
    timer_deinit(TIMER0);

    /* TIMER0 configuration */
    timer_initpara.prescaler         = 1;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 499;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER0,&timer_initpara);
    
    
    timer_internal_trigger_as_external_clock_config(TIMER0,TIMER_SMCFG_TRGSEL_ITI1);
    timer_slave_mode_select(TIMER0, TIMER_SLAVE_MODE_EXTERNAL0);
    
    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER0);
    
    nvic_irq_enable(TIMER0_UP_IRQn, 1, 0);
    timer_interrupt_flag_clear(TIMER0, TIMER_INT_FLAG_UP);
    timer_interrupt_enable(TIMER0, TIMER_INT_UP);
    
    timer_enable(TIMER0);
}

/*!
    \brief      configure the TIMER0 interrupt callback
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer0_interrupt_callback(void)
{
    timer2_pulse_output(timer2PulseWidth);
}

/*!
    \brief      configure the TIMER1 peripheral: B10 PWMOUT 2MHz
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer1_config(void)
{
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_MAX, GPIO_PIN_10); 
    gpio_pin_remap_config(GPIO_TIMER1_FULL_REMAP,ENABLE);
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;
	
    timer_deinit(TIMER1);

    /* TIMER1 configuration */
    timer_initpara.prescaler         = 0;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 59;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER1,&timer_initpara);
    
    timer_channel_output_struct_para_init(&timer_ocintpara);

    /* CH2 configuration in PWM mode */
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

    timer_channel_output_config(TIMER1,TIMER_CH_2,&timer_ocintpara);

    timer_channel_output_pulse_value_config(TIMER1, TIMER_CH_2, 29);
    timer_channel_output_mode_config(TIMER1,TIMER_CH_2,TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER1,TIMER_CH_2,TIMER_OC_SHADOW_DISABLE);
    
    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER1);
    
    //master mode
    timer_master_output_trigger_source_select(TIMER1,TIMER_TRI_OUT_SRC_O2CPRE);
    timer_master_slave_mode_config(TIMER1,TIMER_MASTER_SLAVE_MODE_ENABLE); 
    /* auto-reload preload enable */
    timer_enable(TIMER1);
}

/*!
    \brief      configure the TIMER2 peripheral: A6 PWMOUT singlepulse init_value 1us
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer2_config(void)
{
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_MAX, GPIO_PIN_6); 
	
    timer_oc_parameter_struct timer_ocinitpara;
    timer_parameter_struct timer_initpara;
    timer_ic_parameter_struct timer_icinitpara;

    timer_deinit(TIMER2);

    /* TIMER2 configuration */
    timer_initpara.prescaler         = 0;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 65535;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER2,&timer_initpara);

    /* auto-reload preload disable */
    timer_auto_reload_shadow_disable(TIMER2);
    
    
    timer_internal_trigger_as_external_clock_config(TIMER2,TIMER_SMCFG_TRGSEL_ITI1);
    timer_slave_mode_select(TIMER2, TIMER_SLAVE_MODE_EXTERNAL0);

    timer_channel_output_struct_para_init(&timer_ocinitpara);
    /* CH1 configuration in OC PWM mode */
    timer_ocinitpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocinitpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocinitpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocinitpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocinitpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocinitpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    
    timer_channel_output_config(TIMER2,TIMER_CH_0,&timer_ocinitpara);

    timer_channel_output_pulse_value_config(TIMER2,TIMER_CH_0,19);
    timer_channel_output_mode_config(TIMER2,TIMER_CH_0,TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER2,TIMER_CH_0,TIMER_OC_SHADOW_DISABLE);
    
    timer_disable(TIMER2);
    nvic_irq_enable(TIMER2_IRQn, 0, 1);
    timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_CH0);
    timer_interrupt_enable(TIMER2, TIMER_INT_CH0);
}

/*!
    \brief      configure the TIMER2 pulse width
    \param[in]  pulse_width : pulse_width/120 us
    \param[out] none
    \retval     none
*/
void timer2_pulse_output(uint16_t pulse_width)
{
    timer_channel_output_pulse_value_config(TIMER2,TIMER_CH_0,pulse_width-1);
    timer_counter_value_config(TIMER2, 0);
    timer_enable(TIMER2);
}

/*!
    \brief      configure the TIMER2 pulse width by user
    \param[in]  pulse_width : pulse_width/120 us
    \param[out] none
    \retval     none
*/
void timer2_pulse_set(uint16_t pulse_width)
{
    timer2PulseWidth = pulse_width;
}

/*!
    \brief      configure the TIMER2 interrupt callback
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer2_interrupt_callback(void)
{
    timer_disable(TIMER2);
}


