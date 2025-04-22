/*******************************************************************
  Simple program to check LCD functionality on MicroZed
  based MZ_APO board designed by Petr Porazil at PiKRON

  mzapo_phys.h      - mapping of the physical address to process

  (C) Copyright 2017 by Pavel Pisa
      e-mail:   pisa@cmp.felk.cvut.cz
      homepage: http://cmp.felk.cvut.cz/~pisa
      company:  http://www.pikron.com/
      license:  any combination of GPL, LGPL, MPL or BSD licenses

 *******************************************************************/

#ifndef MZAPO_PHYS_H
#define MZAPO_PHYS_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

void *map_phys_address(off_t region_base, size_t region_size, int opt_cached);

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif  /*MZAPO_PHYS_H*/
