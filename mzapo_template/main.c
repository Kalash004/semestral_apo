
#include "main.h"
/*
This module manages the initialization of game assets, input device setup, image loading, pipe configuration, and the game’s main loop. 
It includes logic for both single-player and multiplayer modes and handles main menu navigation via rotary knobs and clicks.
*/

//entry point
int main(int argc, char *argv[])
{
    serialize();

    program();

    serialize_unlock();
    return 0;
}

void serialize() {
    if (serialize_lock(1) <= 0) {
        printf("System is occupied\n");
        printf("Waiting\n");
        serialize_lock(0);
    }
}

// initializes framebuffer, LED, and game assets (images, pipes, and player data), handles game mode selection in main menu
void program() {
    char path[100] = PATH;
    origin_lcd = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
    membase = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
    knob_init();
    led_draw(0, 0x000000);
    led_draw(1, 0x000000);
    background = ppm_load_image(strcat(path,"background.ppm"));
    strcpy(path, PATH);
    top_pipe = ppm_load_image(strcat(path,"top.ppm"));
    strcpy(path, PATH);
    btm_pipe = ppm_load_image(strcat(path,"bottom.ppm"));
    strcpy(path, PATH);
    bird1 = ppm_load_image(strcat(path,"bird1.ppm"));
    strcpy(path, PATH);
    bird_blue = ppm_load_image(strcat(path,"bird_blue.ppm"));
    strcpy(path, PATH);
    bird_red = ppm_load_image(strcat(path, "bird_red.ppm"));

    pipe_pool = calloc(sizeof(GameObject_t), 6);
    for (int i = 0; i < 3; ++i) {
        GameObject_t top = { .x = i * (80 + 160), .y = -200, .img = top_pipe, .debounce = 0, .score = 0, .health = 1, .knob_id = 0};
        GameObject_t btm = { .x = i * (80 + 160), .y = -200 + GAP, .img = btm_pipe, .debounce = 0, .score = 0, .health = 1, .knob_id = 0 };
        pipe_pool[i] = top;
        pipe_pool[i + 3] = btm;
    }

    GameObject_t bird_obj1;
    
    GameObject_t bird_obj2;

    GameObject_t bird_obj3;


    GameObject_t **player_arr = calloc(sizeof(GameObject_t *), 3);
    player_arr[0] = &bird_obj1;
    player_arr[1] = &bird_obj2;
    player_arr[2] = &bird_obj3;



    while (1) {
        options_t options;
        main_menu(&options, origin_lcd);
        switch (options.game_mode) {
            case 1:
                choose_singleplayer_knob(player_arr);
                play(1, player_arr);
                break;
            case 2: {
                int player_count = choose_player_knobs(player_arr);
                play(player_count, player_arr);
                break;
            } case 3:
                draw_stats();
                break;
            case 4:
                exit_game(background, bird1, bird_red, bird_blue, top_pipe, btm_pipe, player_arr, pipe_pool);
                break;
        }
    }
}

// utility to set info about a newly chosen player
void add_to_player_arr(GameObject_t **player_arr, Img *bird_img, int knob_id, int x, int y, int player_count) {
  player_arr[player_count-1]->img = bird_img;
  player_arr[player_count-1]->knob_id = knob_id;
  player_arr[player_count-1]->y = y;
  player_arr[player_count-1]->x = x;
  player_arr[player_count-1]->score = 0;
}

// for Play Singleplayer: wait until the user chooses their knob and add it to the player array
void choose_singleplayer_knob(GameObject_t **player_arr) {
  write_img_to_buffer(background, 0, 0);
  draw_buffer();
  int debounce = 0;
  sleep(1);
  led_draw(0, 0xFFFF00); // yellow led
  led_draw(1, 0xFFFF00);
  while (1) {
    if (get_knob_click(RED_KNOB, &debounce) == 1) {
      add_to_player_arr(player_arr, bird_red, RED_KNOB, 90, 145, 1);
      break;
    }

    if (get_knob_click(GREEN_KNOB, &debounce) == 1) {
      add_to_player_arr(player_arr, bird1, GREEN_KNOB, 230, 145, 1);
      break;
    }

    if (get_knob_click(BLUE_KNOB, &debounce) == 1) {
      add_to_player_arr(player_arr, bird_blue, BLUE_KNOB, 370, 145, 1);
      break;
    }
  }
}

