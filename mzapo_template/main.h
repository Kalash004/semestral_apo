#pragma once
#ifndef MAIN_H
#define MAIN_H

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

#include "img.h"
#include "game.h"
#include "graphics.h"
#include "utils.h"
#include "peripherals.h"



#define _POSIX_C_SOURCE 200112L


typedef struct {
  int game_mode;
  char p;  
} options_t;

typedef struct Pixel;  // Forward declaration
typedef struct Imgs;  // Forward declaration
typedef struct options_t;  // Forward declaration
typedef struct GameObject_t;  // Forward declaration

unsigned char *membase;
uint16_t origin_fb[480][320];
void *origin_lcd;
Img *background;
Img *bird1;
Img *bird_red;
Img *bird_blue;
Img *top_pipe;
Img *btm_pipe;
GameObject_t *pipe_pool;
GameObject_t *bird_obj;
GameObject_t *bird_obj2;
unsigned int highest_player_score;

void main_menu(options_t *opts, void *lcd);
void serialize();
void program();
void exit_game();



#endif
