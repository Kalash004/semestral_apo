#pragma once
#ifndef IMG_H
#define IMG_H

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

#include "header.h"

extern unsigned char *membase;
extern uint16_t origin_fb[480][320];
extern void *origin_lcd;

Img* ppm_load_image(char *path);
uint32_t convert_rgb_to_hexa(Pixel rgb);

#endif
