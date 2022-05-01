#pragma once

#include <utils/mixer.h>
#include <utils/pid.h>
#include <utils/filter.h>

#include <hal/marg.h>

#include <periph/timer.h>

#include <os/periodic_task.h>
#include <os/mutex.h>

#include <app/observers/euler_observer.h>
#include <app/observers/height_observer.h>

#include <data_model/data_ressources_registry.h>

class RateController : public PeriodicTask
{

private:

    DataRessourcesRegistry  * _registry;
    Marg                    * _marg;
    Pid                     * _roll_controller;
    Pid                     * _pitch_controller;
    Pid                     * _yaw_controller;
    Mixer                   * _mixer;
    Mutex                   * _mutex;
    BiQuadraticNotchFilter  * _roll_notch_filter;
    BiQuadraticNotchFilter  * _pitch_notch_filter;
    BiQuadraticNotchFilter  * _yaw_notch_filter;
    PTFilter                * _roll_low_pass_filter;
    PTFilter                * _pitch_low_pass_filter;
    PTFilter                * _yaw_low_pass_filter;
    PTFilter                * _accx_low_pass_filter;
    PTFilter                * _accy_low_pass_filter;
    PTFilter                * _accz_low_pass_filter;
    float                     _period;

    float _roll_target;
    float _pitch_target;
    float _yaw_target;

    float _roll_rate;
    float _pitch_rate;
    float _yaw_rate;

    float _throttle;

    bool _roll_enable;
    bool _pitch_enable;
    bool _yaw_enable;

    float _acc_x;
    float _acc_y;
    float _acc_z;

    float * _roll_rate_registry_handle;
    float * _pitch_rate_registry_handle;
    float * _yaw_rate_registry_handle;

    float * _roll_command_registry_handle;
    float * _pitch_command_registry_handle;
    float * _yaw_command_registry_handle;

    float * _gyro_x_registry_handle;
    float * _gyro_y_registry_handle;
    float * _gyro_z_registry_handle;

    float * _acc_x_registry_handle;
    float * _acc_y_registry_handle;
    float * _acc_z_registry_handle;

    void run(void);

public:

    RateController(float period, Marg * marg, Mixer * mixer, DataRessourcesRegistry * registry);

    void calibrate_gyro(void);

    void set_rate_targets(float roll, float pitch, float yaw);

    void set_enables(float roll, float pitch, float yaw);

    void set_throttle(float throttle);

    void set_roll_pid(float kp, float ki, float kd, float kff, float kt);
    void set_pitch_pid(float kp, float ki, float kd, float kff, float kt);
    void set_yaw_pid(float kp, float ki, float kd, float kff, float kt);

    void get_rates(float * roll, float * pitch, float * yaw);
    void get_acc(float * x, float * y, float * z);
    void get_commands(float * roll, float * pitch, float * yaw);
};