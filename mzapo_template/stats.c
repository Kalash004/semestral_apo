#include "stats.h"

void get_stats_from_file() {
    char path[100];
    strcpy(path, PATH);
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

int get_highest_score() {
    char path[100];
    strcpy(path, PATH);
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

void save_stats_to_file(unsigned int highest_players_score, int flag, unsigned int last_game_score[3], unsigned int last_single_game_score) {
  char path[100];
  strcpy(path, PATH);
  FILE *file = fopen(strcat(path,"stats.txt"), "r+");
   if (file == NULL) {
       perror("Error opening file");
       return;
   }
   unsigned int line = 0;
   switch(flag) {
     case 0:
        fprintf(file, "%u\n", highest_players_score);
        fflush(file);
        break;
     case 1:
        fprintf(file, "%u\n", highest_players_score);
        printf("LAST %u\n", last_single_game_score);

        fprintf(file, "%u\n", last_single_game_score);
        fflush(file);
        break;
     default:
        fprintf(file, "%u\n", highest_players_score);
        fscanf(file,"%u\n", &line);
        printf("%u %u %u\n", last_game_score[0], last_game_score[1], last_game_score[2]);
        fprintf(file, "%u %u %u\n", last_game_score[2], last_game_score[1], last_game_score[0]);
        fflush(file);
        break;

   }
   fclose(file);
}