#ifndef BSP_TIMER_H
#define BSP_TIMER_H

#include <stdint.h>

void timer2_interrupt_callback(void);
void timer2_pulse_set(uint16_t pulse_width);
void timer2_pulse_output(uint16_t pulse_width);
void timer2_config(void);
void timer1_config(void);
void timer0_interrupt_callback(void);
void timer0_config(void);

#endif /* BSP_TIMER_H */