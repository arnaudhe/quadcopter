#include <app/workers/data_recorder.h>
#include <esp_attr.h>
#include <hal/log.h>
#include <os/task.h>

#define DATA_RECORDER_MOTORS_SPEED    0.50

DataRecorder::DataRecorder(Motor * front_left, Motor * front_right, Motor * rear_left, Motor * rear_right, Marg * marg)
    : PeriodicTask("data_recorder", Task::Priority::VERY_HIGH, 4 * 1024, 1, false)
{
    _marg        = marg;
    _front_left  = front_left;
    _front_right = front_right;
    _rear_left   = rear_left;
    _rear_right  = rear_right;

    LOG_INFO("Init done");
}

void IRAM_ATTR DataRecorder::run(void)
{
    float gx = 0.0f, gy = 0.0f, gz = 0.0f; /* gyro in drone frame (sensor data) */
    float ax = 0.0f, ay = 0.0f, az = 0.0f;
    static TickType_t start_tick = 0;
    TickType_t delta_tick;

    if (start_tick == 0)
    {
        printf("t;gx;gy;gz\n");
        start_tick = xTaskGetTickCount();
    }

    /* Compute the number of tick between now and startup */
    delta_tick = xTaskGetTickCount() - start_tick;

    /* Read the sensors */
    _marg->read_acc_gyro(&ax, &ay, &az, &gx, &gy, &gz);

    /* Display the measures */
    printf("%f;%f;%f;%f\n", (float)(xTaskGetTickCount() - start_tick) / 1000.0, gx, gy, gz);

    /* Each 5 ms, send motor command */
    if ((delta_tick % 10) == 0)
    {
        if (delta_tick < 3000)
        {
            _front_left->set_speed(0.0);
            _front_right->set_speed(0.0);
            _rear_left->set_speed(0.0);
            _rear_right->set_speed(0.0);
        }
        else if (delta_tick < 30000)
        {
            _front_left->set_speed(DATA_RECORDER_MOTORS_SPEED);
            _rear_left->set_speed(DATA_RECORDER_MOTORS_SPEED);
            _front_right->set_speed(DATA_RECORDER_MOTORS_SPEED);
            _rear_right->set_speed(DATA_RECORDER_MOTORS_SPEED);
        }
        else
        {
            _front_left->set_speed(0.0);
            _front_right->set_speed(0.0);
            _rear_left->set_speed(0.0);
            _rear_right->set_speed(0.0);
        }
    }
}
