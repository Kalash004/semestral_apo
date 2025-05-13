/*******************************************************************
  Project main function template for MicroZed based MZ_APO board
  designed by Petr Porazil at PiKRON

  
  
 *******************************************************************/

#define _POSIX_C_SOURCE 200112L
#include <stdarg.h> 


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



// void spilled_line_anim();
void debug_print(char *pattern , ...);
int get_knob_rotation();
Img* ppm_load_image(char *path);
void restart_game_objects();
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

// 