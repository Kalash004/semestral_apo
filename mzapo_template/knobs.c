#include "knobs.h"

int get_knob_rotation() {
    static int old_value = -1;
    uint8_t current_value = (*(volatile uint32_t*)(membase + SPILED_REG_KNOBS_8BIT_o) >> 16) & 0xff;

    if (old_value == -1) {
        old_value = current_value;
        return 0;
    }

    int diff = (current_value - old_value + 256) % 256;

    // Jitter
    if (diff < 3 || diff > 253) {
        return 0; 
    }


    old_value = current_value;

    return (diff < 128) ? 1 : -1;
}

int get_knob_click(int knob_num, int *debounce) {
  if (knob_num != 0 && knob_num != 2) return 0;
  uint8_t current_value;
  if(knob_num == RED_KNOB) {
    current_value = (*(volatile uint32_t*)(membase + SPILED_REG_KNOBS_8BIT_o) >> (24 + RED_KNOB)) & 0xff;
  } else if(knob_num == BLUE_KNOB) {
    current_value = (*(volatile uint32_t*)(membase + SPILED_REG_KNOBS_8BIT_o) >> (24 + BLUE_KNOB)) & 0x01;
  
  }
  if(current_value == 1 && *debounce == 1) {
    *debounce = 0;
    return 1;
  } else if (current_value == 0) {
    *debounce = 1;
    return 0;
  } else {
    return 0;
  }
  
  return 0;
}