#include "game.h"

void restart_game_objects_multi(int player_count, GameObject_t **player_object_arr) {
  restart_pipes();
  for (int i = 0; i < player_count; ++i) {
  }
  player_object_arr[0]->x = 75;
  player_object_arr[0]->y = 100;
  player_object_arr[1]->x = 75;
  player_object_arr[1]->y = 120;
  player_object_arr[2]->x = 75;
  player_object_arr[2]->y = 120;


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
void physics(GameObject_t *player_obj) {
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

void play(int player_count, int start_id, GameObject_t **player_object_arr) {
  

  printf("Playing\n");
  restart_game_objects_multi(player_count, player_object_arr);
  printf("Restart gameobj \n");
  redraw_game_multiplayer(player_count, player_object_arr);
  printf("Redraw\n");
  int clicked = 0;
  // get_start_click(&clicked);
  while (1) {
    for (int i = start_id; i < player_count; ++i) {
      if (player_object_arr[i]->health <= 0) continue;
          printf("Health check\n");
      int clicked = get_knob_click(&(player_object_arr[i]->knob_id), &(player_object_arr[i]->debounce));
      printf("click\n");
      if (clicked == 1) {
        player_object_arr[i]->acceleration_x += JUMP_FORCE;
      }

      physics(player_object_arr[i]);

      int check = check_player_lost(*player_object_arr[i]);
      if (check == -1 && player_object_arr[i]->health > 0) player_object_arr[i]->health--;
      if (check == 1 && player_object_arr[i]->health > 0) player_object_arr[i]->score++;
    }
    update_pipes();
    redraw_game_multiplayer(player_count, player_object_arr);
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

int check_player_lost(GameObject_t player_obj) {
  return check_hitbox_hit(player_obj);
}

int check_hitbox_hit(GameObject_t player) {
printf("Player %d x: %d y: %d\n",player.knob_id, player.x, player.y);
if (player.y > SCREEN_HEIGHT || player.y < 0) return -1;
  for(int i = 0; i < 3; ++i) {
    GameObject_t *pipe_top = &pipe_pool[i];
    GameObject_t *pipe_bottom = &pipe_pool[i+3]; 
    if(pipe_top->x > 65 && pipe_top->x < 69) return 1;
    if (!(player.x + player.img->w > pipe_top->x && player.x < pipe_top->x + pipe_top->img->w)) continue;  
    if (player.y < pipe_top->y + pipe_top->img->h) return -1;
    else if (player.y + player.img->h > pipe_bottom->y) return -1;
  }
  return 0;
}