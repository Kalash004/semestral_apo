/*******************************************************************
  Project main function template for MicroZed based MZ_APO board
  designed by Petr Porazil at PiKRON

  
  
 *******************************************************************/

#define _POSIX_C_SOURCE 200112L

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "serialize_lock.h"
#include "font_types.h"

#include <stdarg.h>

#define SPILED_REG_BASE_PHYS  0x43c40000
#define SPILED_REG_SIZE       0x00004000

#define SPILED_REG_LED_LINE_o           0x004
#define SPILED_REG_LED_RGB1_o           0x010
#define SPILED_REG_LED_RGB2_o           0x014
#define SPILED_REG_LED_KBDWR_DIRECT_o   0x018

#define SPILED_REG_KBDRD_KNOBS_DIRECT_o 0x020
#define SPILED_REG_KNOBS_8BIT_o         0x024

/* Parallel LCD registers */

#define PARLCD_REG_BASE_PHYS  0x43c00000
#define PARLCD_REG_SIZE       0x00004000

#define PARLCD_REG_CR_o                 0x0000
#define PARLCD_REG_CR_RESET_m                  0x00000002
#define PARLCD_REG_CMD_o                0x0008
#define PARLCD_REG_DATA_o               0x000C

// Screen size
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 320

// Gap between pipes
#define GAP 450

// Bird jump/gravity settings
#define GRAVITY_FORCE 4
#define JUMP_PER_FRAME 5

//Utils
 #define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define PATH "/tmp/kolomcon/"

#define RED_KNOB 2
#define BLUE_KNOB 0

// membase
unsigned char *membase;

unsigned int highest_player_score = 0;

//header
typedef struct {
  int game_mode;
  char p;  
} options_t;

typedef struct {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} Pixel;

typedef struct {
    int w, h;
    Pixel *data;
} Img;

typedef struct {
  int x;
  int y;
  int acceleration_x;
  Img *img;
} GameObject_t;

void serialize();
void program();
// void spilled_line_anim();
void main_menu(options_t *opts, void *lcd);
void draw_font(unsigned int x_pos,unsigned int y_pos, int size, char *str, int highlighted);
void draw_buffer();
void draw_menu_bars(int highlighted, int x, int y, int padding);
void debug_print(char *pattern , ...);
int draw_game(uint16_t origin_fb[480][320], void *lcd, int highlighted, int x, int y, int padding); //test method
int get_knob_rotation();
Img* ppm_load_image(char *path);
void write_img_to_buffer(Img* background_img, int x_pos, int y_pos);
void exit_game();
void add_text_to_buffer(char *pattern, ...);
void restart_game_objects();
void redraw_stats();
void redraw_game_singleplayer(unsigned int player1_score);
void redraw_game_multiplayer(unsigned int player1_score, unsigned int player2_score);
void physics(GameObject_t *player_obj, long long delta_time);
int get_knob_multi_click(int knob_num, int *red_debounce, int *blue_debounce);
// -header

// drawing
uint16_t origin_fb[480][320] = {0xffff}; 
void *origin_lcd;

// images
Img *background;
Img *bird1;
Img *bird_red;
Img *bird_blue;
Img *top_pipe;
Img *btm_pipe;

// Game objects
GameObject_t *pipe_pool;
GameObject_t *bird_obj;
GameObject_t *bird_obj2;

int main(int argc, char *argv[])
{
  /* Serialize execution of applications */
  serialize();

  // program
  program();
  // program_example();

  /* Release the lock */
  serialize_unlock();

  return 0;
}

void serialize() {
   /* Serialize execution of applications */
  /* Try to acquire lock the first */
  if (serialize_lock(1) <= 0) {
    printf("System is occupied\n");
    if (1) {
      printf("Waitting\n");
      /* Wait till application hold_knobs_valueing lock releases it or exits */
      serialize_lock(0);
    }
  }
}

