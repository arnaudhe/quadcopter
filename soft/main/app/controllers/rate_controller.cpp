#include <app/controllers/rate_controller.h>
#include <app/controllers/rate_controller_conf.h>
#include <hal/log.h>
#include <os/task.h>
#include <esp_attr.h>

#define GYRO_CALIBRATION_LOOPS          (1500)
#define RPM_FILTER_THURST_THRESHOLD     (0.1f)

RateController::RateController(float period, Marg * marg, Mixer * mixer, DataRessourcesRegistry * registry):
    PeriodicTask("rate_ctlr", Task::Priority::VERY_HIGH, (int)(period * 1000), false)
{
    _period   = period;
    _marg     = marg;
    _mixer    = mixer;
    _registry = registry;
    _mutex    = new Mutex();

    _roll_controller   = new Pid(period, RATE_PID_ROLL_KP, RATE_PID_ROLL_KI, RATE_PID_ROLL_KD, RATE_PID_ROLL_FF, RATE_PID_ROLL_AW);
    _pitch_controller  = new Pid(period, RATE_PID_PITCH_KP, RATE_PID_PITCH_KI, RATE_PID_PITCH_KD, RATE_PID_PITCH_FF, RATE_PID_PITCH_AW);
    _yaw_controller    = new Pid(period, RATE_PID_YAW_KP, RATE_PID_YAW_KI, RATE_PID_YAW_KD, RATE_PID_YAW_FF, RATE_PID_YAW_AW);

    _registry->internal_set<float>("control.attitude.roll.speed.kp", RATE_PID_ROLL_KP);
    _registry->internal_set<float>("control.attitude.roll.speed.ki", RATE_PID_ROLL_KI);
    _registry->internal_set<float>("control.attitude.roll.speed.kd", RATE_PID_ROLL_KD);
    _registry->internal_set<float>("control.attitude.roll.speed.kff", RATE_PID_ROLL_FF);
    _registry->internal_set<float>("control.attitude.roll.speed.kt", RATE_PID_ROLL_AW);

    _registry->internal_set<float>("control.attitude.pitch.speed.kp", RATE_PID_PITCH_KP);
    _registry->internal_set<float>("control.attitude.pitch.speed.ki", RATE_PID_PITCH_KI);
    _registry->internal_set<float>("control.attitude.pitch.speed.kd", RATE_PID_PITCH_KD);
    _registry->internal_set<float>("control.attitude.pitch.speed.kff", RATE_PID_PITCH_FF);
    _registry->internal_set<float>("control.attitude.pitch.speed.kt", RATE_PID_PITCH_AW);

    _registry->internal_set<float>("control.attitude.yaw.speed.kp", RATE_PID_YAW_KP);
    _registry->internal_set<float>("control.attitude.yaw.speed.ki", RATE_PID_YAW_KI);
    _registry->internal_set<float>("control.attitude.yaw.speed.kd", RATE_PID_YAW_KD);
    _registry->internal_set<float>("control.attitude.yaw.speed.kff", RATE_PID_YAW_FF);
    _registry->internal_set<float>("control.attitude.yaw.speed.kt", RATE_PID_YAW_AW);

    _roll_rate_registry_handle  = _registry->get_handle<float>("control.attitude.roll.speed.current");
    _pitch_rate_registry_handle = _registry->get_handle<float>("control.attitude.pitch.speed.current");
    _yaw_rate_registry_handle   = _registry->get_handle<float>("control.attitude.yaw.speed.current");

    _roll_command_registry_handle  = _registry->get_handle<float>("control.attitude.roll.speed.command");
    _pitch_command_registry_handle = _registry->get_handle<float>("control.attitude.pitch.speed.command");
    _yaw_command_registry_handle   = _registry->get_handle<float>("control.attitude.yaw.speed.command");

    _gyro_x_registry_handle = _registry->get_handle<float>("sensors.gyroscope.x");
    _gyro_y_registry_handle = _registry->get_handle<float>("sensors.gyroscope.y");
    _gyro_z_registry_handle = _registry->get_handle<float>("sensors.gyroscope.z");

    _acc_x_registry_handle = _registry->get_handle<float>("sensors.accelerometer.x");
    _acc_y_registry_handle = _registry->get_handle<float>("sensors.accelerometer.y");
    _acc_z_registry_handle = _registry->get_handle<float>("sensors.accelerometer.z");

    _roll_notch_filter  = new BiQuadraticNotchFilter(period, 90.0, 70.0);
    _pitch_notch_filter = new BiQuadraticNotchFilter(period, 90.0, 70.0);
    _yaw_notch_filter   = new BiQuadraticNotchFilter(period, 90.0, 70.0);
    _roll_low_pass_filter  = new PT3Filter(period, 20.0);
    _pitch_low_pass_filter = new PT3Filter(period, 20.0);
    _yaw_low_pass_filter   = new PT3Filter(period, 20.0);
    _accx_low_pass_filter  = new PT3Filter(period, 10.0);
    _accy_low_pass_filter  = new PT3Filter(period, 10.0);
    _accz_low_pass_filter  = new PT3Filter(period, 10.0);

    _roll_notch_filter->init();
    _pitch_notch_filter->init();
    _yaw_notch_filter->init();
    _roll_low_pass_filter->init();
    _pitch_low_pass_filter->init();
    _yaw_low_pass_filter->init();
    _accx_low_pass_filter->init();
    _accy_low_pass_filter->init();
    _accz_low_pass_filter->init();

    _roll_target  = 0.0;
    _pitch_target = 0.0;
    _yaw_target   = 0.0;

    _roll_enable  = false;
    _pitch_enable = false;
    _yaw_enable   = false;

    LOG_INFO("Init done");
}

