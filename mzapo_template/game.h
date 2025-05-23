
#pragma once
#ifndef GAME_H
#define GAME_H

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

#include "stats.h"
#include "graphics.h"
#include "img.h"
#include "header.h"
#include "peripherals.h"

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })
// Gap between pipes
#define GAP 450
#define RED_KNOB 2
#define GREEN_KNOB 1
#define BLUE_KNOB 0
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 320

// Bird jump/gravity settings
#define GRAVITY_FORCE 4
#define JUMP_PER_FRAME 5
#define JUMP_FORCE 30

// Pipe presets
#define PIPE_SCORE_RANGE 60 // when pipe passes this x value, player gets +1 score
#define PIPE_LEFTMOST_POSITION -100
#define PIPE_RIGHTMOST_POSITION 620
#define PIPE_GAP_YRANGE 190
#define PIPE_TOPMOST_POSITION -320
#define PIPE_SHIFT 3
#define PIPE_GAP_XRANGE 240

extern unsigned char *membase;
extern uint16_t origin_fb[480][320];
extern void *origin_lcd;
extern Img *background;
extern Img *bird1;
extern Img *bird_red;
extern Img *bird_blue;
extern Img *top_pipe;
extern Img *btm_pipe;
extern GameObject_t *pipe_pool;
extern GameObject_t *bird_obj;
extern GameObject_t *bird_obj2;
extern unsigned int highest_player_score;

int check_player_lost(GameObject_t player_obj);
int play_singleplayer();
void restart_game_objects();
void restart_game_objects_multi(int player_id, GameObject_t **player_object_arr);
void restart_pipes();
void physics(GameObject_t *player_obj);
void update_pipes();
void add_multiplayer_score(GameObject_t **player_object_arr, int player_count);
void play(int player_count, GameObject_t **player_object_arr);
int check_hitbox_hit(GameObject_t player);
void process_stats(GameObject_t **player_object_arr, int player_count);
void get_start_click();
#endif
