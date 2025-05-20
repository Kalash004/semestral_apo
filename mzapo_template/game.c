#include "game.h"

/*
This module implements the core gameplay loop, physics, collision detection, and pipe management for the game
supporting up to three players using knobs (RED, GREEN, BLUE). It manages player states, scoring, and game restart logic.
*/

// restart objects before each game
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

// redraw pipe array
void restart_pipes() {
 for (int i = 0; i < 3; ++i) {
    GameObject_t *pipe_top = &pipe_pool[i];
    GameObject_t *pipe_bottom = &pipe_pool[i+3];
    pipe_top->x = PIPE_RIGHTMOST_POSITION + (i * PIPE_GAP_XRANGE); // first is at X=420, second at X=640 etc.
    pipe_bottom->x = PIPE_RIGHTMOST_POSITION + (i * PIPE_GAP_XRANGE);
    int rand_y = rand() % PIPE_GAP_YRANGE;
    pipe_top->y = PIPE_TOPMOST_POSITION + rand_y;
    pipe_bottom->y = PIPE_TOPMOST_POSITION + rand_y + GAP;
    pipe_top->img = top_pipe;
    pipe_bottom->img = btm_pipe;
  }
}


// applies gravity and jumps, updates y based on acceleration value
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

// pipes movement in each display refresh
void update_pipes() {
  for(int i = 0; i < 3; ++i) {
    GameObject_t *pipe_top = &pipe_pool[i];
    GameObject_t *pipe_bottom = &pipe_pool[i+3];
    pipe_top->x -= PIPE_SHIFT;
    pipe_bottom->x -= PIPE_SHIFT;
    if(pipe_top->x < PIPE_LEFTMOST_POSITION) {
      int rand_y = rand() % PIPE_GAP_YRANGE;
      pipe_top->y = PIPE_TOPMOST_POSITION + rand_y;
      pipe_bottom->y = PIPE_TOPMOST_POSITION + rand_y + GAP;
      pipe_top->x = PIPE_RIGHTMOST_POSITION;
      pipe_bottom->x = PIPE_RIGHTMOST_POSITION;
    }
  }
}

// utility function to add score to display output 
void add_multiplayer_score(GameObject_t **player_object_arr, int player_count) {
  unsigned int score[3] = {0};
  memset(score, 0, 3 * sizeof(unsigned int));  
  for(int i = 0; i < player_count; ++i) {
    GameObject_t *player_obj = player_object_arr[i];
    score[player_obj->knob_id] = player_obj->score;
  }
  add_text_to_buffer("Bird score: Red %u | Green %u | Blue %u", 0, 0, score[RED_KNOB], score[GREEN_KNOB], score[BLUE_KNOB]);
}

// main game loop function
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

// function to refresh Stats file
void process_stats(GameObject_t **player_object_arr, int player_count) {
  int highest_player_score = get_highest_score();
  for (int i = 0; i < player_count; ++i) {
    highest_player_score = max(highest_player_score, player_object_arr[i]->score);
  }
  if(player_count == 1) {
    save_stats_to_file(highest_player_score, player_count, NULL, player_object_arr[0]->score);
  } else {
    unsigned int score[3];
    memset(score, 0, 3 * sizeof(unsigned int)); 
    for (int i = 0; i < player_count; ++i) {
      score[player_object_arr[i]->knob_id] = player_object_arr[i]->score;
    }
    save_stats_to_file(highest_player_score, player_count, score, 0);

  }

}

// wait for 1 sec and then get pressed knob
void get_start_click() {
  int rebounce = 1;
  int clicked = 0;
  sleep(1);
  led_draw(0,0x00FF00); // green led
  led_draw(1,0x00FF00);
  while (clicked == 0) {
    clicked += get_knob_click(RED_KNOB, &rebounce);
    clicked += get_knob_click(GREEN_KNOB, &rebounce);
    clicked += get_knob_click(BLUE_KNOB, &rebounce);
  }
}

int check_player_lost(GameObject_t player_obj) {
  return check_hitbox_hit(player_obj);
}

// check whether bird hit a pipe or whether bird passed a pipe (score/health changes)
int check_hitbox_hit(GameObject_t player) {
if (player.y > SCREEN_HEIGHT || player.y < 0) return -1;
  for(int i = 0; i < 3; ++i) {
    GameObject_t *pipe_top = &pipe_pool[i];
    GameObject_t *pipe_bottom = &pipe_pool[i+3]; 
    if(pipe_top->x > PIPE_SCORE_RANGE && pipe_top->x < (PIPE_SCORE_RANGE + 4)) return 1;
    if (!(player.x + player.img->w > pipe_top->x && player.x < pipe_top->x + pipe_top->img->w)) continue;  
    if (player.y < pipe_top->y + pipe_top->img->h) return -1;
    else if (player.y + player.img->h > pipe_bottom->y) return -1;
  }
  return 0;
}