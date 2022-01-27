#pragma once

#define ATTITUDE_PID_ROLL_POSITION_KP               (0.8f)
#define ATTITUDE_PID_ROLL_POSITION_KI               (0.1f)
#define ATTITUDE_PID_ROLL_POSITION_KD               (0.0f)
#define ATTITUDE_PID_ROLL_POSITION_FF               (0.0f)
#define ATTITUDE_PID_ROLL_POSITION_AW               (0.0f)

#define ATTITUDE_PID_PITCH_POSITION_KP              (0.8f)
#define ATTITUDE_PID_PITCH_POSITION_KI              (0.1f)
#define ATTITUDE_PID_PITCH_POSITION_KD              (0.0f)
#define ATTITUDE_PID_PITCH_POSITION_FF              (0.0f)
#define ATTITUDE_PID_PITCH_POSITION_AW              (0.0f)

#define ATTITUDE_PID_YAW_POSITION_KP                (0.12f)
#define ATTITUDE_PID_YAW_POSITION_KI                (0.12f)
#define ATTITUDE_PID_YAW_POSITION_KD                (0.06f)
#define ATTITUDE_PID_YAW_POSITION_FF                (0.00f)
#define ATTITUDE_PID_YAW_POSITION_AW                (0.01f)

#define ATTITUDE_MAX_RATE_COMMAND                   (5.0f)
#define ATTITUDE_MAX_SLEW_COMMAND                   (4.0f)