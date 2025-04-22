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

#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "serialize_lock.h"
#include "font_types.h"

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

//header
void serialize();
void program();
void spilled_line_anim();
// -header


int main(int argc, char *argv[])
{
  /* Serialize execution of applications */
  serialize();

  // program
  program();

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
      /* Wait till application holding lock releases it or exits */
      serialize_lock(0);
    }
  }
}

void program() {
  // spilled_line_anim();

  void *lcd = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
  parlcd_hx8357_init(lcd);
  uint16_t fb[480][320] = {0xffff}; // frame buffer
  memset(fb, 0xffff, sizeof(fb));


  char *str = "Hello world sigma";
  font_descriptor_t *font = &font_rom8x16;
  for(size_t i = 0; i < strlen(str); ++i) {
    //fb[y][y] = 0x0; // muzeme prenastavit cast obrazku (diagonalu)
    for(size_t x = 0; x < font->maxwidth; x++) {
      for(size_t y = 0; y < font->height; y++) {
        fb[2 * (x + i * font->maxwidth)][y] = font->bits[str[i] * font->height + y] & (1 << (15 - x)) ? 0x0 : 0xffff;
      }
    } // sirka fontu
  }

  parlcd_write_cmd(lcd, 0x2c);
  for(size_t y = 0; y < 320; ++y) {
    for(size_t x = 0; x < 480; ++x) {
      parlcd_write_data(lcd, fb[x][y]); //zapiseme prisl pocet pixelu
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