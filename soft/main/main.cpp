#include <nvs_flash.h>

#include <string.h>

#include <platform.h>

#include <app/controllers/attitude_controller.h>
#include <app/controllers/height_controller.h>
#include <app/controllers/position_controller.h>
#include <app/controllers/rate_controller.h>
#include <app/controllers/motors_controller.h>
#include <app/data_recorder/data_recorder.h>
#include <app/data_recorder/telemetry.h>
#include <app/workers/battery_supervisor.h>
#include <app/workers/camera_controller.h>

#include <data_model/data_ressources_registry.h>
#include <data_model/json_protocol.h>

#include <periph/i2c_master.h>
#include <periph/uart.h>
#include <periph/spi.h>

#include <hal/gps.h>
#include <hal/log.h>
#include <hal/marg.h>
#include <hal/motor.h>
#include <hal/udp_server.h>
#include <hal/wifi.h>
#include <hal/radio_command.h>
#include <drv/SI4432.h>

#include <os/task.h>

#include <utils/mixer.h>

// #define DATA_RECORDING

extern "C" void app_main(void)
{
    Logger                  * logger;
    Motor                   * front_left;
    Motor                   * front_right;
    Motor                   * rear_left;
    Motor                   * rear_right;
    I2cMaster               * sensors_i2c;
    Marg                    * marg;
    SPIHost                 * spi;
    Si4432                  * transceiver;
#ifdef DATA_RECORDING
    DataRecorder            * data_recorder;
#else
    Mixer                   * mixer;
    RateController          * rate_controller;
    AttitudeController      * attitude_controller;
    HeightController        * height_controller;
    PositionController      * position_controller;
    Gps                     * gps;
    MotorsController        * motors_controller;
    Wifi                    * wifi;
    UdpServer               * udp;
    DataRessourcesRegistry  * registry;
    JsonDataProtocol        * protocol;
    BatterySupervisor       * battery;
    CameraController        * camera;
    UltrasoundSensor        * ultrasound;
    Barometer               * barometer;
    Telemetry               * telemetry;
    RadioCommand            * radio_command;

#endif

    nvs_flash_init();

    sensors_i2c = new I2cMaster(I2C_MASTER_NUM);
    sensors_i2c->init();

    marg = new Marg(sensors_i2c);
    marg->init();

    spi = new SPIHost(PLATFORM_SPI_NUM, PLATFORM_SPI_MOSI_PIN, PLATFORM_SPI_MISO_PIN, PLATFORM_SPI_CLK_PIN, 64);
    transceiver = new Si4432(spi);

#ifndef DATA_RECORDING
    barometer = new Barometer(sensors_i2c);
    barometer->init();

    ultrasound = new UltrasoundSensor();
#endif

    front_left  = new Motor(PLATFORM_FRONT_LEFT_PULSE_CHANNEL,  PLATFORM_FRONT_LEFT_PULSE_PIN);
    front_right = new Motor(PLATFORM_FRONT_RIGHT_PULSE_CHANNEL, PLATFORM_FRONT_RIGHT_PULSE_PIN);
    rear_left   = new Motor(PLATFORM_REAR_LEFT_PULSE_CHANNEL,   PLATFORM_REAR_LEFT_PULSE_PIN);
    rear_right  = new Motor(PLATFORM_REAR_RIGHT_PULSE_CHANNEL,  PLATFORM_REAR_RIGHT_PULSE_PIN);

    front_left->arm();
    front_right->arm();
    rear_left->arm();
    rear_right->arm();

    front_left->set_speed(0.0f);
    front_right->set_speed(0.0f);
    rear_left->set_speed(0.0f);
    rear_right->set_speed(0.0f);

#ifdef DATA_RECORDING
    data_recorder       = new DataRecorder(front_left, front_right, rear_left, rear_right, marg);
#else
    registry            = new DataRessourcesRegistry("data_model.json");
    wifi                = new Wifi(registry);
    udp                 = new UdpServer("quadcopter_control", 5000);
    logger              = new Logger(udp);
    protocol            = new JsonDataProtocol(udp, registry);
    mixer               = new Mixer(front_left, front_right, rear_left, rear_right);
    motors_controller   = new MotorsController(MOTORS_CONTROLLER_PERIOD, registry, mixer);
    rate_controller     = new RateController(RATE_CONTROLLER_PERIOD, marg, mixer, registry);
    attitude_controller = new AttitudeController(ATTITUDE_CONTROLLER_PERIOD, registry, rate_controller, marg);
    height_controller   = new HeightController(HEIGHT_CONTROLLER_PERIOD, registry, marg, barometer, ultrasound, attitude_controller, rate_controller);
    position_controller = new PositionController(POSITION_CONTROLLER_PERIOD, registry);
    gps                 = new Gps(registry, PLATFORM_GPS_UART, PLATFORM_GPS_RX_PIN, PLATFORM_GPS_TX_PIN);
    battery             = new BatterySupervisor(BATTERY_SUPERVISOR_PERIOD, registry);
    camera              = new CameraController(CAMERA_SUPERVISOR_PERIOD, registry);
    telemetry           = new Telemetry(registry, 100, udp);
    radio_command       = new RadioCommand(registry);

    registry->internal_set<string>("control.mode", "off");
    registry->internal_set<string>("control.phase", "landed");

#endif

    rate_controller->calibrate_gyro();

#ifdef DATA_RECORDING
    data_recorder->start();
#else
    wifi->connect();
    radio_command->start();
    rate_controller->start();
    attitude_controller->start();
    height_controller->start();
    gps->start();
    motors_controller->start();
    telemetry->start();
#endif

    while (true)
    {
#ifndef DATA_RECORDING
        for (int i = 0; i < 20; i++)
        {
            uint8_t length;
            uint8_t packet[16];
            transceiver->receive_packet(packet, &length);
            Task::delay_ms(50);
        }
        udp->send_broadcast("{\"announcement\":\"kwadcopter\"}", 5001);
        transceiver->send_packet((uint8_t *)"hello world", 11);
#else
        Task::delay_ms(1000);
#endif
    }
}
