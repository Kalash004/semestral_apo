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

void get_stats_from_file() {
  FILE *file = fopen("/tmp/kolomcon/stats.txt", "r");
   if (file == NULL) {
       perror("Error opening file");
       return 1;
   }

   int players_score;
   if (fscanf(file, "%u", &players_score) == 1) {
     add_text_to_buffer("Highest player score: %u", 0, 0, &players_score);
   } else {
       perror("Failed to read data from file.\n");
   }

   if (fscanf(file, " %u", &players_score) == 1) {
     add_text_to_buffer("Last single player score: %u", 0, 50, &players_score);
   } else {
       perror("Failed to read data from file.\n");
   }

  int player1_score, player2_score, player3_score;
   if (fscanf(file, " %u %u %u", &player1_score, &player2_score, &player3_score) == 2) {
     add_text_to_buffer("Last single player score: Red %u | Green %d | Blue %d", 0, 100, &players_score);
   } else {
       perror("Failed to read data from file.\n");
   }

   fclose(file);

}

void save_stats_to_file(unsigned int highest_players_score, unsigned int last_singleplayer, unsigned int last_multiplayer[3]) {
  FILE *file = fopen("stats.txt", "w");
   if (file == NULL) {
       perror("Error opening file");
       return 1;
   }

   fprintf(file, "%u %u %u %u %u", highest_players_score, last_singleplayer, last_multiplayer[0], last_multiplayer[1], last_multiplayer[2]);
   fclose(file);
}