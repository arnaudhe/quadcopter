#include <utils/nmea_parser.h>
#include <hal/log.h>

#include <sstream>

NMEAParser::ParsedSentence::ParsedSentence(void)
{
    return;
}

bool NMEAParser::_check_start_delimiter(std::string *sequence_s)
{
    char leading_c = (*sequence_s).at(0);
    if ( leading_c == '$' )
    {
        (*sequence_s).erase(0, 1);
        return true;
    } else {
        return false;
    }
}


bool NMEAParser::_has_checksum(std::string *sequence_s)
{
    char crc_provided_c = (*sequence_s).at((*sequence_s).size() - 4 );
    if ( crc_provided_c == '*' )
    {
        return true;
    } else {
        return false;
    }
}


bool NMEAParser::_check_integrity(std::string *sequence_s)
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


bool NMEAParser::_parse_int(std::string *line_s, int *data_i)
{
    bool r = false;
    if ( line_s->size() > 0 )
    {
        *data_i = std::stoi((*line_s));
        r = true;
    }
    return r;
}


bool NMEAParser::_parse_float(std::string *line_s, float *data_f)
{
    bool r = false;
    if ( line_s->size() > 0 )
    {
        *data_f = std::stof((*line_s));
        r = true;
    }
    return r;
}


bool NMEAParser::_parse_char(std::string *line_s, char *data_c)
{
    bool r = false;
    if ( line_s->size() > 0 )
    {
        *data_c = (*line_s)[0];
        r = true;
    }
    return r;
}


bool NMEAParser::_parse_time (std::string *line_s, int *utc_hour, int *utc_minute, int *utc_second, float *utc_millis)
{
    bool r = false;
    if ( line_s->size() >= 6 )
    {
        *utc_hour = std::stoi(line_s->substr(0,2));
        *utc_minute = std::stoi(line_s->substr(2,2));
        *utc_second = std::stoi(line_s->substr(4,2));
        r = true;
    }
    if ( line_s->size() >= 8 )
    {
        *utc_millis = std::stof(line_s->substr(6));
    }
    return r;
}


bool NMEAParser::_parse_date(std::string *line_s, int *utc_day, int *utc_month, int *utc_year)
{
    bool r = false;
    if ( line_s->size() == 6 )
    {
        *utc_day = std::stoi(line_s->substr(0,2));
        *utc_month = std::stoi(line_s->substr(2,2));
        *utc_year = std::stoi(line_s->substr(4,2));
        r = true;
    }
    return r;
}


