#pragma once
#ifndef UTILS_H
#define UTILS_H

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
#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

extern unsigned char *membase;
extern uint16_t origin_fb[480][320];
extern void *origin_lcd;

void debug_print(char *pattern, ...);
int char_width(int ch, font_descriptor_t *font);
#endif
