#include <serial_parser.hpp>
#include <string.h>

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

    throw Parser::Error::unknown_command;
}

Parser::Command Parser::parseSensorCommand()
{
    char* token = strtok(NULL, " ");

    if(token == NULL)
        return Parser::Command::list_sensors;

    token = strtok(NULL, " ");

    Serial.println((int)token[0]);

    if(!isNumber(token))
    {
        Serial.println("Throwing...");
        throw Parser::Error::number_expected;
    }
    
    sensorId_ = atoi(token);

    token = strtok(NULL, " ");

    if(strcmp(token, "delay"))
    {
        token = strtok(NULL, " ");

        if(!isNumber(token))
            throw Parser::Error::number_expected;

        delay_ms_ = atoi(token);

        return Parser::Command::set_delay;
    }

    if(strcmp(token, "status"))
    {
        return Parser::Command::get_status;
    }

    if(strcmp(token, "unit"))
    {
        token = strtok(NULL, " ");

        if(!isNumber(token))
            throw Parser::Error::number_expected;

        switch(token[0])
        {
            case 'i': unit_ = Srf02::Unit::inc; break;
            case 'c': unit_ = Srf02::Unit::cm;  break;
            case 'm': unit_ = Srf02::Unit::ms;  break;
            default:
                throw Parser::Error::unknown_unit;
        }

        return Parser::Command::set_unit;
    }

    if(strcmp(token, "one-shot"))
    {
        operationMode_ = Srf02::OperationMode::one_shot;
        return Parser::Command::set_operation_mode;
    }

    if(strcmp(token, "off"))
    {
        operationMode_ = Srf02::OperationMode::off;
        return Parser::Command::set_operation_mode;
    }

    if(strcmp(token, "on"))
    {
        token = strtok(NULL, " ");
        
        if(!isNumber(token))
            throw Parser::Error::number_expected;

        operationMode_ = Srf02::OperationMode::on_period;
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
