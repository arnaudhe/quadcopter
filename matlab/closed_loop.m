clear all
close all

addpath('quaternion_library');
load('quad_constants.mat')

dt = 0.01; 

lin_pos = [0; 0; 10];
lin_vel = [0; 0; 0];
ang_pos = [0; 0; 0];
ang_vel = [0; 0; 0];

motors_speed = [500; 500; 500; 500];

result.t       = 0;
result.dt      = dt;
result.lin_pos = lin_pos;
result.lin_vel = lin_vel;
result.ang_pos = ang_pos;
result.ang_pos_hat = ang_pos;
result.ang_vel = ang_vel;
result.input   = motors_speed;

attitude_consign = [pi/4; pi/4; pi/2; 15];

kp = [2; 2; 4; 5];
kd = [1; 1; 6; 5];
ki = [1; 1; 0; 1];

altitude_command_offset = (sqrt((quad_constants.mass*quad_constants.g)/(4*quad_constants.ct))-quad_constants.b)/quad_constants.cr;
offset = [0; 0; 0; altitude_command_offset];
attitude = [ang_pos; lin_pos(3)];
integrates = [0; 0; 0; 0];

AHRS = MadgwickAHRS('SamplePeriod', dt, 'Beta', 0.1);

for t = dt:dt:10-dt
    
    % Measure MARG sensor values
    [acc, gyr, mag]  = marg(ang_pos, ang_vel);
    
    % Rebuild Euler angle with Madgwick filter
    AHRS.Update(gyr', acc', mag');
    ang_pos_hat = quatern2euler(quaternConj(AHRS.Quaternion))';
    
    % Estimate attitude error
    previous       = attitude;
    attitude       = [ang_pos_hat; lin_pos(3)];
    attitude_error = attitude_consign - attitude;
    integrates     = integrates + attitude_error * dt;
    derivates      = (previous - attitude)/dt;
    
    % Compute attitude regulator command
    [attitude_commands] = kp .* attitude_error + ki .* integrates + kd .* derivates + offset;
    
    % Compute motors thottles
    [motors_throttle] = mixer(attitude_commands);
    
    % Apply motor model to convert thottle to speed
    [motors_speed] = motors_model(motors_throttle, quad_constants);

    % Apply to the plant
    [ang_pos, ang_vel, lin_pos, lin_vel] = plant_step_2(quad_constants, ang_pos, ang_vel, lin_pos, lin_vel, motors_speed, dt);
    
    % Store outputs
    result.t            = [result.t, t];
    result.dt           = [result.dt, dt];
    result.lin_pos      = [result.lin_pos, lin_pos];
    result.lin_vel      = [result.lin_vel, lin_vel];
    result.ang_pos      = [result.ang_pos, ang_pos];
    result.ang_pos_hat  = [result.ang_pos_hat ang_pos_hat];
    result.ang_vel      = [result.ang_vel, ang_vel];
    result.input        = [result.input, motors_speed];
    
end

%visualize_2D(result)
%visualize_3D(result)