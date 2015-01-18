/*
 * service.c
 *
 *  Created on: 25.10.2014
 *      Author: BoBaHouD
 */

#include <st/service.h>

void service_clear_array(int16 array[], Uint16 size){
    int i;
    for (i = 0; i < size; i++)
        array[i] = 0;
}

