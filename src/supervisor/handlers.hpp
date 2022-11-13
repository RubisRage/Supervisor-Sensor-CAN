#pragma once

#include <parser.hpp>

typedef void (*handler_t)(Parser);

void noneHandler(Parser p);
void helpHandler(Parser p);
void listSensorsHandler(Parser p);
void getStatusHandler(Parser p);
void setDelayHandler(Parser p);
void setOpModeHandler(Parser p);
void setUnitHandler(Parser p);