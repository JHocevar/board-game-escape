#include "leds.h"
#include "game.h"

#include <FastLED.h>

/*
  leds[]: Array of CRGB objects representing the LED strip
*/
void displayLEDs(CRGB leds[]) {
  int word = 0; // Word Index

  // Color already guessed words
  while (word < previousWordIndex) {

    int row = 5 - word;

    // Check if the word exists, color if it does
    if (previousWords[word] != "") {
      CRGB colors[5];
      getWordColors(previousWords[word], colors, word);
      for (int j = 0; j < 5; j++) {
        int column = (row % 2 == 1) ? j : 4 - j;
        leds[(row * 5) + column] = colors[j];
      }

    // If it does not exist, turn it off
    } else {
      for (int j = 0; j < 5; j++) {
        int column = (row % 2 == 0) ? j : 4 - j;
        leds[(row * 5) + column] = CRGB::Black;
      }
    }
    word++;
  }

  // Color the current word being guessed
  if (currentGameState != WORD_GUESSED && currentGameState != GAME_OVER_FAILED) {
    int currentRow = 5 - word;
    for (int i = 0; i < 5; i++) {
      int column = (currentRow % 2 == 1) ? i : 4 - i;
      if (currentWord[i] != "") {
        leds[currentRow * 5 + column] = CRGB::White;
      } else {
        leds[currentRow * 5 + column] = CRGB::Black;
      }
    }
  } else {
    word--;
  }

  // turn off the remaining words
  for (int i = word + 1; i < 6; i++) {
    for (int j = 0; j < 5; j++) {
      int row = 5 - i;
      int column = (row % 2 == 1) ? j : 4 - j;
      leds[(row * 5) + column] = CRGB::Black;
    }
  }

  FastLED.show();
}
