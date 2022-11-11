// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <CAN.h>
#include <serial_parser.hpp>
#include <array>

Parser sp;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.setTimeout(5000);

}

void readUntil( char until, char* buffer, size_t length)
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

  char buffer[1000];
  readUntil('\n', buffer, 1000);

  Parser::Command c;

  try {
    c = sp.parseCommand(buffer);
    Serial.print("Ok: ");
    Serial.println(c);
  } 
  catch(Parser::Error e)
  {
    Serial.print("Error: ");

    switch(e)
    {
      case Parser::Error::number_expected: Serial.println(1) ; break;
      case Parser::Error::unknown_command: Serial.println(2); break;
      case Parser::Error::unknown_unit:Serial.println(3); break;
    }
  }
}

