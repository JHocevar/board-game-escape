#include "game.h"

#include <Arduino.h>
#include <vector>
#include <FastLED.h>

#include "BGE_screen.h"
#include "BGE_server.h"

#include "words.h"
#include "rfid.h"


String CORRECT_WORD = "HOUSE";
String currentWord[NUM_READERS];    // Current word being filled in

String previousWords[6];    // Previous words guessed. Index 0 being first guess
int previousWordIndex = 0;  // Index of the next word to be guessed (0-6)


// Declare these once only
String rawCards[NUM_READERS];
String confirmedCards[NUM_READERS];
String candidateCards[NUM_READERS];
uint8_t candidateCount[NUM_READERS] = {0};

// Allow for manual override of cards from admin page.
String overrideCards[NUM_READERS] = {"", "", "", "", ""};
bool slotOverridden[NUM_READERS] = {false};

#define CONFIRM_READS 2 // How many consecutive reads of an RFID card are required to confirm the card is present

bool buttonPressedPrevious = false;

GameState currentGameState = GUESSING;
unsigned long wordleGuessCooldown = 0;

void initButton() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  buttonPressedPrevious = digitalRead(BUTTON_PIN) == LOW;
}

void checkStatus() {
  if (currentGameState == INVALID_WORD && wordleGuessCooldown <= millis()) {
    currentGameState = GUESSING;
    wordleGuessCooldown = 0;
  }
}

void printCurrentWord() {
  // Exit early if the word has already been guessed
  if (currentGameState == WORD_GUESSED) {
    return;
  }

  String wordToPrint = "";
  for (int i = 0; i < 5; i++) {

    // For each letter, if it's filled in, print the letter, otherwise print an underscore
    if (currentWord[i] != "") {
      wordToPrint += currentWord[i];
    } else {
      wordToPrint += "_";
    }

    // Add space between letters, except for last letter
    if (i < 4) {
      wordToPrint += " ";
    }
  }

  // Word width = 9 chars * 12px = 108px
  // Centering the word by starting at x = (128 - 108) / 2 = 10px
  printScreenText(wordToPrint, 10, 0, 2);
}

bool allLettersFilled() {
  for (int i = 0; i < 5; i++) {
    if (currentWord[i] == "") {
      return false;
    }
  }
  return true;
}

void printStatusMessage() {
  String retryMessage;

  switch (currentGameState) {
    case GUESSING:
      if (wordleGuessCooldown > millis()) {
        retryMessage = "Try again in " + String((wordleGuessCooldown - millis()) / 1000) + " s";
        // Word width = 17 chars * 6px = 102px
        // Centering the message by starting at x = (128 - 102) / 2 = 13px
        printScreenText(retryMessage, 13, 36, 1);
      } else if (!allLettersFilled()) {
        // Word witdh = 11 chars * 6px = 66px
        // Centering the message by starting at x = (128 - 66) / 2 = 31px
        printScreenText("Scan letter", 31, 36, 1);
      } else {
        // Word witdh = 12 chars * 6px = 72px
        // Centering the message by starting at x = (128 - 72) / 2 = 28px
        printScreenText("Submit word?", 28, 36, 1);
      }
      break;
    case INVALID_WORD:
      printScreenText("Not a valid word!", 10, 28, 1);
      retryMessage = "Try again in " + String((wordleGuessCooldown - millis()) / 1000) + " s";
      printScreenText(retryMessage, 13, 52, 1);
      break;
    case WORD_GUESSED:
      // Word witdhß = 13 chars * 6px = 78px
      // Centering the message by starting at x = (128 - 78) / 2 = 25px
      printScreenText("Word guessed!", 25, 28, 1);
      break;
  }
}

bool canSubmit() {
  if (wordleGuessCooldown > millis() || currentGameState == WORD_GUESSED) {
    if (wordleGuessCooldown > millis()) {
      Serial.print("Cannot submit, cooldown active for ");
      Serial.print((wordleGuessCooldown - millis()) / 1000);
      Serial.println(" seconds");
    } else {
      Serial.println("Cannot submit, word already guessed");
    }
    return false;
  }
  return allLettersFilled();
}

