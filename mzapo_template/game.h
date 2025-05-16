
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
#define GREEN_KNOB 1
#define BLUE_KNOB 0
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 320

// Bird jump/gravity settings
#define GRAVITY_FORCE 4
#define JUMP_PER_FRAME 5
#define JUMP_FORCE 30
typedef struct Pixel;
typedef struct Img;

typedef struct {
  int x;
  int y;
  int acceleration_x;
  Img *img;
  int debounce;
  int score;
  int health;
  int knob_id;
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

int check_player_lost(GameObject_t player_obj);
int play_singleplayer();
void restart_game_objects();
void restart_game_objects_multi(int player_id, GameObject_t **player_object_arr);
void restart_pipes();
void physics(GameObject_t *player_obj);
void update_pipes();
void play(int player_count, GameObject_t **player_object_arr);
int check_hitbox_hit(GameObject_t player);

#endif
