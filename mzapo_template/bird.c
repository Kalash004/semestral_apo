/*******************************************************************
  Project main function template for MicroZed based MZ_APO board
  designed by Petr Porazil at PiKRON

  change_me.c      - main file

  include your name there and license for distribution.

  Remove next text: This line should not appear in submitted
  work and project name should be change to match real application.
  If this text is there I want 10 points subtracted from final
  evaluation.

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

// membase
unsigned char *membase;

//header
typedef struct {
  int player_count;
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
// -header

// drawing
uint16_t origin_fb[480][320] = {0xffff}; 
void *origin_lcd;

// images
Img *background;
Img *bird1;
Img *top_pipe;
Img *btm_pipe;

// Game objects
GameObject_t *pipe_pool;
GameObject_t *bird_obj;

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

  background = ppm_load_image("/tmp/kalasan1/background.ppm");
  bird1 = ppm_load_image("/tmp/kalasan1/bird1.ppm");
  top_pipe = ppm_load_image("/tmp/kalasan1/top.ppm");
  btm_pipe = ppm_load_image("/tmp/kalasan1/bottom.ppm");

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
  bird_obj->x = 75;
  bird_obj->y = 145;

  options_t options;
  main_menu(&options, origin_lcd);
  switch (options.player_count) {
    case 1: 
      play_singleplayer();
      break;
    case 2:
      // play_multiplayer();
      break;
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
    draw_buffer();
    int rot = get_knob_rotation();
    click_value = get_knob_click();
    if (rot == 0) {
      if(click_value == 1) {
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
      opts->player_count = 1;  
      break;
    case 1:
      opts->player_count = 2;
      break;
    case 2:
      // TODO write stats
      break;
    case 3:
      exit_game();
      break;
  }

}

void redraw_game() {
  write_img_to_buffer(background, -10, 0);
  write_img_to_buffer(bird_obj->img, bird_obj->x, bird_obj->y);
  for (int i = 0; i < 6; ++i) {
    write_img_to_buffer(pipe_pool[i].img, pipe_pool[i].x, pipe_pool[i].y);
  }
  draw_buffer();
}

void exit_game() {
  memset(origin_fb, 0x0, sizeof(origin_fb));
  draw_buffer();
  exit(0);
}

void physics() {
  bird_obj->acceleration_x -= GRAVITY_FORCE;
  if(bird_obj->acceleration_x > 0) {
    bird_obj->acceleration_x -= JUMP_PER_FRAME; 
  }
  if(bird_obj->acceleration_x < -25) {
    bird_obj->acceleration_x = -25;
  } else if(bird_obj->acceleration_x > 55) {
    bird_obj->acceleration_x = 55;
  }
  bird_obj->y -= bird_obj->acceleration_x;
}

void update_pipes() {
  for(int i = 0; i < 3; ++i) {
    pipe_pool[i].x -= 3;
    pipe_pool[i + 3].x -= 3;
    if(pipe_pool[i].x < -100) {
      int rand_y = rand() % 190;
      pipe_pool[i].y = -320 + rand_y;
      pipe_pool[i + 3].y = -320 + rand_y + GAP;
      pipe_pool[i].x = 620;
      pipe_pool[i + 3].x = 620;
    }
  }
}

int play_singleplayer() {
  redraw_game();
  int clicked = 0;
  while (clicked == 0) {
    clicked = get_knob_click();
  }

  while (1) {
    physics(); // TODO: think about values 
    clicked = get_knob_click();
    if (clicked == 1) {
      bird_obj->acceleration_x += 50;
    }
    update_pipes();
    redraw_game();
  }
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

int get_knob_click() {
  static int debounce = 0;
  uint8_t current_value = (*(volatile uint32_t*)(membase + SPILED_REG_KNOBS_8BIT_o) >> 26) & 0xff;
  if(current_value == 1 && debounce == 1) {
    debounce = 0;
    return 1;
  } else if (current_value == 0) {
    debounce = 1;
    return 0;
  } else {
    return 0;
  }
  
  return 0;
}

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

void write_img_to_buffer(Img* img, int x_pos, int y_pos) {
  if(img->h + y_pos < 0 || img->w + x_pos < 0 || x_pos >= SCREEN_WIDTH || y_pos >= SCREEN_HEIGHT) {
    return; 
  }
  //printf("%d %d skib", img->w, img->h);
  for(int i = 0; i < img->w; ++i) {
    for(int j = 0; j < img->h; ++j) {
      if (x_pos + i < 0 || y_pos + j < 0 || x_pos + i >= SCREEN_WIDTH || y_pos + j >= SCREEN_HEIGHT) {
        continue;
      }
      origin_fb[x_pos + i][y_pos + j] = convert_rgb_to_hexa(img->data[j * img->w + i]);
    }
  }

}