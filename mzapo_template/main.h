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
#include "peripherals.h"
#include "stats.h"
#include "header.h"
#include "serialize_lock.h"

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200112L
#endif
#define RED_KNOB 2
#define BLUE_KNOB 0
#define GREEN_KNOB 1


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
unsigned int highest_player_score;
unsigned int last_single_score;
unsigned int last_multi_scores[3];
unsigned int all_pipes_passed;

void main_menu(options_t *opts, void *lcd);
void serialize();
void program();
void exit_game();
void add_to_player_arr(GameObject_t **player_arr, Img *bird_img, int knob_id, int x, int y, int player_count);
int choose_player_knobs(GameObject_t **player_arr);
void choose_singleplayer_knob(GameObject_t **player_arr);

#endif
