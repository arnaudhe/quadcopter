#pragma once

#define RATE_GYRO_FILTER_FREQ_SLOPE   (250.0f)
#define RATE_GYRO_FILTER_FREQ_CUTOFF  (12.0f)

#define RATE_PID_ROLL_KP        (0.06f)
#define RATE_PID_ROLL_KI        (0.03f)
#define RATE_PID_ROLL_KD        (0.003f)
#define RATE_PID_ROLL_FF        (0.0f)
#define RATE_PID_ROLL_AW        (0.5f)

#define RATE_PID_PITCH_KP       (0.06f)
#define RATE_PID_PITCH_KI       (0.03f)
#define RATE_PID_PITCH_KD       (0.005f)
#define RATE_PID_PITCH_FF       (0.0f)
#define RATE_PID_PITCH_AW       (0.5f)

#define RATE_PID_YAW_KP         (0.5f)
#define RATE_PID_YAW_KI         (0.08f)
#define RATE_PID_YAW_KD         (0.0f)
#define RATE_PID_YAW_FF         (0.08f)
#define RATE_PID_YAW_AW         (0.200f)

#define RATE_ACC_X_CALIB        (0.016308f)
#define RATE_ACC_Y_CALIB        (0.018719f)
#define RATE_ACC_Z_CALIB        (-0.080354f)

#define RATE_GYRO_X_CALIB       (-0.037643f)
#define RATE_GYRO_Y_CALIB       (-0.009281f)
#define RATE_GYRO_Z_CALIB       (-0.006195f)
