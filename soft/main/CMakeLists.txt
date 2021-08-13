set(COMPONENT_SRCS
    drv/BMP180.cpp
    drv/BMP280.cpp
    drv/MPU6050.cpp
    drv/HMC5883L.cpp
    drv/HCSR04.cpp
    drv/DShotRMT.cpp
    drv/SI4432.cpp
    app/data_recorder/data_recorder.cpp
    app/data_recorder/telemetry.cpp
    app/controllers/position_controller.cpp
    app/controllers/attitude_controller.cpp
    app/controllers/height_controller.cpp
    app/controllers/motors_controller.cpp
    app/controllers/rate_controller.cpp
    app/observers/euler_observer.cpp
    app/observers/height_observer.cpp
    app/observers/position_observer.cpp
    app/workers/battery_supervisor.cpp
    app/workers/camera_controller.cpp
    data_model/ressource_enum.cpp
    data_model/data_ressource.cpp
    data_model/data_ressources_registry.cpp
    data_model/json_protocol.cpp
    hal/battery.cpp
    hal/udp_server.cpp
    hal/ultrasound.cpp
    hal/log.cpp
    hal/mdns.cpp
    hal/barometer.cpp
    hal/imu.cpp
    hal/led.cpp
    hal/motor.cpp
    hal/radio_command.cpp
    hal/wifi.cpp
    hal/wifi_config.c 
    hal/marg.cpp
    hal/gps.cpp
    os/mutex.cpp
    os/periodic_task.cpp
    os/queue.cpp
    os/semaphore.cpp
    os/task.cpp
    utils/pid.cpp
    utils/madgwick.cpp
    utils/mixer.cpp
    utils/nmea_parser.cpp
    utils/kalman.cpp
    utils/vector.cpp
    utils/csv_reader.cpp
    utils/matrix.cpp
    utils/quaternion.cpp
    utils/filter.cpp
    utils/string_utils.cpp
    periph/adc.cpp 
    periph/i2c_master.cpp
    periph/pulse.cpp
    periph/timer.cpp
    periph/gpio.cpp
    periph/spi.cpp
    periph/uart.cpp
    main.cpp)

set(COMPONENT_ADD_INCLUDEDIRS ".")

register_component()

add_custom_command(OUTPUT ${COMPONENT_DIR}/data_model.json
     COMMAND mkdir -p ${COMPONENT_DIR}/storage_img && cp ${COMPONENT_DIR}/../../data_model/data_model.json ${COMPONENT_DIR}/storage_img
     DEPENDS ${COMPONENT_DIR}/../../data_model/data_model.json
     VERBATIM)

add_custom_target(generate_filesystem DEPENDS ${COMPONENT_DIR}/data_model.json)

spiffs_create_partition_image(storage ${COMPONENT_DIR}/storage_img FLASH_IN_PROJECT DEPENDS generate_filesystem)
