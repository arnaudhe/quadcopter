#include <app/data_recorder/telemetry.h>

#include <hal/log.h>
#include <utils/string_utils.h>

using namespace std;

Telemetry::Telemetry(DataRessourcesRegistry * registry, int period) :
    Task("telemetry", Task::Priority::VERY_LOW, 4096, false)
{
    _registry = registry;

    _registry->internal_set<int>("telemetry.period", period);
    _registry->internal_set<bool>("telemetry.enable", false);
    _registry->internal_set<string>("telemetry.values", "");
}

void Telemetry::run(void)
{
    bool enable = false;
    int  period = 100;
    vector<string> keys;

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
                            LOG_INFO("%s %d", key.c_str(), _registry->internal_get<bool>(key));
                            break;

                        case DataRessource::INTEGER:
                            LOG_INFO("%s %d", key.c_str(), _registry->internal_get<int>(key));
                            break;

                        case DataRessource::FLOAT:
                            LOG_INFO("%s %f", key.c_str(), _registry->internal_get<float>(key));
                            break;

                        case DataRessource::DOUBLE:
                            LOG_INFO("%s %f", key.c_str(), _registry->internal_get<double>(key));
                            break;

                        case DataRessource::ENUM:
                        case DataRessource::STRING:
                            LOG_INFO("%s %s", key.c_str(), _registry->internal_get<string>(key).c_str());
                            break;

                        default:
                            break;
                    }
                }
            }
        }
        else
        {
            enable = false;
        }
        Task::delay_ms(period);
    }
}
