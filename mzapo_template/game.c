#include "game.h"

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
    GameObject_t *btm = &pipe_pool[i+3];
    top->x = 480 + (i * (80 + 160));
    btm->x = 480 + (i * (80 + 160));
    top->y = -200;
    btm->y = top->y + GAP;
    top->img = top_pipe;
    btm->img = btm_pipe;
  }
}


// give last time and current time
void physics(GameObject_t *player_obj, long long delta_time) {
  printf("Delta time: %li\n",delta_time);
  player_obj->acceleration_x -= GRAVITY_FORCE * 1;
  if(player_obj->acceleration_x > 0) {
    player_obj->acceleration_x -= JUMP_PER_FRAME * 1; 
  }
  if(player_obj->acceleration_x < -10) {
    player_obj->acceleration_x = -10;
  } else if(player_obj->acceleration_x > 30) {
    player_obj->acceleration_x = 30;
  }
  player_obj->y -= player_obj->acceleration_x * 1;
}

void update_pipes() {
  for(int i = 0; i < 3; ++i) {
    GameObject_t *pipe_top = &pipe_pool[i];
    GameObject_t *pipe_bottom = &pipe_pool[i+3];
    pipe_top->x -= 3;
    pipe_bottom->x -= 3;
    if(pipe_top->x < -100) {
      int rand_y = rand() % 190;
      pipe_top->y = -320 + rand_y;
      pipe_bottom->y = -320 + rand_y + GAP;
      pipe_top->x = 620;
      pipe_bottom->x = 620;
    }
  }
}

void play(int player_count, int start_id, unsigned int *scores_arr, int *health_arr, int *debounce_holder_arr, GameObject_t **player_object_arr, int *knobs_identif_arr) {
  restart_game_objects_multi();
  redraw_game_multiplayer(player_count, scores_arr, player_object_arr);
  int clicked = 0;
  get_start_click(&clicked);
  while (1) {
    for (int i = start_id; i < player_count; ++i) {
      if (health_arr[i] <= 0) continue;
      int clicked = get_knob_click(knobs_identif_arr[i], &debounce_holder_arr[i]);
      if (clicked == 1) {
        player_object_arr[i]->acceleration_x += JUMP_FORCE;
      }

      physics(&player_object_arr[i]);

      int check = check_player_lost(&player_object_arr[i]);
      if (check == -1 && health_arr[i] > 0) health_arr[i]--;
      if (check == 1 && health > 0) scores_arr[i]++;
    }
    update_pipes();
    redraw_game_multiplayer(player_count, scores_arr, player_object_arr);
  }
}

void get_start_click(int *clicked) {
  int *rebounce;
  *clicked = 0;
  sleep(1);
  while (*clicked == 0) {
    *clicked = get_knob_click(RED_KNOB, rebounce);
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
  for(int i = 0; i < 3; ++i) {
    GameObject_t *pipe_top = &pipe_pool[i];
    GameObject_t *pipe_bottom = &pipe_pool[i+3]; 
    if(pipe_top->x > 65 && pipe_top->x < 69) return 1;
    if (!(player->x + player->img->w > pipe_top->x && player->x < pipe_top->x + pipe_top->img->w)) continue;  
    if (player->y < pipe_top->y + pipe_top->img->h) return -1;
    else if (player->y + player->img->h > pipe_bottom->y) return -1;
  }
  return 0;
}