#ifndef GAME_H
#define GAME_H

#define COOLDOWN_INVALID_WORD_SECONDS 30
#define COOLDOWN_WRONG_GUESS_SECONDS 10

#define BUTTON_PIN 26

#include <Arduino.h>
#include <FastLED.h>

#include "rfid.h"

extern String CORRECT_WORD;
extern unsigned long wordleGuessCooldown;
extern String overrideCards[NUM_READERS];
extern bool slotOverridden[NUM_READERS];

enum GameState {
  GUESSING,
  GUESSING_COOLDOWN,
  INVALID_WORD,
  WORD_GUESSED,
  GAME_OVER_FAILED,
};

extern GameState currentGameState;
extern String currentWord[5];
extern String previousWords[6];
extern int previousWordIndex;
extern String confirmedCards[5];


void initButton();
void checkStatus();
void printCurrentWord();
void printStatusMessage();
void readLetters();
void checkSubmit();
void getWordColors(String, CRGB[5], int);
void logPreviousWords();

#endif