void program_example() {
  // spilled_line_anim();

  void *lcd = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
  parlcd_hx8357_init(lcd);
  uint16_t origin_fb[480][320] = {0xffff}; // frame buffer
  memset(origin_fb, 0xffff, sizeof(origin_fb));


  char *str = "Hello world sigma";
  font_descriptor_t *font = &font_rom8x16;
  for(size_t i = 0; i < strlen(str); ++i) {
    //origin_fb[y][y] = 0x0; // muzeme prenastavit cast obrazku (diagonalu)
    for(size_t x = 0; x < font->maxwidth; x++) {
      for(size_t y = 0; y < font->height; y++) {
        origin_fb[2 * (x + i * font->maxwidth)][y] = font->bits[str[i] * font->height + y] & (1 << (15 - x)) ? 0x0 : 0xffff;
      }
    } // sirka fontu
  }

  parlcd_write_cmd(lcd, 0x2c);
  for(size_t y = 0; y < 320; ++y) {
    for(size_t x = 0; x < 480; ++x) {
      parlcd_write_data(lcd, origin_fb[x][y]); //zapiseme prisl pocet pixelu
    }
  }

}

void spilled_line_anim() {
   void *spiled = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
  uint16_t *ledline = spiled + SPILED_REG_LED_LINE_o;
  for (size_t i = 0x00; i < 0xff; ++i) {
      *ledline = i;
        sleep(1);
  }
}

