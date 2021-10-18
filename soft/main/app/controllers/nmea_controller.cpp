/****************************************************************************************************************************

NMEA controller class for ESP32 using IDF framework
nmea_controller.cpp

Created by Alexis Leprince (@lxslprnc) on October 17th 2021.
Copyright © 2021 Alexis Leprince (@lxslprnc). All rights reserved.

-----------------------------------------------------------------------------------------------------------------------------

Created to control a NMEA GPS module. This program mainly parses NMEA messages using a callback and then store position,
speed, date & time data in the project data resource registry. 
More details about NMEA messages here:
http://aprs.gids.nl/nmea/
Even if esp-idf framework works with many Espressif SoCs, this program has been designed for and tested on ESP-WROOM-32.
More details here:
https://github.com/espressif/esp-idf


MIT License

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy,
modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

-----------------------------------------------------------------------------------------------------------------------------

Change Activity:
  Date         Description
  -----------  -------------
  17 Oct 2021  Created.
  18 Oct 2021  Use project custom logging.
  18 Oct 2021  Use project custom task.

****************************************************************************************************************************/


/****************************************************************************************************************************
    Includes
****************************************************************************************************************************/

#include <string.h>
#include <sstream>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <hal/log.h>
#include <os/task.h>

#include <app/controllers/nmea_controller.h>


/****************************************************************************************************************************
    Class functions
****************************************************************************************************************************/

NMEA_controller::NMEA_controller(DataRessourcesRegistry * registry) :
    Task("NMEA CTRLR", Task::Priority::LOW, 4608, true)
{
    _registry = registry;
    _new_sequence = false;
    _worker_enable = true;

    LOG_INFO("NMEA controler object created.");
}


NMEA_controller::~NMEA_controller()
{
    _worker_enable = false;
    LOG_VERBOSE("NMEA controler object destroyed.");
}



void NMEA_controller::parse(int len, std::string str)
{
    _nmea_sequence = str;
    _new_sequence = true;
}


bool NMEA_controller::_check_start_delimiter(std::string *sequence_s)
{
    char leading_c = (*sequence_s).at(0);
    if ( leading_c == '$' )
    {
        LOG_VERBOSE("Valid start sequence");
        (*sequence_s).erase(0, 1);
        return true;
    } else {
        LOG_WARNING("Invalid start sequence");
        return false;
    }
}


bool NMEA_controller::_has_checksum(std::string *sequence_s)
{
    char crc_provided_c = (*sequence_s).at((*sequence_s).size() - 4 );
    if ( crc_provided_c == '*' )
    {
        LOG_VERBOSE("Sequence with checksum.");
        return true;
    } else {
        LOG_VERBOSE("Sequence without checksum.");
        return false;
    }
}


bool NMEA_controller::_check_integrity(std::string *sequence_s)
{
    bool r = false;
    std::string crc_provided_s = (*sequence_s).substr ((*sequence_s).size() - 3);
    uint8_t crc_provided = (uint8_t)strtol(crc_provided_s.c_str(), NULL, 16);
    LOG_VERBOSE("CRC provided: 0x%02hX.", unsigned(crc_provided));
    if ( crc_provided )
    {
        (*sequence_s).erase((*sequence_s).size() - 4, 4);
        uint8_t crc_computed = 0;
        for( char& c : (*sequence_s) )
        {
            crc_computed ^= (uint8_t)(c);
        }
        LOG_VERBOSE("CRC computed: 0x%02hX.", unsigned(crc_computed));
        if ( crc_provided == crc_computed )
        {
            LOG_VERBOSE("Sequence integrity check pass.");
            r = true;
        } else {
            LOG_WARNING("Sequence integrity check fail.");
        }
    } else {
        LOG_WARNING("Invalid checksum provided");
    }
    return r;
}


bool NMEA_controller::_parse_int(std::string *line_s, int *data_i)
{
    bool r = false;
    if ( (*line_s).size() > 0 )
    {
        *data_i = std::stoi((*line_s));
        r = true;
    }
    return r;
}


bool NMEA_controller::_parse_float(std::string *line_s, float *data_f)
{
    bool r = false;
    if ( (*line_s).size() > 0 )
    {
        *data_f = std::stof((*line_s));
        r = true;
    }
    return r;
}


bool NMEA_controller::_parse_char(std::string *line_s, char *data_c)
{
    bool r = false;
    if ( (*line_s).size() > 0 )
    {
        *data_c = (*line_s)[0];
        r = true;
    }
    return r;
}


