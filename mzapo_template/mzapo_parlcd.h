/*******************************************************************
  Simple program to check LCD functionality on MicroZed
  based MZ_APO board designed by Petr Porazil at PiKRON

  mzapo_parlcd.h      - parallel connected LCD low level access

  (C) Copyright 2017 by Pavel Pisa
      e-mail:   pisa@cmp.felk.cvut.cz
      homepage: http://cmp.felk.cvut.cz/~pisa
      company:  http://www.pikron.com/
      license:  any combination of GPL, LGPL, MPL or BSD licenses

 *******************************************************************/

#ifndef MZAPO_PARLCD_H
#define MZAPO_PARLCD_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void parlcd_write_cr(unsigned char *parlcd_mem_base, uint16_t data);

void parlcd_write_cmd(unsigned char *parlcd_mem_base, uint16_t cmd);

void parlcd_write_data(unsigned char *parlcd_mem_base, uint16_t data);

void parlcd_write_data2x(unsigned char *parlcd_mem_base, uint32_t data);

void parlcd_delay(int msec);

void parlcd_hx8357_init(unsigned char *parlcd_mem_base);


#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif  /*MZAPO_PARLCD_H*/
