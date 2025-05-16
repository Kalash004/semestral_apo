#pragma once
#ifndef UTILS_H
#define UTILS_H

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
#include "graphics.h"


extern unsigned char *membase;
extern uint16_t origin_fb[480][320];
extern void *origin_lcd;
extern unsigned int highest_players_score;

void get_stats_from_file();
int get_highest_score();
void save_stats_to_file(unsigned int highest_players_score, int flag, unsigned int last_game_score[3], unsigned int last_single_game_score);
#endif