void RateController::calibrate_gyro(void)
{
    float ax, ay, az;
    float gx, gy, gz;

    float ax_calib, ay_calib, az_calib;
    float roll_calib, pitch_calib, yaw_calib;

    roll_calib  = 0.0;
    pitch_calib = 0.0;
    yaw_calib   = 0.0;

    ax_calib = 0.0;
    ay_calib = 0.0;
    az_calib = 0.0;

    for (int i = 0; i < GYRO_CALIBRATION_LOOPS; i++)
    {
        _marg->read_acc_gyro(&ax, &ay, &az, &gx, &gy, &gz);
        ax_calib     += ax;
        ay_calib     += ay;
        az_calib     += az;
        roll_calib  += gx;
        pitch_calib += gy;
        yaw_calib   += gz;

        Task::delay_ms(1);
    }

    ax_calib /= (float)GYRO_CALIBRATION_LOOPS;
    ay_calib /= (float)GYRO_CALIBRATION_LOOPS;
    az_calib /= (float)GYRO_CALIBRATION_LOOPS;

    roll_calib  /= (float)GYRO_CALIBRATION_LOOPS;
    pitch_calib /= (float)GYRO_CALIBRATION_LOOPS;
    yaw_calib   /= (float)GYRO_CALIBRATION_LOOPS;

    LOG_INFO("Accelero calibration done : %f %f %f", ax_calib, ay_calib, az_calib);
    LOG_INFO("Gyro calibration done : %f %f %f", roll_calib, pitch_calib, yaw_calib);
}

