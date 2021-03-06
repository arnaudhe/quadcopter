set(COMPONENT_SRCS 
    drv/BMP180.cpp 
    drv/MPU6050.cpp 
    drv/HMC5883L.cpp 
    drv/HCSR04.cpp 
    drv/DShotRMT.cpp 
    app/controllers/position_controller.cpp 
    app/controllers/attitude_controller.cpp 
    app/observers/euler_observer.cpp 
    app/observers/height_observer.cpp 
    app/observers/position_observer.cpp 
    data_model/ressource_enum.cpp
    data_model/data_ressource.cpp
    data_model/data_ressources_registry.cpp
    data_model/json_protocol.cpp
    hal/udp_server.cpp 
    hal/ultrasound.cpp 
    hal/mdns.cpp 
    hal/barometer.cpp 
    hal/imu.cpp 
    hal/led.cpp
    hal/motor.cpp 
    hal/wifi.cpp 
    hal/wifi_config.c 
    hal/marg.cpp 
    os/mutex.cpp
    os/periodic_task.cpp
    os/queue.cpp
    os/semaphore.cpp
    os/task.cpp
    utils/pid.cpp 
    utils/madgwick.cpp 
    utils/mixer.cpp 
    utils/kalman.cpp 
    utils/vector.cpp 
    utils/csv_reader.cpp 
    utils/matrix.cpp 
    utils/quaternion.cpp 
    utils/controller.cpp 
    periph/i2c_master.cpp 
    periph/pulse.cpp 
    periph/timer.cpp 
    periph/gpio.cpp 
    periph/spi.cpp 
    periph/uart.cpp 
    main.cpp)

set(COMPONENT_ADD_INCLUDEDIRS ".")

register_component()
