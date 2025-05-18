#include "peripherals.h"
/*
This module provides functions to read the state of rotary knobs and LED lights. 
It supports detecting knob rotation and clicks with debouncing, and allows setting colors on two RGB LEDs.
*/

int get_knob_rotation() {
    static int old_value = -1;
    uint8_t current_value = (*(volatile uint32_t*)(membase + SPILED_REG_KNOBS_8BIT_o) >> 16) & 0xff;

    // initialize old_value and return 0 (no rotation) on 1st call
    if (old_value == -1) {
        old_value = current_value;
        return 0;
    }

    // %256 to handle overflow
    int diff = (current_value - old_value + 256) % 256;

    // ignore noise
    if (diff < 3 || diff > 253) {
        return 0; 
    }


    old_value = current_value;

    // if the difference is less than 128, the knob clockwise, otherwise counter-clockwise
    return (diff < 128) ? 1 : -1;
}

int get_knob_click(int knob_num, int *debounce) {
  if (knob_num < 0 && knob_num > 2) return 0;
  uint8_t current_value;
  current_value = (*(volatile uint32_t*)(membase + SPILED_REG_KNOBS_8BIT_o) >> (24 + knob_num)) & 0x01;
  if(current_value == 1 && *debounce == 1) {
    *debounce = 0;
    return 1;
  } else if (current_value == 0) {
    *debounce = 1;
    return 0;
  }
  return 0;
}

// led lights indication
void led_draw(int led_num, uint32_t color) {
  if(led_num == 0) {
      *(volatile uint32_t*)(membase + SPILED_REG_LED_RGB1_o) = color;
  } else {
      *(volatile uint32_t*)(membase + SPILED_REG_LED_RGB2_o) = color;
  }
}