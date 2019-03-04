#include <app/attitude_controller.h>
#include <app/attitude_controller_conf.h>

AttitudeController::AttitudeController(float period)
{
    _period = period;

    _attitude = new Attitude(period);

    _height_controller = new Controller(period, 
                                        new Pid(period, ATTITUDE_PID_HEIGHT_KP, ATTITUDE_PID_HEIGHT_KP, ATTITUDE_PID_HEIGHT_KP),
                                        new Pid(period, ATTITUDE_PID_HEIGHT_KP, ATTITUDE_PID_HEIGHT_KP, ATTITUDE_PID_HEIGHT_KP));

    _roll_controller   = new Controller(period, 
                                        new Pid(period, ATTITUDE_PID_ROLL_KP, ATTITUDE_PID_ROLL_KP, ATTITUDE_PID_ROLL_KP),
                                        new Pid(period, ATTITUDE_PID_ROLL_KP, ATTITUDE_PID_ROLL_KP, ATTITUDE_PID_ROLL_KP));

    _pitch_controller  = new Controller(period, 
                                        new Pid(period, ATTITUDE_PID_PITCH_KP, ATTITUDE_PID_PITCH_KP, ATTITUDE_PID_PITCH_KP),
                                        new Pid(period, ATTITUDE_PID_PITCH_KP, ATTITUDE_PID_PITCH_KP, ATTITUDE_PID_PITCH_KP));

    _yaw_controller    = new Controller(period, 
                                        new Pid(period, ATTITUDE_PID_YAW_KP, ATTITUDE_PID_YAW_KP, ATTITUDE_PID_YAW_KP),
                                        new Pid(period, ATTITUDE_PID_YAW_KP, ATTITUDE_PID_YAW_KP, ATTITUDE_PID_YAW_KP));

    _mixer = new Mixer(new Motor(ATTITUDE_MOTOR_FRONT_LEFT_PULSE_CHANNEL,  ATTITUDE_MOTOR_FRONT_LEFT_PULSE_PIN),
                       new Motor(ATTITUDE_MOTOR_FRONT_RIGHT_PULSE_CHANNEL, ATTITUDE_MOTOR_FRONT_RIGHT_PULSE_PIN),
                       new Motor(ATTITUDE_MOTOR_REAR_LEFT_PULSE_CHANNEL,   ATTITUDE_MOTOR_REAR_LEFT_PULSE_PIN),
                       new Motor(ATTITUDE_MOTOR_REAR_RIGHT_PULSE_CHANNEL,  ATTITUDE_MOTOR_REAR_RIGHT_PULSE_PIN));
}

void AttitudeController::update(void)
{
    /* Estimate the attitude */
    _attitude->update();

    /* Compute the controls */
    _height_controller->update (_attitude->height(), _attitude->vertical_speed());
    _roll_controller->update   (_attitude->roll(),   _attitude->roll_speed());
    _pitch_controller->update  (_attitude->pitch(),  _attitude->pitch_speed());
    _yaw_controller->update    (_attitude->yaw(),    _attitude->yaw_speed());

    /* Apply the controls to the motors */
    _mixer->update(_height_controller->command(), _roll_controller->command(), _pitch_controller->command(), _yaw_controller->command());
}

void AttitudeController::set_height_target(Controller::Mode mode, float target)
{
    _height_controller->update_target(mode, target);
}

void AttitudeController::set_roll_target(Controller::Mode mode, float target)
{
    _roll_controller->update_target(mode, target);
}

void AttitudeController::set_pitch_target(Controller::Mode mode, float target)
{
    _pitch_controller->update_target(mode, target);
}

void AttitudeController::set_yaw_target(Controller::Mode mode, float target)
{
    _yaw_controller->update_target(mode, target);
}