bool NMEA_controller::_parse_time (std::string *line_s, int *utc_hour, int *utc_minute, int *utc_second, float *utc_millis)
{
    bool r = false;
    if ( (*line_s).size() >= 6 )
    {
        *utc_hour = std::stoi((*line_s).substr(0,2));
        *utc_minute = std::stoi((*line_s).substr(2,2));
        *utc_second = std::stoi((*line_s).substr(4,2));
        r = true;
    }
    if ( (*line_s).size() >= 8 )
    {
        *utc_millis = std::stof((*line_s).substr(6));
    }
    return r;
}


bool NMEA_controller::_parse_date(std::string *line_s, int *utc_day, int *utc_month, int *utc_year)
{
    bool r = false;
    if ( (*line_s).size() == 6 )
    {
        *utc_day = std::stoi((*line_s).substr(0,2));
        *utc_month = std::stoi((*line_s).substr(2,2));
        *utc_year = std::stoi((*line_s).substr(4,2));
        r = true;
    }
    return r;
}


bool NMEA_controller::_parse_coordinates(std::string *line_s, double *coordinates_deg, int *coordinates_min, float *coordinates_sec)
{
    bool r = false;
    if ( (*line_s).size() > 2 )
    {
        double dfloat = std::stod((*line_s));
        (*coordinates_deg) = (int)(dfloat / 100);
        dfloat -= (*coordinates_deg) * 100;
        (*coordinates_min) = dfloat;
        (*coordinates_deg) += dfloat / 60;
        (*coordinates_sec) = (dfloat - (int)(dfloat)) * 60;
        r = true;
    }
    return r;
}


bool NMEA_controller::_parse_latitude_cardinality(std::string *line_s, std::string *latitude_card, double *latitude_deg)
{
    bool r = false;
    char * latitude_c = new char('X');
    if ( (*line_s).size() == 1 )
    {
        (*latitude_c) = (*line_s)[0];
    }
    if ( (*latitude_c) == 'N' ) { *latitude_card = "north"; r = true; }
    if ( (*latitude_c) == 'S' ) { *latitude_card = "south"; r = true; (*latitude_deg) *= -1; }
    return r;
}


bool NMEA_controller::_parse_longitude_cardinality(std::string *line_s, std::string *longitude_card, double *longitude_deg)
{
    bool r = false;
    char * longitude_c = new char('X');
    if ( (*line_s).size() == 1 )
    {
        (*longitude_c) = (*line_s)[0];
    }
    if ( (*longitude_c) == 'E' ) { *longitude_card = "east"; r = true; }
    if ( (*longitude_c) == 'W' ) { *longitude_card = "west"; r = true; (*longitude_deg) *= -1; }
    return r;
}


