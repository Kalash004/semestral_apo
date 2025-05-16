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

int play_multiplayer() {
  unsigned int player1_score = 0;
  unsigned int player2_score = 0;
  int health = 1;
  int health2 = 1;
  int red_debounce = 1;
  int blue_debounce = 1;
  restart_game_objects_multi();
  redraw_game_multiplayer(player1_score, player2_score);
  int clicked = 0;
  sleep(1);
  while (clicked == 0) {
    clicked = get_knob_click(RED_KNOB, &red_debounce);
  }
  struct timespec old, new; 
  led_draw(0, 0x00FF00);
  led_draw(1, 0x00FF00);
  while (1) {
    
    clock_gettime(CLOCK_MONOTONIC, &new);
    long long start_ns = old.tv_sec * 1e9 + old.tv_nsec;
    long long end_ns = new.tv_sec * 1e9 + new.tv_nsec;
    long long delta_time_ns = end_ns - start_ns;
    printf("Time: %lli - %lli = %lli\n", end_ns, start_ns, delta_time_ns);
    //int clicked_red = get_knob_click(RED_KNOB, &red_debounce);
    //int clicked_blue = get_knob_click(BLUE_KNOB, &blue_debounce);
    int clicked_red = get_knob_click(RED_KNOB, &red_debounce);
    if (clicked_red == 1) {
      bird_obj->acceleration_x += 30;
    }
    int clicked_blue = get_knob_click(BLUE_KNOB, &blue_debounce);
    if (clicked_blue == 1) {
      bird_obj2->acceleration_x += 30;
    }  

    if (health > 0) {
      physics(bird_obj, delta_time_ns);
    }
    if (health2 > 0) {
      physics(bird_obj2, delta_time_ns);
    } 
    
    clock_gettime(CLOCK_MONOTONIC, &old);;
    update_pipes();
    int check = check_player_lost(bird_obj);
    if (check == -1 && health > 0) {
      health--;
    } 
    else if(check == 1 && health > 0) {
      player1_score++;
    }
    check = check_player_lost(bird_obj2);
    if (check == -1 && health2 > 0) { 
      health2--;
    } 
    else if(check == 1 && health2 > 0) {
      player2_score++;
    }
    if (health + health2 <= 0) break;
    redraw_game_multiplayer(player1_score, player2_score);
    
  }
  led_draw(0, 0x000000);
  led_draw(1, 0x000000);
  return (player1_score > player2_score ? player1_score : player2_score);
}


int play_singleplayer() {
  int debounce = 1;
  unsigned int player1_score = 0;
  int health = 1;
  restart_game_objects();
  redraw_game_singleplayer(player1_score);
  int clicked = 0;
  sleep(1);
  while (clicked == 0) {
    clicked = get_knob_click(RED_KNOB, &debounce);
  }
  
  led_draw(0, 0x00FF00);
  struct timespec old, new; 
  while (1) {
    clock_gettime(CLOCK_MONOTONIC, &new);
    long long start_ns = old.tv_sec * 1e9 + old.tv_nsec;
    long long end_ns = new.tv_sec * 1e9 + new.tv_nsec;
    long long delta_time_ns = end_ns - start_ns;
    physics(bird_obj, delta_time_ns); // TODO: think about values 
    clicked = get_knob_click(RED_KNOB, &debounce);
    if (clicked == 1) {
      bird_obj->acceleration_x += 30;
    }
    clock_gettime(CLOCK_MONOTONIC, &old);;
    update_pipes();
    if (check_player_lost(bird_obj) == -1) health--; // Maybe ?
    else if(check_player_lost(bird_obj) == 1 && health > 0) player1_score++;
    if (health < 0) break;
    redraw_game_singleplayer(player1_score);
  }
  led_draw(0, 0x000000);
  return player1_score;
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