void program() {
  origin_lcd = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
  membase = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
  background = ppm_load_image("/tmp/kolomcon/background.ppm");
  bird1 = ppm_load_image("/tmp/kolomcon/bird1.ppm");
  top_pipe = ppm_load_image("/tmp/kolomcon/top.ppm");
  btm_pipe = ppm_load_image("/tmp/kolomcon/bottom.ppm");
  bird_blue = ppm_load_image("/tmp/kolomcon/bird_blue.ppm");
  bird_red = ppm_load_image("/tmp/kolomcon/bird_red.ppm");
  pipe_pool = calloc(sizeof(GameObject_t), 6);
  for (int i = 0; i < 3; ++i) {
    GameObject_t top;
    GameObject_t btm;
    top.x = 0 + (i * (80 + 160));
    btm.x = 0 + (i * (80 + 160));
    top.y = -200;
    btm.y = top.y + GAP;
    top.img = top_pipe;
    btm.img = btm_pipe;
    pipe_pool[i] = top;
    pipe_pool[i+3] = btm;
  }

  bird_obj = calloc(sizeof(GameObject_t), 1);
  bird_obj->img = bird1;
  bird_obj2 = calloc(sizeof(GameObject_t), 1);
  bird_obj2->img = bird1;

  while (1) {
    options_t options;
    main_menu(&options, origin_lcd);
    switch (options.game_mode) {
      case 1: 
        highest_player_score = max(highest_player_score, play_singleplayer());
        break;
      case 2:
        highest_player_score = max(highest_player_score, play_multiplayer());
        break;
      case 3:
        draw_stats();
        break;
    }
  }
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
    GameObject_t *btm = &pipe_pool[i+3];
    top->x = 480 + (i * (80 + 160));
    btm->x = 480 + (i * (80 + 160));
    top->y = -200;
    btm->y = top->y + GAP;
    top->img = top_pipe;
    btm->img = btm_pipe;
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

void draw_stats() {
  while(1) {
    int debounce = 1;
    write_img_to_buffer(background, 0, 0);
    int click_value = get_knob_click(RED_KNOB, &debounce);
    if(click_value == 1) {
      break;
    }
    add_text_to_buffer("Highest score is %u", highest_player_score);
    draw_buffer();
  }
  
}

void redraw_game_singleplayer(unsigned int player1_score) {
  write_img_to_buffer(background, 0, 0);
  write_img_to_buffer(bird_obj->img, bird_obj->x, bird_obj->y);
  for (int i = 0; i < 6; ++i) {
    write_img_to_buffer(pipe_pool[i].img, pipe_pool[i].x, pipe_pool[i].y);
  }
  add_text_to_buffer("Player score %u", player1_score);
  draw_buffer();
}


void redraw_game_multiplayer(unsigned int player1_score, unsigned int player2_score) {
  write_img_to_buffer(background, 0, 0);
  write_img_to_buffer(bird_obj->img, bird_obj->x, bird_obj->y);
  write_img_to_buffer(bird_obj2->img, bird_obj2->x, bird_obj2->y);
  for (int i = 0; i < 6; ++i) {
    write_img_to_buffer(pipe_pool[i].img, pipe_pool[i].x, pipe_pool[i].y);
  }
  //add_text_to_buffer("Player score Red: %u | Blue %u", player1_score, player2_score);
  draw_buffer();
}

void exit_game() {
  memset(origin_fb, 0x0, sizeof(origin_fb));
  draw_buffer();
  exit(0);
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

void draw_menu_bars(int highlighted, int x, int y, int padding) {
  draw_font(x, y, 2, "Single Player", 0);
  draw_font(x, y+padding, 2, "Multi Player", 0);
  draw_font(x, y+padding*2, 2, "Stats", 0);
  draw_font(x, y+padding*3, 2, "Exit", 0);
  switch (highlighted) {
    case 0: 
      draw_font(x, y, 2, "Single Player", 1);
      break;
    case 1: 
      draw_font(x, y+padding, 2, "Multi Player", 1);
      break;
    case 2: 
      draw_font(x, y+padding*2, 2, "Stats", 1);
      break;
    case 3:
      draw_font(x, y+padding*3, 2, "Exit", 1);
      break;
    default:
      break;
  }

}

int draw_game(uint16_t origin_fb[480][320], void *lcd, int highlighted, int x, int y, int padding) {
  draw_font(x, y, 3, "GAME", 0);
}

void draw_font(unsigned int x_pos,unsigned int y_pos, int size, char *str, int highlighted) {
  // Fill buffer
  font_descriptor_t *font = &font_rom8x16;
  for(size_t i = 0; i < strlen(str); ++i) {
    for(size_t x = 0; x < font->maxwidth; x++) {
      for(size_t y = 0; y < font->height; y++) {
        if (x_pos + x + i > SCREEN_WIDTH) {
          continue;
        }
        if (y_pos + y > SCREEN_HEIGHT) {
          continue;
        }
        if (highlighted == 0) {
          origin_fb[x_pos + size * (x + i * font->maxwidth)][y_pos + y * size] = font->bits[str[i] * font->height + y] & (1 << (15 - x)) ? 0xffff : 0x0;
        } else {
          origin_fb[x_pos + size * (x + i * font->maxwidth)][y_pos + y * size] = font->bits[str[i] * font->height + y] & (1 << (15 - x)) ? 0x0 : 0xffff;
        }
      }
    }
  }

}

void draw_buffer() {
  parlcd_write_cmd(origin_lcd, 0x2c);
  for(size_t y = 0; y < SCREEN_HEIGHT; ++y) {
    for(size_t x = 0; x < SCREEN_WIDTH; ++x) {
      parlcd_write_data(origin_lcd, origin_fb[x][y]);
    }
  }
}

void debug_print(char *pattern, ...) {
    va_list args;
    char str[5555];

    va_start(args, origin_fb);
    vsnprintf(str, sizeof(str), pattern, args);
    va_end(args);

    draw_font(0, 0, 1, str, 0);
    draw_buffer();
}

void add_text_to_buffer(char *pattern, ...) {
    va_list args;
    char str[5555];

    va_start(args, origin_fb);
    vsnprintf(str, sizeof(str), pattern, args);
    va_end(args);

    draw_font(0, 0, 1, str, 0);
}

int get_knob_click(int knob_num, int *debounce) {
  if (knob_num != 0 && knob_num != 2) return 0;
  uint8_t current_value;
  if(knob_num == RED_KNOB) {
    current_value = (*(volatile uint32_t*)(membase + SPILED_REG_KNOBS_8BIT_o) >> (24 + RED_KNOB)) & 0xff;
  } else if(knob_num == BLUE_KNOB) {
    current_value = (*(volatile uint32_t*)(membase + SPILED_REG_KNOBS_8BIT_o) >> (24 + BLUE_KNOB)) & 0x01;

  }
  if(current_value == 1 && *debounce == 1) {
    *debounce = 0;
    return 1;
  } else if (current_value == 0) {
    *debounce = 1;
    return 0;
  } else {
    return 0;
  }
  
  return 0;
}

int get_knob_multi_click(int knob_num, int *red_debounce, int *blue_debounce) {
  if (knob_num != 0 && knob_num != 2) return 0;
  uint8_t current_value_blue = (*(volatile uint32_t*)(membase + SPILED_REG_KNOBS_8BIT_o) >> (24 + RED_KNOB)) & 0xff;
  uint8_t current_value_red = (*(volatile uint32_t*)(membase + SPILED_REG_KNOBS_8BIT_o) >> (24 + BLUE_KNOB)) & 0x01;
  debug_print("%d %d debounce %d %d", current_value_red, current_value_blue, *red_debounce, *blue_debounce);
  if(current_value_red == 1 && *red_debounce == 1 && current_value_blue && *blue_debounce == 1) {
    *red_debounce = 0;
    *blue_debounce = 0;
    return 3;
  }
  if(current_value_blue == 1 && *blue_debounce == 1) {
    *blue_debounce = 0;
    *red_debounce = 1;
    return 2;
  }
  if(current_value_red == 1 && *red_debounce == 1) {
    *blue_debounce = 1;
    *red_debounce = 0;
    return 1;
  } 
  if (current_value_red == 0 && current_value_blue == 0) {
    *red_debounce = 1;
    *blue_debounce = 1;
    return 0;
  }
  if(current_value_red == 0) {
    *red_debounce = 1;
    return 0;
  } 
  if(current_value_blue == 0) {
    *blue_debounce = 1;
    return ;
  }  
  else {
    return 0;
  }
  
  return 0;
}

// 

int get_knob_rotation() {
    static int old_value = -1;
    uint8_t current_value = (*(volatile uint32_t*)(membase + SPILED_REG_KNOBS_8BIT_o) >> 16) & 0xff;

    if (old_value == -1) {
        old_value = current_value;
        return 0;
    }

    int diff = (current_value - old_value + 256) % 256;

    // Jitter
    if (diff < 3 || diff > 253) {
        return 0; 
    }


    old_value = current_value;

    return (diff < 128) ? 1 : -1;
}

Img* ppm_load_image(char *path) {
    char buff[16];
    Img *img;
    FILE *fp;
    int rgbscanval;
    fp = fopen(path, "rb");
    if (!fp) {
        fprintf(stderr, "Unable to open img file n");
        return 1;
    }

    if (!fgets(buff, sizeof(buff), fp)) {
        return 1;
    }

    if (buff[0] != 'P' || buff[1] != '6') {
        fprintf(stderr, "Invalid P6 format\n");
        return 1;
    }
    img = (Img *)malloc(sizeof(Img));
    if (!img) {
        fprintf(stderr, "Malloc fail\n");
        return 1;
    }

  char c;
  
  if (fscanf(fp, "%c", &c) == 1 && c == '#') {
    while(fscanf(fp, "%c", &c) == 1 && c != '\n') {
    }
  }
  fseek(fp, -1, SEEK_CUR);


    if (fscanf(fp, "%d %d", &img->w, &img->h) != 2) {
        fprintf(stderr, "Invalid image size\n");
        return 1;
    }
    if (fscanf(fp, "%d", &rgbscanval) != 1) {
        fprintf(stderr, "Invalid max rgb value");
        return 1;
    }

    printf("%d %d\n", img->w, img->h);

    while (fgetc(fp) != '\n');

    img->data = (Pixel*)malloc(img->w * img->h * sizeof(Pixel));

    if (!img) {
        fprintf(stderr, "Malloc fail\n");
        return 1;
    }
    if (fread(img->data, 3 * img->w, img->h, fp) != img->h) {
        //  fprintf(stderr, "Error loading img file '%s'\n", argv[1]);
        return 1;
    }
    

    fclose(fp);
    return img;
}

uint32_t convert_rgb_to_hexa(Pixel rgb) {
  uint16_t r = ((uint32_t)rgb.red  >> 3) & 0x1F; // 5 bits
  uint16_t g = ((uint32_t)rgb.green >> 2) & 0x3F; // 6 bits
  uint16_t b = ((uint32_t)rgb.blue >> 3) & 0x1F; // 5 bits

  return (r << 11) | (g << 5) | b;
}

// todo - skip given color
void write_img_to_buffer(Img* img, int x_pos, int y_pos) {
  if(img->h + y_pos < 0 || img->w + x_pos < 0 || x_pos >= SCREEN_WIDTH || y_pos >= SCREEN_HEIGHT) {
    return; 
  }
  for(int i = 0; i < img->w; ++i) {
    for(int j = 0; j < img->h; ++j) {
      if (x_pos + i < 0 || y_pos + j < 0 || x_pos + i >= SCREEN_WIDTH || y_pos + j >= SCREEN_HEIGHT) {
        continue;
      }
      uint32_t color = convert_rgb_to_hexa(img->data[j * img->w + i]);
      if (color == 17289) {
        // transparent color
        continue;
      }
      origin_fb[x_pos + i][y_pos + j] = convert_rgb_to_hexa(img->data[j * img->w + i]);
    }
  }

}