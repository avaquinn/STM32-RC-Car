/*
 * bt.h
 *
 *  Created on: May 4, 2026
 *      Author: avais
 */

#ifndef BT_H_
#define BT_H_

#define BT_TIMEOUT_MS 500

uint8_t UART1_char_available(void);
void UART1_init(void);
char UART1_read_char(void);
void UART1_write_char(char c);
void UART1_print(char *str);


#endif /* BT_H_ */
