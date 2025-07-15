#ifndef BSP_USART_H
#define BSP_USART_H

#include <stdint.h>

void usart0_transmit_interrupt_callback(void);
void usart0_receive_interrupt_callback(void);
void reset_receive(void);
uint8_t usart0_transmit(uint8_t* data, uint16_t num);
void usart0_config(void);

#endif /* BSP_USART_H */