void NMEA_controller::run()
{
    LOG_INFO("worker task started");

    while (_worker_enable ) {
        if ( _new_sequence )
        {
            LOG_VERBOSE("new sequence to parse");

            _parse();

            std::string str = _nmea_sequence;
            char *cstr = new char[str.length() + 1];
            strcpy(cstr, str.c_str());            
            LOG_VERBOSE("Sequence parsed:%s", cstr);
            delete [] cstr;
            _new_sequence = false;
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    LOG_INFO("stoppinng worker task");
    vTaskDelete(NULL);
}


void NMEA_controller::_parse()
{
    std::string str = _nmea_sequence;

    if ( ! _check_start_delimiter(&str) )
    {
        LOG_WARNING("Invalid start sequence. Parsing aborted.");
    }

    if ( _has_checksum(&str) )
    {
        if ( ! _check_integrity(&str) )
        {
            LOG_WARNING("Sequence integrity check fail. Parsing aborted.");
            return;
        }
    }

    int utc_hour = -1;
    int utc_minute = -1;
    int utc_second = -1;
    float utc_millis = -1.f;
    int utc_year = -1;
    int utc_month = -1;
    int utc_day = -1;

    double latitude_deg = -1.f;
    int latitude_min = -1.f;
    float latitude_sec = -1.f;
    std::string latitude_card = "undefined";
    
    double longitude_deg = -1.f;
    int longitude_min = -1.f;
    float longitude_sec = -1.f;
    std::string longitude_card = "undefined";

    float altitude_msl = -1.f;
    float geoidal_altitude_separation = -1.f;

    int satellite_fix_status = -1;
    int fixed_satellite = -1;

    float speed_ms = -1.f;
    float course_made_good = -1.f;

    NMEA_talker_ID talker_ID_e = resolve_talker_ID(str.substr(0, 2));
    switch ( talker_ID_e )
    {
        case GPS:
        {
            NMEA_message_type message_type_e = resolve_message_type(str.substr(2, 3));
            switch ( message_type_e )
            {
                case GGA:
                {
                    str.erase(0, 5);
                    std::istringstream sstr;
                    sstr.str(str);
                    int i = 0;
                    for (std::string line; std::getline(sstr, line, ','); )
                    {
                        switch ( i )
                        {
                            case 1: // UTC time
                            {
                                if ( _parse_time(&line, &utc_hour, &utc_minute, &utc_second, &utc_millis) )
                                {
                                    _registry->internal_set<int>("gps.time.hour", utc_hour);
                                    _registry->internal_set<int>("gps.time.minute", utc_minute);
                                    _registry->internal_set<int>("gps.time.second", utc_second);
                                    _registry->internal_set<float>("gps.time.millis", utc_millis);
                                }
                                break;
                            }
                            case 2: // latitude coordinates
                            {
                                if ( _parse_coordinates(&line, &latitude_deg, &latitude_min, &latitude_sec) )
                                {
                                    _registry->internal_set<double>("gps.position.latitude.deg", latitude_deg);
                                    _registry->internal_set<int>("gps.position.latitude.min", latitude_min);
                                    _registry->internal_set<float>("gps.position.latitude.sec", latitude_sec);
                                }
                                break;
                            }
                            case 3: // latitude cardinality
                            {
                                if ( _parse_latitude_cardinality(&line, &latitude_card, &latitude_deg) )
                                {
                                    _registry->internal_set<string>("gps.position.latitude.cardinality", latitude_card);
                                    _registry->internal_set<double>("gps.position.latitude.deg", latitude_deg);
                                }
                                break;
                            }
                            case 4: // longitude coordinates
                            {
                                if ( _parse_coordinates(&line, &longitude_deg, &longitude_min, &longitude_sec) )
                                {
                                    _registry->internal_set<double>("gps.position.longitude.deg", longitude_deg);
                                    _registry->internal_set<int>("gps.position.longitude.min", longitude_min);
                                    _registry->internal_set<float>("gps.position.longitude.sec", longitude_sec);
                                }
                                break;
                            }
                            case 5: // longitude cardinality
                            {
                                if ( _parse_longitude_cardinality(&line, &longitude_card, &longitude_deg) )
                                {
                                    _registry->internal_set<string>("gps.position.longitude.cardinality", longitude_card);
                                    _registry->internal_set<double>("gps.position.longitude.deg", longitude_deg);
                                }
                                break;
                            }
                            case 6: // FIX
                            {
                                if ( _parse_int(&line, &satellite_fix_status) )
                                {
                                    _registry->internal_set<string>("gps.status.module_status", ( satellite_fix_status > 0 ? "fixed" : "unfixed"));
                                }
                                break;
                            }
                            case 7: // satellite count
                            {
                                if ( _parse_int(&line, &fixed_satellite) )
                                {
                                    _registry->internal_set<int>("gps.status.fixed_satellite", fixed_satellite);
                                }
                                break;
                            }
                            case 9: // altitude mean sea level (MSL)
                            {
                                if ( _parse_float(&line, &altitude_msl) )
                                {
                                    _registry->internal_set<float>("gps.position.altitude.msl", altitude_msl);
                                }
                                break;
                            }
                            case 11: // geoidal separation
                            {
                                if ( _parse_float(&line, &geoidal_altitude_separation) )
                                {
                                    _registry->internal_set<float>("gps.position.altitude.geo", geoidal_altitude_separation);
                                }
                                break;
                            }
                            default:
                                break;
                            }
                        i++;
                    }
                    break;
                }
                case RMC:
                {
                    str.erase(0, 5);
                    std::istringstream sstr;
                    sstr.str(str);
                    int i = 0;
                    for (std::string line; std::getline(sstr, line, ','); )
                    {
                        switch ( i )
                        {
                            case 1: // UTC time
                            {
                                if ( _parse_time(&line, &utc_hour, &utc_minute, &utc_second, &utc_millis) )
                                {
                                    _registry->internal_set<int>("gps.time.hour", utc_hour);
                                    _registry->internal_set<int>("gps.time.minute", utc_minute);
                                    _registry->internal_set<int>("gps.time.second", utc_second);
                                    _registry->internal_set<float>("gps.time.millis", utc_millis);
                                }
                                break;
                            }
                            case 3: // latitude coordinates
                            {
                                if ( _parse_coordinates(&line, &latitude_deg, &latitude_min, &latitude_sec) )
                                {
                                    _registry->internal_set<double>("gps.position.latitude.deg", latitude_deg);
                                    _registry->internal_set<int>("gps.position.latitude.min", latitude_min);
                                    _registry->internal_set<float>("gps.position.latitude.sec", latitude_sec);
                                }
                                break;
                            }
                            case 4: // latitude cardinality
                            {
                                if ( _parse_latitude_cardinality(&line, &latitude_card, &latitude_deg) )
                                {
                                    _registry->internal_set<string>("gps.position.latitude.cardinality", latitude_card);
                                    _registry->internal_set<double>("gps.position.latitude.deg", latitude_deg);
                                }
                                break;
                            }
                            case 5: // longitude coordinates
                            {
                                if ( _parse_coordinates(&line, &longitude_deg, &longitude_min, &longitude_sec) )
                                {
                                    _registry->internal_set<double>("gps.position.longitude.deg", longitude_deg);
                                    _registry->internal_set<int>("gps.position.longitude.min", longitude_min);
                                    _registry->internal_set<float>("gps.position.longitude.sec", longitude_sec);
                                }
                                break;
                            }
                            case 6: // longitude cardinality
                            {
                                if ( _parse_longitude_cardinality(&line, &longitude_card, &longitude_deg) )
                                {
                                    _registry->internal_set<string>("gps.position.longitude.cardinality", longitude_card);
                                    _registry->internal_set<double>("gps.position.longitude.deg", longitude_deg);
                                }
                                break;
                            }
                            case 7: // ground speed
                            {
                                if ( _parse_float(&line, &speed_ms) )
                                {
                                    speed_ms *= 0.514444f; // knots to m/s convertion
                                    _registry->internal_set<float>("gps.speed.ground_speed", speed_ms);
                                }
                                break;
                            }
                            case 8: // course made good
                            {
                                if ( _parse_float(&line, &course_made_good) )
                                {
                                    _registry->internal_set<float>("gps.speed.cmg_speed", course_made_good);
                                }
                                break;
                            }
                            case 9: // date
                            {
                                if ( _parse_date(&line, &utc_day, &utc_month, &utc_year) )
                                {
                                    _registry->internal_set<int>("gps.date.day", utc_day);
                                    _registry->internal_set<int>("gps.date.month", utc_month);
                                    _registry->internal_set<int>("gps.date.year", 2000 + utc_year);
                                }
                                break;
                            }
                        }
                        i++;
                    }
                    break;
                }
                case GLL:
                case GSA:
                case GSV:
                case VTG:
                {
                    LOG_VERBOSE("Unsupported message type. Parsing aborted.");
                    break;
                }
                case Unknown_message_type:
                default:
                {
                    LOG_WARNING("Unknown message type. Parsing aborted.");
                    break;
                }
            }
            break;
        }
        case GALILEO:
        case GLONASS:
        case BEIDOU:
        {
            LOG_VERBOSE("Unsupported talker ID. Parsing aborted.");
            break;
        }
        case Unknown_talker_ID:
        default:
        {
            LOG_WARNING("Unknown talker ID. Parsing aborted.");
            break;
        }
    }
}


NMEA_talker_ID NMEA_controller::resolve_talker_ID(std::string talker_ID)
{
    if( talker_ID == "GP" ) return GPS;
    if( talker_ID == "GA" ) return GALILEO;
    if( talker_ID == "GL" ) return GLONASS;
    if( talker_ID == "GB" ) return BEIDOU;
    return Unknown_talker_ID;
}


NMEA_message_type NMEA_controller::resolve_message_type(std::string message_type)
{
    if( message_type == "GGA" ) return GGA;
    if( message_type == "GLL" ) return GLL;
    if( message_type == "GSA" ) return GSA;
    if( message_type == "GSV" ) return GSV;
    if( message_type == "VTG" ) return VTG;
    if( message_type == "RMC" ) return RMC;
    return Unknown_message_type;
}