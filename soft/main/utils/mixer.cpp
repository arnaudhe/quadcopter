#include <utils/mixer.h>

Mixer::Mixer(Motor * front_left, Motor * front_right, Motor * rear_left, Motor * rear_right)
{
    _front_left  = front_left;
    _front_right = front_right;
    _rear_left   = rear_left;
    _rear_right  = rear_right;
}

void Mixer::update(float height_command, float roll_command, float pitch_command, float yaw_command)
{
    _front_left->set_speed ( height_command - roll_command - pitch_command - yaw_command);
    _front_right->set_speed( height_command + roll_command - pitch_command + yaw_command);
    _rear_right->set_speed ( height_command + roll_command + pitch_command - yaw_command);
    _rear_left->set_speed  ( height_command - roll_command + pitch_command + yaw_command);
}