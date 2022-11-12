#include <serial_parser.hpp>
#include <string.h>
#include <stdexcept>

#define DEBUG true

#if DEBUG
#include "Arduino.h"
#endif

bool isNumber(char* str)
{
    if(*str == 0) return false;

    for(; *str != 0; str++)
        if(*str < '0' || *str > '9')
            return false;

    return true;
}

Parser::Command Parser::parseCommand(char* toParse)
{
    char* token = strtok(toParse, " ");

    if (strcmp(token, "help") == 0)
    {
        return Parser::Command::help;
    } 

    if (strcmp(token, "us") == 0) 
    {
        return parseSensorCommand();
    }

    return Parser::Command::none;
}

Parser::Command Parser::parseSensorCommand() 
{
    char* token = strtok(NULL, " ");

    if(token == NULL)
        return Parser::Command::list_sensors;

    if(!isNumber(token))
    {
        return Parser::Command::none;
    }
    
    sensorId_ = atoi(token);

    token = strtok(NULL, " ");

    if(strcmp(token, "delay") == 0)
    {
        Serial.println(1);
        token = strtok(NULL, " ");

        if(!isNumber(token))
            return Parser::Command::none;

        delay_ms_ = atoi(token);

        return Parser::Command::set_delay;
    }

    if(strcmp(token, "status") == 0)
    {
        return Parser::Command::get_status;
    }

    if(strcmp(token, "unit") == 0)
    {
        token = strtok(NULL, " ");

        // TODO: Actually check if string matches {inc | cm | ms}
        switch(token[0])
        {
            case 'i': unit_ = Srf02Config::Unit::inc; break;
            case 'c': unit_ = Srf02Config::Unit::cm;  break;
            case 'm': unit_ = Srf02Config::Unit::ms;  break;
            default:
                return Parser::Command::none;
        }

        return Parser::Command::set_unit;
    }

    if(strcmp(token, "one-shot") == 0)
    {
        operationMode_ = Srf02Config::OperationMode::one_shot;
        return Parser::Command::set_operation_mode;
    }

    if(strcmp(token, "off") == 0)
    {
        operationMode_ = Srf02Config::OperationMode::off;
        return Parser::Command::set_operation_mode;
    }

    if(strcmp(token, "on") == 0)
    {
        token = strtok(NULL, " ");
        
        if(!isNumber(token))
            return Parser::Command::none;

        operationMode_ = Srf02Config::OperationMode::on_period;
        period_ms_ = atoi(token);

        return Parser::Command::set_operation_mode;
    }
}

Parser::Command Parser::command()
{ 
    Command command = lastCommand_;
    lastCommand_ = Command::none;
    return command; 
}