// for Play Multiplayer: wait until 2-3 users choose their knobs and add them to the player array
int choose_player_knobs(GameObject_t **player_arr) {
  write_img_to_buffer(background, 0, 0);
  draw_buffer();
  restart_pipes();
  int player_count = 0;
  int red_clicked = 0;
  int green_clicked = 0;
  int blue_clicked = 0;
  int red_debounce = 1;
  int green_debounce = 1;
  int blue_debounce = 1;
  sleep(1);
  led_draw(0, 0xFFFF00); // yellow led 
  led_draw(1, 0xFFFF00);
  while (1) {
    if ((red_clicked > 1 || blue_clicked > 1 || green_clicked > 1) && player_count >= 2) break;
    if (get_knob_click(RED_KNOB, &red_debounce) == 1) {
      red_clicked++;
      if (red_clicked == 1){
        ++player_count;
        add_to_player_arr(player_arr, bird_red, RED_KNOB, 90, 145, player_count);
      }
    }

    if (get_knob_click(GREEN_KNOB, &green_debounce) == 1) {
      green_clicked++;
      if (green_clicked == 1) {
        player_count++;
        add_to_player_arr(player_arr, bird1, GREEN_KNOB, 230, 145, player_count);
      }
    }

    if (get_knob_click(BLUE_KNOB, &blue_debounce) == 1) {
      blue_clicked++;
      if (blue_clicked == 1) {
        player_count++;
        add_to_player_arr(player_arr, bird_blue, BLUE_KNOB, 370, 145, player_count);
      }
    }
    redraw_game_multiplayer(player_count, player_arr, 0);
  }
  return player_count;
}

// draws main menu and sets index based on current knob rotation changes
void main_menu(options_t *opts, void *lcd) {
  // Frame buffer
  // memset(origin_fb, 0x0, sizeof(origin_fb));
  write_img_to_buffer(background, 0, 0);
 // Big sparse fonts
  int highlited_index = -1;
  draw_font(100, 10, 3, "FLAPPY BIRD", 2, CHANGING_WIDTH_FONT);
  int click_value = 0;
  while (1) {
    int debounce = 1;
    draw_menu_bars(highlited_index, 100, 100, 40);
    draw_buffer();
    int rot = get_knob_rotation();
    click_value = get_knob_click(RED_KNOB, &debounce);
    if (rot == 0) {
      if(click_value == 1 && highlited_index != -1) {
        break;
      }
      continue;
    }
    if (rot == -1) {
      highlited_index = (highlited_index == -1 ? 0 : highlited_index - 1);
      highlited_index = (highlited_index + 4) % 4;
    } 
    if (rot == 1) {
      highlited_index += 1;
      highlited_index = highlited_index % 4;
    } 
    
    rot = 0;
  }
  switch (highlited_index) {
    case 0:
      opts->game_mode = 1;  
      break;
    case 1:
      opts->game_mode = 2;
      break;
    case 2:
      opts->game_mode = 3;
      break;
    case 3:
      opts->game_mode = 4;
      break;
  }

}

void free_img(Img *img) {
  if(img == NULL) return;
  free(img->data);
  free(img);
  img = NULL;
}

void exit_game(void *background, void *bird1, void *bird_red, void *bird_blue, void *top_pipe, void *btm_pipe, void *object_arr, void *pipe_arr) {
  free_img(background);
  free_img(bird1);
  free_img(bird_red);
  free_img(bird_blue);
  free_img(top_pipe);
  free_img(btm_pipe);
  free(object_arr);
  free(pipe_arr);
  memset(origin_fb, 0x0, sizeof(origin_fb));
  draw_buffer();
  exit(0);
}