#include "peripherals.h"
/*
This module provides functions to read the state of rotary knobs and LED lights. 
It supports detecting knob rotation and clicks with debouncing, and allows setting colors on two RGB LEDs.
*/


static uint8_t old_value;
void knob_init() {
  old_value = (*(volatile uint32_t*)(membase + SPILED_REG_KNOBS_8BIT_o) >> 16) & 0xff;
}

int get_knob_rotation() {
    uint8_t current_value = (*(volatile uint32_t*)(membase + SPILED_REG_KNOBS_8BIT_o) >> 16) & 0xff;

    int8_t diff = (current_value - old_value);
    
    if(diff > 3) {
      old_value += 4;
      return 1;
    } else if(diff < -3) {
      old_value -= 4;
      return -1;
    } else { // ignore noise
      return 0;
    }
    // if the difference is more than 3, the knob clockwise, else if less than -3, rotate counter-clockwise
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