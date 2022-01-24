#include <app/data_recorder/telemetry.h>

#include <hal/log.h>
#include <utils/string_utils.h>
#include <sstream>
#include <esp_attr.h>

#define TELEMETRY_DEST_PORT     5003
#define TELEMETRY_SEND_PERIOD   50

using namespace std;

Telemetry::Telemetry(DataRessourcesRegistry * registry, int period, UdpServer * udp) :
    Task("telemetry", Task::Priority::VERY_LOW, 4096, false)
{
    _registry = registry;
    _udp = udp;

    _registry->internal_set<int>("telemetry.period", period);
    _registry->internal_set<bool>("telemetry.enable", false);
    _registry->internal_set<string>("telemetry.values", "");
}

void IRAM_ATTR Telemetry::run(void)
{
    bool enable = false;
    int  period = 100;
    vector<string> keys;
    stringstream telemetry_data;
    TickType_t tick;

    tick = xTaskGetTickCount();

    /* Clear the stringstream content */
    telemetry_data.str(string());

    while(1)
    {
        if (_registry->internal_get<bool>("telemetry.enable"))
        {
            if (enable == false)
            {
                LOG_INFO("Start Telemetry");
                keys = StringUtils::split(_registry->internal_get<string>("telemetry.values"), ";");
                period = _registry->internal_get<int>("telemetry.period");
                enable = true;
            }

            for (const string & key : keys)
            {
                if (key.length())
                {
                    switch (_registry->type(key))
                    {
                        case DataRessource::BOOL:
                            telemetry_data << _registry->internal_get<bool>(key);
                            break;

                        case DataRessource::INTEGER:
                            telemetry_data << _registry->internal_get<int>(key);
                            break;

                        case DataRessource::FLOAT:
                            telemetry_data << _registry->internal_get<float>(key);
                            break;

                        case DataRessource::DOUBLE:
                            telemetry_data << _registry->internal_get<double>(key);
                            break;

                        case DataRessource::ENUM:
                        case DataRessource::STRING:
                            telemetry_data << _registry->internal_get<string>(key).c_str();
                            break;

                        default:
                            break;
                    }
                }

                telemetry_data << ";";
            }

            /* Remove last separator */
            telemetry_data.seekp(-1, std::ios_base::end);
            telemetry_data << '\n';

            if ((unsigned int)(xTaskGetTickCount() - tick) > TELEMETRY_SEND_PERIOD)
            {
                /* Send telemetry data through udp */
                _udp->send_broadcast(telemetry_data.str(), TELEMETRY_DEST_PORT);
                tick = xTaskGetTickCount();
                telemetry_data.str(string());
            }

            Task::delay_ms(period);
        }
        else
        {
            enable = false;

            Task::delay_ms(100);
        }
    }
}
