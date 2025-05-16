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

///////

void draw_pixel(int x, int y, unsigned short color) {
  if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
    origin_fb[x][y] = color;
  }
}
 
void draw_pixel_big(int x, int y, unsigned short color, int scale) {
  int i,j;
  for (i=0; i<scale; i++) {
    for (j=0; j<scale; j++) {
      draw_pixel(x+i, y+j, color);
    }
  }
}
 
int draw_changing_width_char(int x_pos, int y_pos, char ch, int highlighted, int scale, font_descriptor_t *fdes) {
  int w = char_width(ch, fdes);
  const font_bits_t *ptr;
  if ((ch >= fdes->firstchar) && (ch-fdes->firstchar < fdes->size)) {
    if (fdes->offset) {
      ptr = &fdes->bits[fdes->offset[ch-fdes->firstchar]];
    } else {
      int bw = (fdes->maxwidth+15)/16;
      ptr = &fdes->bits[(ch-fdes->firstchar)*bw*fdes->height];
    }
    int i, j;
    for (i=0; i<fdes->height; i++) {
      font_bits_t val = *ptr;
      for (j=0; j<w; j++) {
        switch(highlighted) {
          case 0:
            if((val&0x8000)!=0) draw_pixel_big(x_pos+scale*j, y_pos+scale*i, 0xd71c, scale);
            break;
          case 1:
            draw_pixel_big(x_pos+scale*j, y_pos+scale*i, (((val&0x8000)!=0) ? 0x7639 : 0xd71c), scale);
            break;
          case 2:
            draw_pixel_big(x_pos+scale*j, y_pos+scale*i, (((val&0x8000)!=0) ? 0xd71c : 0x7639), scale);
            break;
        }
        val<<=1;
      }
      ptr++;
    }
  }
  return w * scale;
}

///////

int draw_sparse_char(int x_pos, int y_pos, char ch, int highlighted, int scale, font_descriptor_t *font) {
  for(size_t x = 0; x < font->maxwidth; x++) {
      for(size_t y = 0; y < font->height; y++) {
        if (x_pos + x > SCREEN_WIDTH) {
          continue;
        }
        if (y_pos + y > SCREEN_HEIGHT) {
          continue;
        }
        if (highlighted == 0) {
          origin_fb[x_pos + scale * (x + font->maxwidth)][y_pos + y * scale] = font->bits[ch * font->height + y] & (1 << (15 - x)) ? 0xffff : 0x0;
        } else {
          origin_fb[x_pos + scale * (x + font->maxwidth)][y_pos + y * scale] = font->bits[ch * font->height + y] & (1 << (15 - x)) ? 0x0 : 0xffff;
        }
      }
  }
  return font->maxwidth * scale;
}

void draw_font(unsigned int x_pos,unsigned int y_pos, int size, char *str, int highlighted, int font_style) {
  font_descriptor_t *font;
  if(font_style == SAME_WIDTH_FONT) {font = &font_rom8x16;}
  else if(font_style == CHANGING_WIDTH_FONT) {font = &font_winFreeSystem14x16;}
  // Fill buffer
  for(size_t i = 0; i < strlen(str); ++i) {
    //draw_char(x_pos, y_pos, size, str[i], i, highlighted, font);
    int w;
    if(font_style == SAME_WIDTH_FONT) {
      w = draw_sparse_char(x_pos, y_pos, str[i], highlighted, size, font);

    } else if(font_style == CHANGING_WIDTH_FONT) {
      w = draw_changing_width_char(x_pos, y_pos, str[i], highlighted, size, font);

    }
    x_pos += w;
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
  int font_size = 2;
  draw_font(x, y, font_size, "Single Player", 0, SAME_WIDTH_FONT);
  draw_font(x, y+padding, font_size, "Multi Player", 0, SAME_WIDTH_FONT);
  draw_font(x, y+padding*2, font_size, "Stats", 0, SAME_WIDTH_FONT);
  draw_font(x, y+padding*3, font_size, "Exit", 0, SAME_WIDTH_FONT);
  switch (highlighted) {
    case 0: 
      draw_font(x, y, font_size, "Single Player", 1, SAME_WIDTH_FONT);
      break;
    case 1: 
      draw_font(x, y+padding, font_size, "Multi Player", 1, SAME_WIDTH_FONT);
      break;
    case 2: 
      draw_font(x, y+padding*2, font_size, "Stats", 1, SAME_WIDTH_FONT);
      break;
    case 3:
      draw_font(x, y+padding*3, font_size, "Exit", 1, SAME_WIDTH_FONT);
      break;
    default:
      break;
  }

}

void draw_stats() {
    int debounce = 1;
    write_img_to_buffer(background, 0, 0);
    int click_value = get_knob_click(RED_KNOB, &debounce);
    get_stats_from_file();
    draw_buffer();
    sleep(1);
    while(1) {
      int click_value = get_knob_click(RED_KNOB, &debounce);
      if(click_value == 1) {
        break;
      }
    }
}

void redraw_game_multiplayer(int player_count, GameObject_t **player_arr, int isPlaying) {
  write_img_to_buffer(background, 0, 0);
  for (int i = 0; i < player_count; ++i) {
    write_img_to_buffer(player_arr[i]->img, player_arr[i]->x, player_arr[i]->y);
  }
  for (int i = 0; i < 6; ++i) {
    write_img_to_buffer(pipe_pool[i].img, pipe_pool[i].x, pipe_pool[i].y);
  }
  if(player_count == 1 && isPlaying == 1) {add_text_to_buffer("Bird score: %u", 0, 0, player_arr[0]->score);}
  else if(isPlaying == 1) {add_multiplayer_score(player_arr, player_count);}
  draw_buffer();
}

void add_text_to_buffer(char *pattern, int x, int y, ...) {
    va_list args;
    char str[5555];

    va_start(args, origin_fb);
    vsnprintf(str, sizeof(str), pattern, args);
    va_end(args);

    draw_font(x, y, 1, str, 0, CHANGING_WIDTH_FONT);
}