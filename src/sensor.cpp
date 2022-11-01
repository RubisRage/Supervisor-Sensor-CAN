// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <srf02.hpp>
#include <Arduino.h>



void setup() {
  Serial.begin(9600);

  while(!Serial);
}

void loop() {
  Srf02 s1(0xE8);

  while (true) {
    int16_t range = 0;
    if(s1.readRange(range) == Srf02::Status::ok) {
      Serial.print("Rango: ");
      Serial.println(range);
    }
  }

}

