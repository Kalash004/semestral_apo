#include "game.h"
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

int play_singleplayer();
int play_multiplayer();
void draw_stats();
void update_pipes();
int check_player_lost(GameObject_t *player_obj);
int check_multiplayer_lost();
int check_hitbox_hit(GameObject_t *player);
void redraw_game_singleplayer(unsigned int player1_score);
void redraw_game_multiplayer(unsigned int player1_score, unsigned int player2_score);
void restart_game_objects();
void restart_game_objects_multi();
void restart_pipes();
void physics(GameObject_t *player_obj, long long delta_time);

// All game logic was moved here from program()

void update_pipes() {
  for (int i = 0; i < 3; ++i) {
    GameObject_t *pipe_top = &pipe_pool[i];
    GameObject_t *pipe_bottom = &pipe_pool[i + 3];
    pipe_top->x -= 3;
    pipe_bottom->x -= 3;
    if (pipe_top->x < -100) {
      int rand_y = rand() % 190;
      pipe_top->y = -320 + rand_y;
      pipe_bottom->y = -320 + rand_y + GAP;
      pipe_top->x = 620;
      pipe_bottom->x = 620;
    }
  }
}

int check_player_lost(GameObject_t *player_obj) {
  return check_hitbox_hit(player_obj);
}

int check_multiplayer_lost() {
  int hit1 = check_hitbox_hit(bird_obj);
  int hit2 = check_hitbox_hit(bird_obj2);
  if (hit1 + hit2 == -2) return 3;
  if (hit1 == -1) return 1;
  if (hit2 == -1) return 2;
  return 0;
}

int check_hitbox_hit(GameObject_t *player) {
  if (player->y > SCREEN_HEIGHT || player->y < 0) return -1;
  for (int i = 0; i < 3; ++i) {
    GameObject_t *pipe_top = &pipe_pool[i];
    GameObject_t *pipe_bottom = &pipe_pool[i + 3];
    if (pipe_top->x > 65 && pipe_top->x < 69) return 1;
    if (!(player->x + player->img->w > pipe_top->x && player->x < pipe_top->x + pipe_top->img->w)) continue;
    if (player->y < pipe_top->y + pipe_top->img->h) return -1;
    else if (player->y + player->img->h > pipe_bottom->y) return -1;
  }
  return 0;
}

void physics(GameObject_t *player_obj, long long delta_time) {
  player_obj->acceleration_x -= GRAVITY_FORCE * 1;
  if (player_obj->acceleration_x > 0) {
    player_obj->acceleration_x -= JUMP_PER_FRAME * 1;
  }
  if (player_obj->acceleration_x < -10) {
    player_obj->acceleration_x = -10;
  } else if (player_obj->acceleration_x > 30) {
    player_obj->acceleration_x = 30;
  }
  player_obj->y -= player_obj->acceleration_x * 1;
}

void restart_game_objects() {
  restart_pipes();
  bird_obj->x = 75;
  bird_obj->y = 145;
  bird_obj->img = bird1;
}

void restart_game_objects_multi() {
  bird_obj->img = bird_red;
  bird_obj2->img = bird_blue;
  restart_pipes();
  bird_obj2->x = 75;
  bird_obj2->y = 160;
  bird_obj->x = 75;
  bird_obj->y = 120;
}

void restart_pipes() {
  for (int i = 0; i < 3; ++i) {
    GameObject_t *top = &pipe_pool[i];
    GameObject_t *btm = &pipe_pool[i + 3];
    top->x = 480 + (i * (80 + 160));
    btm->x = 480 + (i * (80 + 160));
    top->y = -200;
    btm->y = top->y + GAP;
    top->img = top_pipe;
    btm->img = btm_pipe;
  }
}
