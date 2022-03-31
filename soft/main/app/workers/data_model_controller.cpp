#include <app/workers/data_model_controller.h>
#include <hal/log.h>

DataModelController::DataModelController(float period, Broker * broker, JsonDataProtocol * json_protocol, BinaryDataProtocol * binary_protocol):
    PeriodicTask("data_model_ctlr", Task::Priority::LOW, (int)(period * 1000), false)
{
    _broker          = broker;
    _json_protocol   = json_protocol;
    _binary_protocol = binary_protocol;

    _broker->register_channel(Broker::Channel::CONTROL, Broker::Medium::UDP_AND_RADIO);
}

void DataModelController::run(void)
{
    if (_broker->received_frame_pending(Broker::Channel::CONTROL))
    {
        tuple<ByteArray, Broker::Medium, string> recv = _broker->receive_from(Broker::Channel::CONTROL);

        ByteArray      request     = get<0>(recv);
        Broker::Medium medium      = get<1>(recv);
        string         src_address = get<2>(recv);

        LOG_INFO("Frame received %s %d %s", request.hex().c_str(), medium, src_address.c_str());

        if (request.length())
        {
            if (medium == Broker::Medium::UDP)
            {
                string response;
                _json_protocol->on_received_packet(string((const char *)request.data(), request.length()), response);
                _broker->send_to(Broker::Channel::CONTROL, ByteArray(response), src_address);
            }
            else if (medium == Broker::Medium::RADIO)
            {
                ByteArray response;
                _binary_protocol->on_received_packet(request, response);
                LOG_INFO("%s %s", request.hex().c_str(), response.hex().c_str());
                _broker->send_to(Broker::Channel::CONTROL, response, src_address);
            }
        }
    }
}
