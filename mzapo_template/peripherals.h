#pragma once
#ifndef PERIPHERALS_H
#define PERIPHERALS_H

#include <stdarg.h> 
#include "mzapo_regs.h"
#include "mzapo_phys.h"
#include "mzapo_parlcd.h"
#include "font_types.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#define RED_KNOB 2
#define GREEN_KNOB 1
#define BLUE_KNOB 0

#define SPILED_REG_BASE_PHYS  0x43c40000
#define SPILED_REG_SIZE       0x00004000

#define SPILED_REG_LED_LINE_o           0x004
#define SPILED_REG_LED_RGB1_o           0x010
#define SPILED_REG_LED_RGB2_o           0x014
#define SPILED_REG_LED_KBDWR_DIRECT_o   0x018

#define SPILED_REG_KBDRD_KNOBS_DIRECT_o 0x020
#define SPILED_REG_KNOBS_8BIT_o         0x024


extern unsigned char *membase;
extern uint16_t origin_fb[480][320];
extern void *origin_lcd;

int get_knob_rotation();
int get_knob_click(int knob_num, int *debounce);
void led_draw(int led_num, uint32_t color);
void knob_init();
#endif