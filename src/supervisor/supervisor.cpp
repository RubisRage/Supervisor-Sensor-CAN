#include <CAN.h>
#include <parser.hpp>
#include <can_format.hpp>
#include "handlers.hpp"

Parser p;

void error_handler()
{}


#define HANDLER_NUM 7

handler_t handlers[HANDLER_NUM];

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.setTimeout(5000);

  if(!CAN.begin(500E3))
  {
    Serial.println("Starting CAN failed!");
    exit(1);
  }

  handlers[Parser::Command::none] = noneHandler;
  handlers[Parser::Command::help] = helpHandler;
  handlers[Parser::Command::set_operation_mode] = setOpModeHandler;

  // handlers[Parser::Command::list_sensors] =
  // handlers[Parser::Command::get_status] =
  // handlers[Parser::Command::set_delay] = 
  // handlers[Parser::Command::set_unit] = 
}

void readUntil(char until, char* buffer, size_t length)
{
  char current = 0;

  size_t i = 0;

  while(i < length && current != until)
  {
    if(Serial.available())
    {
      current = Serial.read();
      buffer[i++] = current;
    }
  }

  buffer[i-1] = 0;
}

void loop() 
{
  #define SIZE 1024
  char buffer[SIZE];

  readUntil('\n', buffer, SIZE);

  Parser::Command c = p.parseCommand(buffer);

  handlers[c](p);
}
