#include "image.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "mzapo_parlcd.h"

extern void *origin_lcd;
extern uint16_t origin_fb[480][320];

void draw_img(GameObject_t *obj) {
  if (!obj || !obj->img) return;

  int start_x = obj->x;
  int start_y = obj->y;
  int w = obj->img->w;
  int h = obj->img->h;

  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x) {
      int px = start_x + x;
      int py = start_y + y;

      if (px < 0 || py < 0 || px >= 320 || py >= 480) continue;

      uint16_t pixel = obj->img->data[y * w + x];
      if (pixel != 0xFFFF) { // Assuming 0xFFFF is used for transparency
        origin_fb[py][px] = pixel;
      }
    }
  }
}

Img *load_image_from_rgb565_data(const uint16_t *data, int width, int height) {
  Img *img = (Img *)malloc(sizeof(Img));
  if (!img) return NULL;

  img->w = width;
  img->h = height;
  img->data = (uint16_t *)malloc(width * height * sizeof(uint16_t));

  if (!img->data) {
    free(img);
    return NULL;
  }

  memcpy(img->data, data, width * height * sizeof(uint16_t));
  return img;
}

void free_image(Img *img) {
  if (!img) return;
  if (img->data) free(img->data);
  free(img);
}
