#include <app/controllers/rate_controller.h>
#include <app/controllers/rate_controller_conf.h>
#include <hal/log.h>
#include <os/task.h>
#include <esp_attr.h>

#define GYRO_CALIBRATION_LOOPS          (100)
#define RPM_FILTER_THURST_THRESHOLD     (0.1f)

RateController::RateController(float period, Marg * marg, Mixer * mixer):
    PeriodicTask("rate_ctlr", Task::Priority::VERY_HIGH, (int)(period * 1000), false)
{
    float gx, gy, gz;

    _period = period;
    _marg   = marg;
    _mixer  = mixer;
    _mutex  = new Mutex();

    _roll_controller   = new Pid(period, RATE_PID_ROLL_KP, RATE_PID_ROLL_KI, RATE_PID_ROLL_KD, RATE_PID_ROLL_FF);
    _pitch_controller  = new Pid(period, RATE_PID_PITCH_KP, RATE_PID_PITCH_KI, RATE_PID_PITCH_KD, RATE_PID_PITCH_FF);
    _yaw_controller    = new Pid(period, RATE_PID_YAW_KP, RATE_PID_YAW_KI, RATE_PID_YAW_KD, RATE_PID_YAW_FF);

    _roll_filter  = new BiQuadraticNotchFilter(period, 100.0, 80.0);
    _pitch_filter = new BiQuadraticNotchFilter(period, 100.0, 80.0);
    _yaw_filter   = new BiQuadraticNotchFilter(period, 100.0, 80.0);

    _roll_filter->init();
    _pitch_filter->init();
    _yaw_filter->init();

    _roll_target  = 0.0;
    _pitch_target = 0.0;
    _yaw_target   = 0.0;

    _roll_calib   = 0.0;
    _pitch_calib  = 0.0;
    _yaw_calib    = 0.0;

    for (int i = 0; i < GYRO_CALIBRATION_LOOPS; i++)
    {
        _marg->read_gyro(&gx, &gy, &gz);
        _roll_calib  += gx;
        _pitch_calib += gy;
        _yaw_calib   += gz;
    }

    _roll_calib  /= (float)GYRO_CALIBRATION_LOOPS;
    _pitch_calib /= (float)GYRO_CALIBRATION_LOOPS;
    _yaw_calib   /= (float)GYRO_CALIBRATION_LOOPS;

    LOG_INFO("Gyro calibration done : %f %f %f", _roll_calib, _pitch_calib, _yaw_calib);

    _roll_enable  = false;
    _pitch_enable = false;
    _yaw_enable   = false;

    LOG_INFO("Init done");
}

void IRAM_ATTR RateController::run(void)
{
    float gx, gy, gz;   // gyro in drone frame (sensor data)
    float roll_command;
    float pitch_command;
    float yaw_command;
    float height_command;

    /* Read the sensors */
    _marg->read_gyro(&gx, &gy, &gz);

    /* Remove calibration offset */
    gx -= _roll_calib;
    gy -= _pitch_calib;
    gx -= _yaw_calib;

    _mutex->lock();

    /* Apply filter */
    if (_throttle >= RPM_FILTER_THURST_THRESHOLD)
    {
        gx = _roll_filter->apply(gx);
        gy = _pitch_filter->apply(gy);
        gz = _yaw_filter->apply(gz);
    }

    /* Update rate attributes */
    _roll_rate  = gx;
    _pitch_rate = gy;
    _yaw_rate   = gz;

    /* Run PIDs */
    roll_command   = _roll_enable  ? _roll_controller->update(_roll_rate) : 0.0f;
    pitch_command  = _pitch_enable ? _pitch_controller->update(_pitch_rate) : 0.0f;
    yaw_command    = _yaw_enable   ? _yaw_controller->update(_yaw_rate) : 0.0f;
    height_command = _throttle;

    _mutex->unlock();

    /* Update motors outputs */
    if ((_roll_enable || _pitch_enable || _yaw_enable) && (height_command > 0.01))
    {
        _mixer->update(height_command, roll_command, pitch_command, yaw_command);
    }
}

void IRAM_ATTR RateController::set_rate_targets(float roll, float pitch, float yaw)
{
    _mutex->lock();
    _roll_controller->set_setpoint(roll);
    _pitch_controller->set_setpoint(pitch);
    _yaw_controller->set_setpoint(yaw);
    _mutex->unlock();
}

void IRAM_ATTR RateController::set_enables(float roll, float pitch, float yaw)
{
    _mutex->lock();
    _roll_enable  = roll;
    _pitch_enable = pitch;
    _yaw_enable   = yaw;
    _mutex->unlock();
}

void IRAM_ATTR RateController::set_throttle(float throttle)
{
    float center_frequency = RATE_GYRO_FILTER_FREQ_SLOPE * throttle + RATE_GYRO_FILTER_FREQ_CUTOFF;
    float cutoff_frequency = center_frequency - 10.0;

    _mutex->lock();
    _throttle = throttle;
    _roll_filter->update(center_frequency, cutoff_frequency);
    _roll_filter->update(center_frequency, cutoff_frequency);
    _roll_filter->update(center_frequency, cutoff_frequency);
    _mutex->unlock();
}

void IRAM_ATTR RateController::get_rates(float * roll, float * pitch, float * yaw)
{
    _mutex->lock();
    *roll  = _roll_rate;
    *pitch = _pitch_rate;
    *yaw   = _yaw_rate;
    _mutex->unlock();
}

void IRAM_ATTR RateController::set_roll_pid(float kp, float ki, float kd, float kff)
{
    _mutex->lock();
    _roll_controller->set_kp(kp);
    _roll_controller->set_ki(ki);
    _roll_controller->set_kd(kd);
    _roll_controller->set_kff(kff);
    _mutex->unlock();
}

void IRAM_ATTR RateController::set_pitch_pid(float kp, float ki, float kd, float kff)
{
    _mutex->lock();
    _pitch_controller->set_kp(kp);
    _pitch_controller->set_ki(ki);
    _pitch_controller->set_kd(kd);
    _pitch_controller->set_kff(kff);
    _mutex->unlock();
}

void IRAM_ATTR RateController::set_yaw_pid(float kp, float ki, float kd, float kff)
{
    _mutex->lock();
    _yaw_controller->set_kp(kp);
    _yaw_controller->set_ki(ki);
    _yaw_controller->set_kd(kd);
    _yaw_controller->set_kff(kff);
    _mutex->unlock();
}
