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
extern GameObject_t *pipe_pool;
extern GameObject_t *bird_obj;
extern GameObject_t *bird_obj2;
extern unsigned int highest_player_score;

void draw_menu_bars(int highlighted, int x, int y, int padding);
void draw_buffer();
void write_img_to_buffer(Img* img, int x_pos, int y_pos);
void draw_font(unsigned int x_pos,unsigned int y_pos, int size, char *str, int highlighted);
int draw_game(uint16_t origin_fb[480][320], void *lcd, int highlighted, int x, int y, int padding); //test method
void draw_stats();
void redraw_game_singleplayer(unsigned int player1_score);
void redraw_game_multiplayer(unsigned int player1_score, unsigned int player2_score);
void add_text_to_buffer(char *pattern, ...);

#endif