bool NMEAParser::_parse_coordinates(std::string *line_s, double *coordinates_deg, int *coordinates_min, float *coordinates_sec)
{
    bool r = false;
    if ( line_s->size() > 2 )
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


bool NMEAParser::_parse_latitude_cardinality(std::string *line_s, Cardinality *latitude_card, double *latitude_deg)
{
    bool r = false;
    char latitude_c = 'X';
    if ( line_s->size() == 1 )
    {
        latitude_c = (*line_s)[0];
    }
    if ( latitude_c == 'N' ) { *latitude_card = Cardinality::NORTH; r = true; }
    if ( latitude_c == 'S' ) { *latitude_card = Cardinality::SOUTH; r = true; (*latitude_deg) *= -1; }
    return r;
}


bool NMEAParser::_parse_longitude_cardinality(std::string *line_s, Cardinality *longitude_card, double *longitude_deg)
{
    bool r = false;
    char longitude_c = 'X';
    if ( line_s->size() == 1 )
    {
        longitude_c = (*line_s)[0];
    }
    if ( longitude_c == 'E' ) { *longitude_card = Cardinality::EAST; r = true; }
    if ( longitude_c == 'W' ) { *longitude_card = Cardinality::WEST; r = true; (*longitude_deg) *= -1; }
    return r;
}


NMEAParser::TalkerID NMEAParser::_resolve_talker_ID(std::string talker_ID)
{
    if( talker_ID == "GP" ) return GPS;
    if( talker_ID == "GA" ) return GALILEO;
    if( talker_ID == "GL" ) return GLONASS;
    if( talker_ID == "GB" ) return BEIDOU;
    return Unknown_talker_ID;
}


NMEAParser::MessageType NMEAParser::_resolve_message_type(std::string message_type)
{
    if( message_type == "GGA" ) return GGA;
    if( message_type == "GLL" ) return GLL;
    if( message_type == "GSA" ) return GSA;
    if( message_type == "GSV" ) return GSV;
    if( message_type == "VTG" ) return VTG;
    if( message_type == "RMC" ) return RMC;
    return Unknown_message_type;
}

bool NMEAParser::parse(std::string sequence, ParsedSentence * sentence)
{
    if ( ! _check_start_delimiter(&sequence))
    {
        LOG_WARNING("Invalid start sequence. Parsing aborted.");
        return false;
    }

    if ( _has_checksum(&sequence) )
    {
        if ( ! _check_integrity(&sequence) )
        {
            LOG_WARNING("Sequence integrity check fail. Parsing aborted.");
            return false;
        }
    }

    TalkerID talkerID = _resolve_talker_ID(sequence.substr(0, 2));
    switch ( talkerID )
    {
        case GPS:
        {
            MessageType message_type = _resolve_message_type(sequence.substr(2, 3));
            switch ( message_type )
            {
                case GGA:
                {
                    sentence->type = MessageType::GGA;

                    sequence.erase(0, 5);
                    std::istringstream sstr;
                    sstr.str(sequence);
                    int i = 0;
                    for (std::string line; std::getline(sstr, line, ','); )
                    {
                        switch ( i )
                        {
                            case 1: // UTC time
                            {
                                if ( _parse_time(&line, &(sentence->gga.utc_hour), &(sentence->gga.utc_minute), &(sentence->gga.utc_second), &(sentence->gga.utc_millis)) == false)
                                {
                                    sentence->gga.utc_hour   = 0;
                                    sentence->gga.utc_minute = 0;
                                    sentence->gga.utc_second = 0;
                                    sentence->gga.utc_millis = 0.;
                                }
                                break;
                            }
                            case 2: // latitude coordinates
                            {
                                if ( _parse_coordinates(&line, &(sentence->gga.latitude_deg), &(sentence->gga.latitude_min), &(sentence->gga.latitude_sec)) == false)
                                {
                                    sentence->gga.latitude_deg = 0.;
                                    sentence->gga.latitude_min = 0;
                                    sentence->gga.latitude_sec = 0.;
                                }
                                break;
                            }
                            case 3: // latitude cardinality
                            {
                                if ( _parse_latitude_cardinality(&line, &(sentence->gga.latitude_card), &(sentence->gga.latitude_deg)) == false)
                                {
                                    sentence->gga.latitude_card = Cardinality::UNKNOWN;
                                    sentence->gga.latitude_deg  = 0.;
                                }
                                break;
                            }
                            case 4: // longitude coordinates
                            {
                                if ( _parse_coordinates(&line, &(sentence->gga.longitude_deg), &(sentence->gga.longitude_min), &(sentence->gga.longitude_sec)) == false)
                                {
                                    sentence->gga.longitude_deg = 0.;
                                    sentence->gga.longitude_min = 0;
                                    sentence->gga.longitude_sec = 0.;
                                }
                                break;
                            }
                            case 5: // longitude cardinality
                            {
                                if ( _parse_longitude_cardinality(&line, &(sentence->gga.longitude_card), &(sentence->gga.longitude_deg)) == false)
                                {
                                    sentence->gga.longitude_card = Cardinality::UNKNOWN;
                                    sentence->gga.longitude_deg  = 0.;
                                }
                                break;
                            }
                            case 6: // FIX
                            {
                                if ( _parse_int(&line, &(sentence->gga.satellite_fix_status)) == false)
                                {
                                    sentence->gga.satellite_fix_status = 0;
                                }
                                break;
                            }
                            case 7: // satellite count
                            {
                                if ( _parse_int(&line, &(sentence->gga.fixed_satellite)) == false)
                                {
                                    sentence->gga.fixed_satellite = 0;
                                }
                                break;
                            }
                            case 9: // altitude mean sea level (MSL)
                            {
                                if ( _parse_float(&line, &(sentence->gga.altitude_msl)) == false)
                                {
                                    sentence->gga.altitude_msl = 0.;
                                }
                                break;
                            }
                            case 11: // geoidal separation
                            {
                                if ( _parse_float(&line, &(sentence->gga.geoidal_altitude_separation)) == false)
                                {
                                    sentence->gga.geoidal_altitude_separation = 0.;
                                }
                                break;
                            }
                            default:
                                break;
                            }
                        i++;
                    }
                    return true;
                    break;
                }
                case RMC:
                {
                    sentence->type = MessageType::RMC;

                    sequence.erase(0, 5);
                    std::istringstream sstr;
                    sstr.str(sequence);
                    int i = 0;
                    for (std::string line; std::getline(sstr, line, ','); )
                    {
                        switch ( i )
                        {
                            case 1: // UTC time
                            {
                                if ( _parse_time(&line, &(sentence->rmc.utc_hour), &(sentence->rmc.utc_minute), &(sentence->rmc.utc_second), &(sentence->rmc.utc_millis)) == false)
                                {
                                    sentence->gga.utc_hour   = 0;
                                    sentence->gga.utc_minute = 0;
                                    sentence->gga.utc_second = 0;
                                    sentence->gga.utc_millis = 0.;
                                }
                                break;
                            }
                            case 3: // latitude coordinates
                            {
                                if ( _parse_coordinates(&line, &(sentence->rmc.latitude_deg), &(sentence->rmc.latitude_min), &(sentence->rmc.latitude_sec)) == false)
                                {
                                    sentence->gga.latitude_deg = 0.;
                                    sentence->gga.latitude_min = 0;
                                    sentence->gga.latitude_sec = 0.;
                                }
                                break;
                            }
                            case 4: // latitude cardinality
                            {
                                if ( _parse_latitude_cardinality(&line, &(sentence->rmc.latitude_card), &(sentence->rmc.latitude_deg)) == false)
                                {
                                    sentence->gga.latitude_card = Cardinality::UNKNOWN;
                                    sentence->gga.latitude_deg  = 0.;
                                }
                                break;
                            }
                            case 5: // longitude coordinates
                            {
                                if ( _parse_coordinates(&line, &(sentence->rmc.longitude_deg), &(sentence->rmc.longitude_min), &(sentence->rmc.longitude_sec)) == false)
                                {
                                    sentence->gga.longitude_deg = 0.;
                                    sentence->gga.longitude_min = 0;
                                    sentence->gga.longitude_sec = 0.;
                                }
                                break;
                            }
                            case 6: // longitude cardinality
                            {
                                if ( _parse_longitude_cardinality(&line, &(sentence->rmc.longitude_card), &(sentence->rmc.longitude_deg)) == false)
                                {
                                    sentence->gga.longitude_card = Cardinality::UNKNOWN;
                                    sentence->gga.longitude_deg  = 0.;
                                }
                                break;
                            }
                            case 7: // ground speed
                            {
                                if ( _parse_float(&line, &(sentence->rmc.speed_ms)) == true)
                                {
                                    sentence->rmc.speed_ms *= 0.514444f; // knots to m/s convertion
                                }
                                else
                                {
                                    sentence->rmc.speed_ms = 0.;
                                }
                                break;
                            }
                            case 8: // course made good
                            {
                                if ( _parse_float(&line, &(sentence->rmc.course_made_good)) == false)
                                {
                                    sentence->rmc.course_made_good = 0.;
                                }
                                break;
                            }
                            case 9: // date
                            {
                                if ( _parse_date(&line, &(sentence->rmc.utc_day), &(sentence->rmc.utc_month), &(sentence->rmc.utc_year)) == true)
                                {
                                    sentence->rmc.utc_year += 2000;
                                }
                                else
                                {
                                    sentence->rmc.utc_day   = 0;
                                    sentence->rmc.utc_month = 0;
                                    sentence->rmc.utc_year  = 0;
                                }
                                break;
                            }
                        }
                        i++;
                    }
                    return true;
                    break;
                }
                case GLL:
                case GSA:
                case GSV:
                case VTG:
                {
                    LOG_VERBOSE("Unsupported message type. Parsing aborted.");
                    return false;
                    break;
                }
                case Unknown_message_type:
                default:
                {
                    LOG_WARNING("Unknown message type. Parsing aborted.");
                    return false;
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
            return false;
            break;
        }
        case Unknown_talker_ID:
        default:
        {
            LOG_WARNING("Unknown talker ID. Parsing aborted.");
            return false;
            break;
        }
    }
}
