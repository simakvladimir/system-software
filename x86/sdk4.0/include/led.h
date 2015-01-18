/*
 * led.h
 *
 *  Created on: 05.03.2014
 *      Author: BoBaHouD
 */

#ifndef LED_H_
#define LED_H_

#include <types.h>

#define LED_RED_PIO_PIN 24
#define LED_YELLOW_PIO_PIN 25

void led_red_init();
void led_yellow_init();
void led_green_init();

void led_red_set_state( uint8_t state );
void led_yellow_set_state( uint8_t state );
void led_green_set_state( uint8_t state );

#endif /* LED_H_ */
