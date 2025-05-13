
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

#include "img.h"
// Gap between pipes
#define GAP 450
#define RED_KNOB 2
#define BLUE_KNOB 0
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 320

// Bird jump/gravity settings
#define GRAVITY_FORCE 4
#define JUMP_PER_FRAME 5
typedef struct Pixel;
typedef struct Img;

typedef struct {
  int x;
  int y;
  int acceleration_x;
  Img *img;
} GameObject_t;

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

int check_player_lost(GameObject_t *player_obj);
int play_singleplayer();
void restart_game_objects();
void restart_game_objects_multi();
void restart_pipes();
void physics(GameObject_t *player_obj, long long delta_time);
void update_pipes();
int play_multiplayer();
int check_multiplayer_lost();
int check_hitbox_hit(GameObject_t *player);

#endif
