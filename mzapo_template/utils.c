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

void get_stats_from_file(unsigned int *highest_players_score, unsigned int *all_pipes_passed) {
  FILE *file = fopen("stats.txt", "r");
   if (file == NULL) {
       perror("Error opening file");
       return 1;
   }

   int players_score, pipes_passed;
   if (fscanf(file, "%u %u", &players_score, &pipes_passed) == 2) {
       *highest_players_score = players_score;
       *all_pipes_passed = pipes_passed;
   } else {
       perror("Failed to read data from file.\n");
   }

   fclose(file);

}

void save_stats_to_file(unsigned int highest_players_score, unsigned int all_pipes_passed) {
  FILE *file = fopen("stats.txt", "w");
   if (file == NULL) {
       perror("Error opening file");
       return 1;
   }

   fprintf(file, "%u %u", highest_players_score, all_pipes_passed);
   fclose(file);
}