void handleSubmit() {
  Serial.println("previous words prior to submit:");
  logPreviousWords();

  if (!canSubmit()) {
    Serial.println("Cannot submit!");
    return;
  }

  Serial.println("Submitting word: " + String(currentWord[0]) + String(currentWord[1]) + String(currentWord[2]) + String(currentWord[3]) + String(currentWord[4]));

  // Build word from letters
  String word = "";
  for (int i = 0; i < 5; i++) {
    word += currentWord[i];
  }

  // First check if valid word
  if (!isValidWord(word)) {
    Serial.println("Invalid word: " + word);
    currentGameState = INVALID_WORD;
    wordleGuessCooldown = millis() + COOLDOWN_INVALID_WORD_SECONDS * 1000;
    for (int i = 0; i < 5; i++) {
      currentWord[i] = "";
    }
    return;
  }

  // Check if corret word
  if (word.equalsIgnoreCase(CORRECT_WORD)) {
    Serial.println("Correct word guessed: " + word);
    previousWords[previousWordIndex] = word;
    previousWordIndex += 1;
    currentGameState = WORD_GUESSED;
    return;
  }

  if (previousWordIndex >= 5) {
    Serial.println("Game over, max guesses reached");
    currentGameState = GAME_OVER_FAILED;
    wordleGuessCooldown = millis() + COOLDOWN_INVALID_WORD_SECONDS * 1000 * 5;
    return;
  }

  // Incorrect guess (valid word, but incorrect word)
  currentGameState = GUESSING;
  wordleGuessCooldown = millis() + COOLDOWN_WRONG_GUESS_SECONDS * 1000;

  previousWords[previousWordIndex] = word;
  previousWordIndex += 1;

  Serial.println("Incorrect guess, word added to previous words: " + word);
  logPreviousWords();
}

void getWordColors(String word, CRGB colors[5], int wordIndex) {
  word.toUpperCase();
  int letterCounts[26] = {0};

  // Pass 1: Mark greens, count unmatched correct letters
  for (int i = 0; i < 5; i++) {
    colors[i] = CRGB::Gray;
    if (word[i] == CORRECT_WORD[i]) {
      colors[i] = CRGB::Green;
    } else {
      letterCounts[CORRECT_WORD[i] - 'A']++;
    }
  }

  // Pass 2: Mark yellows
  for (int i = 0; i < 5; i++) {
    if (colors[i] == CRGB::Green) continue;
    char c = word[i];
    if (letterCounts[c - 'A'] > 0) {
      colors[i] = CRGB::Yellow;
      letterCounts[c - 'A']--;
    }
  }

}

void updateConfirmedCards(String* rawResults) {
  // For each reader, check if the raw result matches the candidate card.
  for (uint8_t i = 0; i < NUM_READERS; i++) {

    // If the raw result matches, increment the count
    if (rawResults[i] == candidateCards[i] && rawResults[i] != "") {
      candidateCount[i]++;

      // If we have reached the threshold, confirm the card
      if (candidateCount[i] >= CONFIRM_READS) {
        confirmedCards[i] = rawResults[i];
      }
    
    // If we did not match, reset the count and update the candidate
    } else {
      candidateCards[i] = rawResults[i];
      candidateCount[i] = 0;
      if (rawResults[i] == "") {
        confirmedCards[i] = "";
      }
    }
  }
}

void readLetters() {
  getRfidCards(rawCards);
  updateConfirmedCards(rawCards);

  for (uint8_t i = 0; i < NUM_READERS; i++) {
    if (slotOverridden[i]) {
      currentWord[i] = overrideCards[i];
    } else {
      currentWord[i] = confirmedCards[i];
    }
}
}

void checkSubmit() {
  bool buttonPressed = digitalRead(BUTTON_PIN) == LOW;

  if (buttonPressed && !buttonPressedPrevious) {
    Serial.println("Button pressed, checking submit");
    handleSubmit();
  }

  buttonPressedPrevious = buttonPressed;
}

void logPreviousWords() {
  Serial.println("Previous words:");
  for (int i = 0; i < previousWordIndex; i++) {
    if (i > 5) {
      // Exit early if we have already printed all 5 words
      return;
    }
    for (int j = 0; j < 5; j++) {
      Serial.print(previousWords[i][j]);
    }
    Serial.println();
  }
}