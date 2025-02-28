/*
Model světel sanitky s použiťí rassberry pi pico W a 12 ledek s 12 tranzistorama
autor: Jonáš Erlebach <jonaserlebach@email.cz>
*/
#include "pico/stdlib.h"
#include <stdint.h>

uint8_t led_map[12] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

uint32_t plna_maska = 1 | (1<<1) | (1<<2) | (1<<3) | (1<<4) | (1<<5) | (1<<6) | (1<<7) | (1<<8) | (1<<9) | (1<<10) | (1<<11);

uint32_t masky[10] = {};

int main() {
    for(uint8_t i = 0; i < 12; i++){
        gpio_init(led_map[i]);
        gpio_set_dir(led_map[i], GPIO_OUT);
    }


    uint8_t iteration = 0;
    while (1) {
        if(iteration <= 3){
            uint32_t first_three = 1 | (1<<1) | (1<<2);
            for(uint8_t i = 1; i < 19; i++){
                uint32_t new_mask = 0;
                if(i < 10){
                    new_mask = first_three << i ;
                }else{
                    new_mask = (first_three<<9) >> (i-9);
                }
                gpio_set_mask(new_mask);
                sleep_ms(50);
                gpio_clr_mask(new_mask);
            }
            iteration++;
        }else{
            for(uint8_t i = 0; i < 3; i++){
                sleep_ms(25);
                gpio_set_mask(plna_maska);
                sleep_ms(25);
                gpio_clr_mask(plna_maska);
            }
            iteration = 0;
        }
    }
}