void IRAM_ATTR RateController::run(void)
{
    float gx, gy, gz;   // gyro in drone frame (sensor data)
    float ax, ay, az;   // accelero in drone frame (sensor data)
    float height_command;
    bool  enable;
    float roll_command, pitch_command, yaw_command;

    /* Read the sensors */
    _marg->read_acc_gyro(&ax, &ay, &az, &gx, &gy, &gz);

    _registry->internal_set_fast<float>(_gyro_x_registry_handle, gx);
    _registry->internal_set_fast<float>(_gyro_y_registry_handle, gy);
    _registry->internal_set_fast<float>(_gyro_z_registry_handle, gz);

    _registry->internal_set_fast<float>(_acc_x_registry_handle, ax);
    _registry->internal_set_fast<float>(_acc_y_registry_handle, ay);
    _registry->internal_set_fast<float>(_acc_z_registry_handle, az);

    /* Remove calibration offset */
    gx -= RATE_GYRO_X_CALIB;
    gy -= RATE_GYRO_Y_CALIB;
    gz -= RATE_GYRO_Z_CALIB;

    ax -= RATE_ACC_X_CALIB;
    ay -= RATE_ACC_Y_CALIB;
    az -= RATE_ACC_Z_CALIB;

    /* Apply filters */
    gx = _roll_low_pass_filter->apply(_roll_notch_filter->apply(gx));
    gy = _pitch_low_pass_filter->apply(_pitch_notch_filter->apply(gy));
    gz = _yaw_low_pass_filter->apply(_yaw_notch_filter->apply(gz));

    ax = _accx_low_pass_filter->apply(ax);
    ay = _accy_low_pass_filter->apply(ay);
    az = _accz_low_pass_filter->apply(az);

    /* Update filtered rates in registry */
    _registry->internal_set_fast<float>(_roll_rate_registry_handle, gx);
    _registry->internal_set_fast<float>(_pitch_rate_registry_handle, gy);
    _registry->internal_set_fast<float>(_yaw_rate_registry_handle, gz);

    _mutex->lock();

    /* Update accel attributes */
    _acc_x = ax;
    _acc_y = ay;
    _acc_z = az;

    /* Update rate attributes */
    _roll_rate  = gx;
    _pitch_rate = gy;
    _yaw_rate   = gz;

    /* Run PIDs */
    if (_roll_enable)
    {
        roll_command = _roll_controller->update(_roll_rate);
    }
    else
    {
        _roll_controller->reset();
        roll_command = 0.0f;
    }

    if (_pitch_enable)
    {
        pitch_command = _pitch_controller->update(_pitch_rate);
    }
    else
    {
        _pitch_controller->reset();
        pitch_command = 0.0f;
    }

    if (_yaw_enable)
    {
        yaw_command = _yaw_controller->update(_yaw_rate);
    }
    else
    {
        _yaw_controller->reset();
        yaw_command = 0.0f;
    }

    height_command = _throttle;

    /* Store enable status in non concurrent-accessed local variable */
    enable = ((_roll_enable || _pitch_enable || _yaw_enable) && (height_command > 0.01));

    _mutex->unlock();

    _registry->internal_set_fast<float>(_roll_command_registry_handle, roll_command);
    _registry->internal_set_fast<float>(_pitch_command_registry_handle, pitch_command);
    _registry->internal_set_fast<float>(_yaw_command_registry_handle, yaw_command);

    /* Update motors outputs */
    if (enable)
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
    // float center_frequency = RATE_GYRO_FILTER_FREQ_SLOPE * throttle + RATE_GYRO_FILTER_FREQ_CUTOFF;
    // float cutoff_frequency = center_frequency - 10.0;

    _mutex->lock();
    _throttle = throttle;
    // _roll_notch_filter->update(center_frequency, cutoff_frequency);
    // _pitch_notch_filter->update(center_frequency, cutoff_frequency);
    // _yaw_notch_filter->update(center_frequency, cutoff_frequency);
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

void IRAM_ATTR RateController::get_acc(float * roll, float * pitch, float * yaw)
{
    _mutex->lock();
    *roll  = _acc_x;
    *pitch = _acc_y;
    *yaw   = _acc_z;
    _mutex->unlock();
}

void IRAM_ATTR RateController::set_roll_pid(float kp, float ki, float kd, float kff, float kt)
{
    _mutex->lock();
    _roll_controller->set_kp(kp);
    _roll_controller->set_ki(ki);
    _roll_controller->set_kd(kd);
    _roll_controller->set_kff(kff);
    _roll_controller->set_kt(kt);
    _mutex->unlock();
}

void IRAM_ATTR RateController::set_pitch_pid(float kp, float ki, float kd, float kff, float kt)
{
    _mutex->lock();
    _pitch_controller->set_kp(kp);
    _pitch_controller->set_ki(ki);
    _pitch_controller->set_kd(kd);
    _pitch_controller->set_kff(kff);
    _pitch_controller->set_kt(kt);
    _mutex->unlock();
}

void IRAM_ATTR RateController::set_yaw_pid(float kp, float ki, float kd, float kff, float kt)
{
    _mutex->lock();
    _yaw_controller->set_kp(kp);
    _yaw_controller->set_ki(ki);
    _yaw_controller->set_kd(kd);
    _yaw_controller->set_kff(kff);
    _yaw_controller->set_kt(kt);
    _mutex->unlock();
}
