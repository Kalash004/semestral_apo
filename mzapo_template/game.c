#include "game.h"

void restart_game_objects_multi(int player_count, GameObject_t **player_object_arr) {
  restart_pipes();
  for (int i = 0; i < player_count; ++i) {
    player_object_arr[i]->health = 1;
    player_object_arr[i]->x = 75;
    player_object_arr[i]->y = 80 + ((i+1) * 20) + (30 / player_count);  
    player_object_arr[i]->score = 0;
  }
  for(int i = player_count; i < 3; ++i) {
    player_object_arr[i]->score = 0;
  }
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

void add_multiplayer_score(GameObject_t **player_object_arr, int player_count) {
  unsigned int score[3] = {0};
  memset(score, 0, 3 * sizeof(unsigned int));  
  for(int i = 0; i < player_count; ++i) {
    GameObject_t *player_obj = player_object_arr[i];
    score[player_obj->knob_id] = player_obj->score;
  }
  add_text_to_buffer("Bird score: Red %u | Green %u | Blue %u", 0, 0, score[2], score[1], score[0]);
}

void play(int player_count, GameObject_t **player_object_arr) {
  restart_game_objects_multi(player_count, player_object_arr);
  redraw_game_multiplayer(player_count, player_object_arr, 0);
  get_start_click();
  led_draw(0,0x000000);
  led_draw(1,0x000000);
  while (1) {
    for (int i = 0; i < player_count; ++i) {
      if (player_object_arr[i]->health <= 0) continue;
      int clicked = get_knob_click(player_object_arr[i]->knob_id, &(player_object_arr[i]->debounce));
      if (clicked == 1) {
        player_object_arr[i]->acceleration_x += JUMP_FORCE;
      }

      physics(player_object_arr[i]);

      int check = check_player_lost(*player_object_arr[i]);
      if (check == -1 && player_object_arr[i]->health > 0) player_object_arr[i]->health--;
      if (check == 1 && player_object_arr[i]->health > 0) player_object_arr[i]->score++;
    }
    int health_sum = 0;
    for (int i = 0; i < player_count; ++i) {
      health_sum += player_object_arr[i]->health;
    }
    if (health_sum <= 0) {
      break;
    }
    update_pipes();
    redraw_game_multiplayer(player_count, player_object_arr, 1);
  }
  process_stats(player_object_arr, player_count);
  
}

void process_stats(GameObject_t **player_object_arr, int player_count) {
  int highest_player_score = get_highest_score();
  for (int i = 0; i < player_count; ++i) {
    highest_player_score = max(highest_player_score, player_object_arr[i]->score);
  }
  if(player_count == 1) {
    save_stats_to_file(highest_player_score, player_count, NULL, player_object_arr[0]->score);
  } else {
    unsigned int score[3];
    score[0] = 0;
    score[1] = 0;
    score[2] = 0;
    for (int i = 0; i < player_count; ++i) {
      score[player_object_arr[i]->knob_id] = player_object_arr[i]->score;
    }
    save_stats_to_file(highest_player_score, player_count, score, 0);

  }

}

void get_start_click() {
  int rebounce = 1;
  int clicked = 0;
  sleep(1);
  while (clicked == 0) {
    clicked += get_knob_click(RED_KNOB, &rebounce);
    clicked += get_knob_click(GREEN_KNOB, &rebounce);
    clicked += get_knob_click(BLUE_KNOB, &rebounce);
  }
}

int check_player_lost(GameObject_t player_obj) {
  return check_hitbox_hit(player_obj);
}

int check_hitbox_hit(GameObject_t player) {
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