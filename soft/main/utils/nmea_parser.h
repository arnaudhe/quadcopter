#pragma once

#include <string>

class NMEAParser
{

public:

    enum Cardinality {
        NORTH,
        SOUTH,
        WEST,
        EAST,
        UNKNOWN
    };

    enum TalkerID {
        GPS,
        GALILEO,
        GLONASS,
        BEIDOU,
        Unknown_talker_ID
    };

    enum MessageType {
        GGA,
        GLL,
        GSA,
        GSV,
        VTG,
        RMC,
        Unknown_message_type
    };

private:

    static bool _check_start_delimiter(std::string *sequence_s);
    static bool _has_checksum(std::string *sequence_s);
    static bool _check_integrity(std::string *sequence_s);
    static bool _parse_int(std::string *line_s, int *data_i);
    static bool _parse_float(std::string *line_s, float *data_f);
    static bool _parse_char(std::string *line_s, char *data_c);
    static bool _parse_time(std::string *line_s, int *utc_hour, int *utc_minute,
                            int *utc_second, float *utc_millis);
    static bool _parse_date(std::string *line_s, int *utc_day, int *utc_month,
                            int *utc_year);
    static bool _parse_coordinates(std::string *line_s, double *coordinates_deg,
                                    int *coordinates_min, float *coordinates_sec);
    static bool _parse_latitude_cardinality(std::string *line_s,
                                            Cardinality *latitude_card,
                                            double *latitude_deg);
    static bool _parse_longitude_cardinality(std::string *line_s,
                                             Cardinality *longitude_card,
                                             double *longitude_deg);

    static TalkerID _resolve_talker_ID(std::string talker_ID);
    static MessageType _resolve_message_type(std::string message_type);

public:

    struct ParsedSentence {
        MessageType type;
        union {
            struct {
                int utc_hour;
                int utc_minute;
                int utc_second;
                float utc_millis;

                double latitude_deg;
                int latitude_min;
                float latitude_sec;
                Cardinality latitude_card;

                double longitude_deg;
                int longitude_min;
                float longitude_sec;
                Cardinality longitude_card;

                float altitude_msl;
                float geoidal_altitude_separation;

                int fixed_satellite;
                int satellite_fix_status;
            }gga;
            struct {
                int utc_hour;
                int utc_minute;
                int utc_second;
                float utc_millis;

                int utc_year;
                int utc_month;
                int utc_day;

                double latitude_deg;
                int latitude_min;
                float latitude_sec;
                Cardinality latitude_card;

                double longitude_deg;
                int longitude_min;
                float longitude_sec;
                Cardinality longitude_card;

                float speed_ms;
                float course_made_good;

                int satellite_fix_status;
            }rmc;
        };

        ParsedSentence(void);
    };

    static bool parse(std::string str, ParsedSentence * sentence);
};
