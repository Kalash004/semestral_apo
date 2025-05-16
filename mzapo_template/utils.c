#include "utils.h"

void debug_print(char *pattern, ...) {
    va_list args;
    char str[5555];

    va_start(args, origin_fb);
    vsnprintf(str, sizeof(str), pattern, args);
    va_end(args);

    draw_font(0, 0, 1, str, 0);
    draw_buffer();
}

int char_width(int ch, font_descriptor_t *font) {
  int width;
  if (!font->width) {
    width = font->maxwidth;
  } else {
    width = font->width[ch-font->firstchar];
  }
  return width;
}