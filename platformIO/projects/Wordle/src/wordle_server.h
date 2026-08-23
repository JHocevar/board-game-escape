#ifndef WORDLE_SERVER_H
#define WORDLE_SERVER_H

#include <Arduino.h>
#include <WebServer.h>

extern WebServer server;

void initServer();
void handleStatus();
void handleRestart();
void handleReboot();
void handleSetLetter();

#endif
