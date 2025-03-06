/*
Model světel sanitky s použiťí rassberry pi pico W a 12 ledek s 12 tranzistorama
autor: Jonáš Erlebach <jonaserlebach@email.cz>
*/
#include "pico/stdlib.h"
#include <stdint.h>
#include "hardware/structs/iobank0.h"

uint8_t led_map[12] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

uint32_t plna_maska = 1 | (1<<1) | (1<<2) | (1<<3) | (1<<4) | (1<<5) | (1<<6) | (1<<7) | (1<<8) | (1<<9) | (1<<10) | (1<<11);

uint32_t masky[10] = {};

uint32_t casy[4] = {10, 50, 100, 250};
int8_t casy_index = 0;

const uint8_t button_pin_down = 14;
const uint8_t button_pin_up = 15;

uint32_t gpio_get_events(uint gpio)
{
    int32_t mask = 0xF << 4 * ( gpio % 8 );
    return (iobank0_hw->intr[gpio / 8] & mask) >> 4 * ( gpio % 8 );
}

void gpio_clear_events(uint gpio, uint32_t events)
{
    gpio_acknowledge_irq(gpio, events);
}

int main() {
    for(uint8_t i = 0; i < 12; i++){
        gpio_init(led_map[i]);
        gpio_set_dir(led_map[i], GPIO_OUT);
    }
    //nastavi gpio pin 14 jako button který bude sňižovat čas
    gpio_set_function( button_pin_down, GPIO_FUNC_SIO );
    gpio_set_dir( button_pin_down, false );
    gpio_pull_up( button_pin_down );
    gpio_clear_events( button_pin_down, GPIO_IRQ_EDGE_FALL );

    //nastavi gpio pin 15 jako button který bude zvyšovat čas
    gpio_set_function( button_pin_up, GPIO_FUNC_SIO );
    gpio_set_dir( button_pin_up, false );
    gpio_pull_up( button_pin_up );
    gpio_clear_events( button_pin_up, GPIO_IRQ_EDGE_FALL );

    uint8_t iteration = 0;
    while (1) {
        // checkne jestli se stal event na pinu 14
        int32_t event_down = gpio_get_events( button_pin_down );
        gpio_clear_events( button_pin_down, GPIO_IRQ_EDGE_FALL );
        if(event_down & GPIO_IRQ_EDGE_FALL){
            if(casy_index-1 != -1){
                casy_index--;
            }
        }
        
        // checkne jestli se stal event na pinu 15
        int32_t event_up = gpio_get_events( button_pin_up );
        gpio_clear_events( button_pin_up, GPIO_IRQ_EDGE_FALL );
        if(event_up & GPIO_IRQ_EDGE_FALL){
            if(casy_index+1 != 4){
                casy_index++;
            }
        }

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
                sleep_ms(casy[casy_index]);
                gpio_clr_mask(new_mask);
            }
            iteration++;
        }else{
            for(uint8_t i = 0; i < 3; i++){
                sleep_ms(casy[casy_index]/2);
                gpio_set_mask(plna_maska);
                sleep_ms(casy[casy_index]/2);
                gpio_clr_mask(plna_maska);
            }
            iteration = 0;
        }
    }
}
