#include "stats.h"

/*
This module handles reading and writing player statistics to a file. 
It supports retrieving the highest score, last single player score, and last multiplayer scores, and saving updated statistics after each game session.
*/

// get data from file (3 lines, 5 numbers, used to load when stats are opened)
void get_stats_from_file() {
    char path[100];
    strcpy(path, IMGPATH);
    FILE *file = fopen(strcat(path,"stats.txt"), "r");
    if (file == NULL) {
       perror("Error opening file");
       return;
    }

   unsigned int players_score;
   if (fscanf(file, "%u\n", &players_score) == 1) {
     add_text_to_buffer("Highest player score: %u", 0, 0, players_score);
   } else {
       perror("Failed to read data from file 1.\n");
   }

   if (fscanf(file, "%u\n", &players_score) == 1) {
     add_text_to_buffer("Last single player score: %u", 0, 50, players_score);
   } else {
       perror("Failed to read data from file. 2\n");
   }

  unsigned int player1_score, player2_score, player3_score;
   if (fscanf(file, "%u %u %u\n", &player1_score, &player2_score, &player3_score) == 3) {
    add_text_to_buffer("Last single player score: Red %u | Green %u | Blue %u", 0, 100, player1_score, player2_score, player3_score);
   } else {
    perror("Failed to read data from file. 3\n");
   }

   fclose(file);
}

// get only highest score data from file (used to load when each game ends)
int get_highest_score() {
    char path[100];
    strcpy(path, IMGPATH);
  FILE *file = fopen(strcat(path,"stats.txt"), "r");
  int players_score;
  if (fscanf(file, "%u", &players_score) == 1) {
    add_text_to_buffer("Highest player score: %u", 0, 0, players_score);
  } else {
    perror("Failed to read data from file 1.\n");
  }
  fclose(file);
  return players_score;
}   

// after comparing with a new game, save stats to file
void save_stats_to_file(unsigned int highest_players_score, int flag, unsigned int last_game_score[3], unsigned int last_single_game_score) {
  char path[100];
  strcpy(path, IMGPATH);
  FILE *file = fopen(strcat(path,"stats.txt"), "r+");
   if (file == NULL) {
       perror("Error opening file");
       return;
   }
   unsigned int line = 0;
   fprintf(file, "%u\n", highest_players_score);
   switch(flag) {
     case 0: // = update highest score/line 0 only
        fflush(file);
        break;
     case 1: // = update highest score & singleplayer/line 0 and 1
        fprintf(file, "%u\n", last_single_game_score);
        fflush(file);
        break;
     default: // = update highest score & multiplayer/line 0 and 2
        fscanf(file,"%u\n", &line);
        fprintf(file, "%u %u %u\n", last_game_score[2], last_game_score[1], last_game_score[0]);
        fflush(file);
        break;

   }
   fclose(file);
}