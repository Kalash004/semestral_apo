#ifndef HEADER_H
#define HEADER_H

#define PATH "/tmp/kolomcon/"

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
  int game_mode;
  char p;  
} options_t;

typedef struct {
  int x;
  int y;
  int acceleration_x;
  Img *img;
  int debounce;
  unsigned int score;
  int health;
  int knob_id;
} GameObject_t;
#endif