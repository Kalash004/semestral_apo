#include "graphics.h"


void write_img_to_buffer(Img* img, int x_pos, int y_pos) {
    if (!img || img->h + y_pos < 0 || img->w + x_pos < 0 || x_pos >= 480 || y_pos >= 320) return;

    for (int i = 0; i < img->w; ++i) {
        for (int j = 0; j < img->h; ++j) {
            if (x_pos + i < 0 || y_pos + j < 0 || x_pos + i >= 480 || y_pos + j >= 320) {
                continue;
            }

            uint32_t color = convert_rgb_to_hexa(img->data[j * img->w + i]);
            if (color == 17289) {
                continue; // transparent color
            }
            origin_fb[x_pos + i][y_pos + j] = color;
        }
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
  add_text_to_buffer("Player score Red: %u | Blue %u", player1_score, player2_score);
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