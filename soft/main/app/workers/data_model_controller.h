#pragma once

#include <os/task.h>
#include <app/workers/broker.h>
#include <data_model/json_protocol.h>
#include <data_model/binary_protocol.h>

class DataModelController : public PeriodicTask
{

private:

    Broker              * _broker;
    JsonDataProtocol    * _json_protocol;
    BinaryDataProtocol  * _binary_protocol;

    void run(void);

public:

    DataModelController(float period, Broker * broker, JsonDataProtocol * json_protocol, BinaryDataProtocol * binary_protocol);

};