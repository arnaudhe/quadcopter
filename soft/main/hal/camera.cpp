#include <hal/camera.h>
#include <utils/json.hpp>
#include <utils/byte_array.h>

#include <hal/log.h>

#define CAMERA_IP       "192.168.42.1"
#define CAMERA_PORT     7878

using json = nlohmann::json;
using namespace std;

Camera::Camera(Wifi * wifi) :
    Task("camera", Task::Priority::VERY_LOW, 4096, false)
{
    _tcp           = new Tcp();
    _wifi          = wifi;
    _mutex         = new Mutex();
    _battery_level = 0;
    _request       = NONE;
    _token         = 0;
    _recv_data     = new ByteArray();
}

void Camera::run(void)
{
    int         json_counter = 0, json_flip = 0;
    ByteArray   recv_byte;
    ByteArray   recv_data = ByteArray();

    while(1)
    {
        if (_wifi->get_state() == Wifi::State::CONNECTED)
        {
            if (_tcp->is_connected() == false)
            {
                LOG_DEBUG("Trying to connect to %s:%d", CAMERA_IP, CAMERA_PORT);
                if (_tcp->connect(CAMERA_IP, CAMERA_PORT))
                {
                    /* Force token refresh with re-authentication */
                    _token = 0;
                    LOG_INFO("Connected");
                }
            }
        }
        else
        {
            if (_tcp->is_connected())
            {
                _tcp->disconnect();
            }
        }

        if (_tcp->is_connected())
        {
            recv_byte = _tcp->receive(1);
            if (recv_byte.length() == 1)
            {
                recv_data.append(recv_byte);
                if ((char)(recv_byte(0)) == '{')
                {
                    json_counter += 1;
                    json_flip = 1;
                }
                else if ((char)(recv_byte(0)) == '}')
                {
                    json_counter -= 1;
                }
                if ((json_flip == 1) and (json_counter == 0))
                {
                    LOG_DEBUG("%.*s", recv_data.length(), recv_data.data());
                    _mutex->lock();
                    _recv_data->set_data(recv_data.data(), recv_data.length());
                    _mutex->unlock();
                    recv_data.clear();
                }
            }
        }
        else
        {
            Task::delay_ms(1000);
            _mutex->lock();
            _recv_data->clear();
            _mutex->unlock();
        }
    }
}

bool Camera::send_request(Command command)
{
    json   json_request;
    string request;

    json_request["msg_id"] = command;
    json_request["token"]  = _token;

    request = json_request.dump();

    LOG_DEBUG("%s", request.c_str());

    if (_tcp->send(ByteArray(request)))
    {
        return true;
    }
    else
    {
        LOG_ERROR("Send command %d error", command);
        return false;
    }
}

bool Camera::wait_response(Command &command, int &rval, int &param)
{
    ByteArray response_bytes;
    string    response;
    json      json_response;

    while ((response_bytes.length() == 0) && (is_connected()))
    {
        _mutex->lock();
        if (_recv_data->length())
        {
            /* Copy the received data in local bytes array */
            response_bytes.set_data(_recv_data->data(), _recv_data->length());
            /* Clear the reception data shared variable to make it available */
            _recv_data->clear();
        }
        _mutex->unlock();
        Task::delay_ms(10);
    }

    if (response_bytes.length())
    {
        /* convert and parse the received response */
        response = string((const char *)response_bytes.data(), response_bytes.length());
        json_response = json::parse(response);

        if (json_response.is_object())
        {
            command = (Command)json_response["msg_id"].get<int>();
            rval    = 0;
            param   = 0;

            if (json_response.contains("rval"))
            {
                rval = json_response["rval"].get<int>();
            }

            if (json_response.contains("param"))
            {
                if (json_response["param"].is_number_integer())
                {
                    param = json_response["param"].get<int>();
                }
                else if (json_response["param"].is_string())
                {
                    param = stoi(json_response["param"].get<string>());
                }
                else if (json_response["param"].is_boolean())
                {
                    param = (int)(json_response["param"].get<bool>());
                }
            }

            return true;
        }
        else
        {
            LOG_WARNING("Invalid response");
            return false;
        }
    }
    else
    {
        LOG_WARNING("Connection lost");
        return false;
    }
}

bool Camera::execute_command(Command command, int &output_param)
{
    bool       result = false;
    Command    response_command;
    int        response_rval;
    int        response_param;

    if (is_connected() == true)
    {
        if ((command != Command::COMMAND_AUTHENT) && (_token == 0))
        {
            /* Execute authentication command */
            execute_command(Command::COMMAND_AUTHENT);
        }

        do
        {
            if (send_request(command) == true)
            {
                if (wait_response(response_command, response_rval, response_param) == true)
                {
                    if ((response_command == Command::COMMAND_AUTHENT) && (response_rval == 0))
                    {
                        LOG_DEBUG("Update token to %d", response_param);
                        _token = response_param;
                    }

                    if (response_command == command)
                    {
                        if (response_rval == 0)
                        {
                            output_param = response_param;
                            result = true;
                        }
                        else if (response_rval == -4)
                        {
                            LOG_DEBUG("Out-of-date token");
                            _token = 0;
                        }
                        else
                        {
                            LOG_WARNING("Command error %d", response_rval);
                        }
                    }
                }
                else
                {
                    break;
                }
            }
            else
            {
                break;
            }
        } while (response_command != command);
    }
    else
    {
        LOG_ERROR("Cannot execute command while disconnected");
    }

    return result;
}

bool Camera::execute_command(Command command)
{
    int dummy;
    return execute_command(command, dummy);
}

void Camera::get_token()
{
    int token;

    execute_command(Command::COMMAND_AUTHENT, token);

    LOG_INFO("Successfully logged int with token %d", token);

    _token = token;
}

bool Camera::is_connected()
{
    return _tcp->is_connected();
}

int Camera::get_battery_level()
{
    int battery_level;

    execute_command(Command::COMMAND_GET_BATTERY, battery_level);

    _battery_level = battery_level;

    return _battery_level;
}

bool Camera::take_picture()
{
    return execute_command(Command::COMMAND_PHOTO);
}

bool Camera::start_recording()
{
    return execute_command(Command::COMMAND_START_RECORDING);
}

bool Camera::stop_recording()
{
    return execute_command(Command::COMMAND_STOP_RECORDING);
}
