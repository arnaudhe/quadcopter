clear all
close all

addpath('quaternion_library');
load('quad_constants.mat')

dt = 0.01; 
max_time = 10;

speed = sqrt(quad_constants.mass * quad_constants.g/(4*quad_constants.ct));
motors_speed = [speed + 20; speed + 5; speed + 50; speed - 10];

lin_pos = [0; 0; 10];
lin_vel = [0; 0; 0];
ang_pos = [0; 0; 0];
ang_vel = [0; 0; 0];

result.t           = 0;
result.dt          = dt;
result.lin_pos     = lin_pos;
result.lin_vel     = lin_vel;
result.ang_pos     = ang_pos;
result.ang_pos_hat = ang_pos;
result.ang_vel     = ang_vel;
result.input       = motors_speed;

ahrs = ahrs('sample_period', dt, 'beta', 0.1);

for t = dt:dt:(max_time-dt)
    
    % Measure MARG sensor values
    [acc, gyr, mag] = marg(ang_pos, ang_vel);
    
    % Build Euler angle with Madgwick filter
    ahrs.update(gyr', acc', mag');
    ang_pos_hat = quatern2euler(quaternConj(ahrs.quaternion))';
    
    % Apply quadcopter model
    [ang_pos, ang_vel, lin_pos, lin_vel] = quad_model(quad_constants, ang_pos, ang_vel, lin_pos, lin_vel, motors_speed, dt);
    
    %Store results
    result.t            = [result.t, t];
    result.dt           = [result.dt, dt];
    result.lin_pos      = [result.lin_pos, lin_pos];
    result.lin_vel      = [result.lin_vel, lin_vel];
    result.ang_pos      = [result.ang_pos, ang_pos];
    result.ang_pos_hat  = [result.ang_pos_hat ang_pos_hat];
    result.ang_vel      = [result.ang_vel, ang_vel];
    result.input        = [result.input, motors_speed];
end

visualize_2D(result)