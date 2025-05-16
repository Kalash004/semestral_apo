#pragma once
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdarg.h> 
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

#include "img.h"
#include "game.h"

#define RED_KNOB 2
#define BLUE_KNOB 0

// Screen size
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 320

/* Parallel LCD registers */

#define PARLCD_REG_BASE_PHYS  0x43c00000
#define PARLCD_REG_SIZE       0x00004000

#define PARLCD_REG_CR_o                 0x0000
#define PARLCD_REG_CR_RESET_m                  0x00000002
#define PARLCD_REG_CMD_o                0x0008
#define PARLCD_REG_DATA_o               0x000C

// font styles
#define SAME_WIDTH_FONT 0
#define CHANGING_WIDTH_FONT 1

typedef struct Pixel;
typedef struct Img;
typedef struct GameObject_t;

extern unsigned char *membase;
extern uint16_t origin_fb[480][320];
extern void *origin_lcd;
extern Img *background;
extern Img *bird1;
extern Img *bird_red;
extern Img *bird_blue;
extern Img *top_pipe;
extern Img *btm_pipe;
extern unsigned int highest_player_score;

void draw_menu_bars(int highlighted, int x, int y, int padding);
void draw_buffer();
void write_img_to_buffer(Img* img, int x_pos, int y_pos);
void draw_font(unsigned int x_pos,unsigned int y_pos, int size, char *str, int highlighted, int font_style);
int draw_changing_width_char(int x_pos, int y_pos, char ch, int highlighted, int scale, font_descriptor_t *fdes);
int draw_sparse_char(int x_pos, int y_pos, char ch, int highlighted, int scale, font_descriptor_t *font);
void draw_pixel_big(int x, int y, unsigned short color, int scale);
void draw_pixel(int x, int y, unsigned short color);
void draw_char(unsigned int x_pos,unsigned int y_pos, int size, char ch, int ch_shift, int highlighted, font_descriptor_t *font);
void draw_stats();
void add_text_to_buffer(char *pattern, ...);
void redraw_game_multiplayer(int player_count, GameObject_t **player_arr);

#endif
