
#include "main.h"

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

void program() {
    origin_lcd = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
    membase = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);

    background = ppm_load_image("/tmp/kolomcon/background.ppm");
    top_pipe = ppm_load_image("/tmp/kolomcon/top.ppm");
    btm_pipe = ppm_load_image("/tmp/kolomcon/bottom.ppm");
    bird1 = ppm_load_image("/tmp/kolomcon/bird1.ppm");
    bird_blue = ppm_load_image("/tmp/kolomcon/bird_blue.ppm");
    bird_red = ppm_load_image("/tmp/kolomcon/bird_red.ppm");

    pipe_pool = calloc(sizeof(GameObject_t), 6);
    for (int i = 0; i < 3; ++i) {
        GameObject_t top = { .x = i * (80 + 160), .y = -200, .img = top_pipe, .debounce = 0, .score = 0, .health = 1, .knob_id = 0};
        GameObject_t btm = { .x = i * (80 + 160), .y = -200 + GAP, .img = btm_pipe, .debounce = 0, .score = 0, .health = 1, .knob_id = 0 };
        pipe_pool[i] = top;
        pipe_pool[i + 3] = btm;
    }

    GameObject_t bird_obj1;
    bird_obj1.img = bird_red;
    bird_obj1.debounce = 1;
    bird_obj1.score = 0;
    bird_obj1.health = 1;
    bird_obj1.knob_id = RED_KNOB;
    

    GameObject_t bird_obj2;
    bird_obj2.img = bird_blue;
    bird_obj2.debounce = 1;
    bird_obj2.score = 0;
    bird_obj2.health = 1;
    bird_obj2.knob_id = BLUE_KNOB;

    GameObject_t bird_obj3;
    bird_obj3.img = bird1;
    bird_obj3.debounce = 1;
    bird_obj3.score = 0;
    bird_obj3.health = 1;
    bird_obj3.knob_id = GREEN_KNOB;


    GameObject_t **player_arr = calloc(sizeof(GameObject_t *), 3);
    player_arr[0] = &bird_obj1;
    player_arr[1] = &bird_obj2;
    player_arr[2] = &bird_obj3;



    while (1) {
        options_t options;
        main_menu(&options, origin_lcd);
        switch (options.game_mode) {
            case 1:
                play(3, 2, player_arr);
                break;
            case 2:
                play(2, 0, player_arr);
                break;
            case 3:
                printf("Game mode %d\n", options.game_mode);
                draw_stats();
                break;
        }
    }
}

void main_menu(options_t *opts, void *lcd) {
  // Frame buffer
  uint16_t origin_fb[480][320] = {0x0};
  //memset(origin_fb, 0x0, sizeof(origin_fb));
  write_img_to_buffer(background, 0, 0);
  draw_buffer();
  volatile uint32_t knobs_value = 0;

 // Big fonts
  int highlited_index = -1;
  draw_font(100, 10, 3, "FLAPPY BIRD",1);
  draw_menu_bars(highlited_index, 100, 100, 40);
  draw_buffer();
  // struct timespec loop_delay = {.tv_sec = 0, .tv_nsec = 200 * 1000 * 1000};
  int click_value = 0;
  while (1) {
    int debounce = 1;
    draw_buffer();
    int rot = get_knob_rotation();
    click_value = get_knob_click(RED_KNOB, &debounce);
    if (rot == 0) {
      if(click_value == 1 && highlited_index != -1) {
        break;
      }
      draw_buffer();
      continue;
    }
    if (rot == -1) {
      highlited_index = (highlited_index == -1 ? 0 : highlited_index - 1);
      highlited_index = (highlited_index + 4) % 4;
      draw_menu_bars(highlited_index, 100, 100, 40);
      draw_buffer();
    } 
    if (rot == 1) {
      highlited_index += 1;
      highlited_index = highlited_index % 4;
      draw_menu_bars(highlited_index, 100, 100, 40);
      draw_buffer();
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
      exit_game();
      break;
  }

}

void exit_game() {
  memset(origin_fb, 0x0, sizeof(origin_fb));
  draw_buffer();
  exit(0);
}