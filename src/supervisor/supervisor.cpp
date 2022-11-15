#include <CAN.h>
#include <parser.hpp>
#include <can_format.hpp>
#include "handlers.hpp"
#include <srf02_config.hpp>

Parser p;

#define HANDLER_NUM 7
handler_t handlers[HANDLER_NUM];

const char* unit2string(Srf02Config::Unit unit)
{
    static const char* unitStrings[] = {
        "inc",
        "cm",
        "ms"
    };

    return unitStrings[unit - Srf02Config::Unit::inc];
}

void onReceive(int packetSize)
{
  long id = CAN.packetId();

  switch(id)
  {
    case CAN_ID::ACK:
      Serial.println("Ok");
    break;
    
    case CAN_ID::ERR:
      ERR_M err;
      readMessage<ERR_M>(err);

      Serial.println(errorMsg[err.error]);
    break;

    case CAN_ID::LIST_RESPONSE:
      LIST_SENSORS_RESPONSE_M list;
      readMessage(list);

      Serial.println("Available sensors:");
      Serial.print("\tId: ");
      Serial.println(list.sensorId1);
      Serial.print("\tId: ");
      Serial.println(list.sensorId2);
    break;

    case CAN_ID::STATUS_RESPONSE:
      STATUS_RESPONSE_M status;
      readMessage<STATUS_RESPONSE_M>(status);

      Serial.print("\t[ Sensor ");
      Serial.print(status.sensorId);
      Serial.println(" ]");
      Serial.print("\t-> I2C address: 0x");
      Serial.println(status.i2cAddr, HEX);
      Serial.print("\t-> Delay: ");
      Serial.print(status.delay);
      Serial.println(" ms");
      Serial.print("\t-> Unit: ");
      Serial.println(unit2string(status.unit));

      Serial.print("\tPeriod: ");

      if(status.period_ms == 0)
        Serial.println("Not activated.");
      else
      {
        Serial.print(status.period_ms);
        Serial.println(" ms");
      }
    break;

    case CAN_ID::SENSOR_MEASUREMENT:
      SENSOR_MEASUREMENT_M measurement;
      readMessage<SENSOR_MEASUREMENT_M>(measurement);

      Serial.print("\t-> Sensor ");
      Serial.print(measurement.sensorId);
      Serial.print(" measures: ");
      Serial.print(measurement.range);
      Serial.print(" ");
      Serial.println(unit2string(measurement.unit));
    break;
  }

}

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.setTimeout(5000);

  if(!CAN.begin(500E3))
  {
    Serial.println("Starting CAN failed!");
    exit(1);
  }

  CAN.onReceive(onReceive);

  handlers[Parser::Command::none]               = noneHandler;
  handlers[Parser::Command::help]               = helpHandler;
  handlers[Parser::Command::set_operation_mode] = setOpModeHandler;
  handlers[Parser::Command::set_delay]          = setDelayHandler;
  handlers[Parser::Command::set_unit]           = setUnitHandler; 
  handlers[Parser::Command::list_sensors]       = listSensorsHandler; 
  handlers[Parser::Command::get_status]         = getStatusHandler;
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

