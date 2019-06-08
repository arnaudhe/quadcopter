#pragma once

#define I2C_MASTER_NUM      I2C_NUM_1         /*!< I2C port number for master dev */


#define ATTITUDE_PID_YAW_KP                         (0)
#define ATTITUDE_PID_YAW_KI                         (0)
#define ATTITUDE_PID_YAW_KD                         (0)
#define ATTITUDE_PID_ROLL_POSITION_KP               (3.0f)
#define ATTITUDE_PID_ROLL_POSITION_KI               (0.3f)
#define ATTITUDE_PID_ROLL_POSITION_KD               (0.0f)
#define ATTITUDE_PID_ROLL_SPEED_KP                  (0.12f)
#define ATTITUDE_PID_ROLL_SPEED_KI                  (0.0f)
#define ATTITUDE_PID_ROLL_SPEED_KD                  (0.0f)
#define ATTITUDE_PID_ROLL_CONSIGN                   (0.0f)
#define ATTITUDE_PID_YAW_CONSIGN                    (0)


#define ATTITUDE_PID_HEIGHT_KP                      (0)
#define ATTITUDE_PID_HEIGHT_KI                      (0)
#define ATTITUDE_PID_HEIGHT_KD                      (0)
#define ATTITUDE_PID_PITCH_POSITION_KP              (3.0f)
#define ATTITUDE_PID_PITCH_POSITION_KI              (0.3f)
#define ATTITUDE_PID_PITCH_POSITION_KD              (0.0f)
#define ATTITUDE_PID_PITCH_SPEED_KP                 (0.12f)
#define ATTITUDE_PID_PITCH_SPEED_KI                 (0.0f)
#define ATTITUDE_PID_PITCH_SPEED_KD                 (0.0f)
#define ATTITUDE_PID_PITCH_CONSIGN                  (0.0f)
#define ATTITUDE_PID_HEIGHT_CONSIGN                 (0)

#define ATTITUDE_MOTOR_REAR_LEFT_PULSE_CHANNEL      (0)
#define ATTITUDE_MOTOR_REAR_LEFT_PULSE_PIN          (2)

#define ATTITUDE_MOTOR_FRONT_LEFT_PULSE_CHANNEL     (1)
#define ATTITUDE_MOTOR_FRONT_LEFT_PULSE_PIN         (21)

#define ATTITUDE_MOTOR_FRONT_RIGHT_PULSE_CHANNEL    (2)
#define ATTITUDE_MOTOR_FRONT_RIGHT_PULSE_PIN        (0)

#define ATTITUDE_MOTOR_REAR_RIGHT_PULSE_CHANNEL     (3)
#define ATTITUDE_MOTOR_REAR_RIGHT_PULSE_PIN         (33)
