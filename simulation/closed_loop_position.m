clear all
close all

addpath('quaternion_library');
load('quad_constants.mat')

dt = 0.01; 
max_time = 10;

lin_pos = [0; 0; 10];
lin_vel = [0; 0; 0];
ang_pos = [0; 0; 0];
ang_vel = [0; 0; 0];

position_consign = [10; 0; 10];
attitude_initial = [ang_pos; lin_pos(3)]; % Initial state

motors_speed_initial = [500; 500; 500; 500];

result.t           = 0;
result.dt          = dt;
result.lin_pos     = lin_pos;
result.lin_vel     = lin_vel;
result.ang_pos     = ang_pos;
result.ang_pos_hat = ang_pos;
result.ang_vel     = ang_vel;
result.input       = motors_speed_initial;

ahrs  = ahrs('sample_period', dt, 'beta', 0.1);
controller = attitude_controller('sample_period', dt, ...
                                 'consign', attitude_initial, ...
                                 'initial', attitude_initial, ...
                                 'quad_constants', quad_constants);

kp = [0.02; 0.02];
kd = [0.05; 0.05];
ki = [0; 0];

for t = dt:dt:(max_time-dt)
    
    % Measure MARG sensor values
    [acc, gyr, mag] = marg(ang_pos, ang_vel);
    
    % Build Euler angle with Madgwick filter
    ahrs.update(gyr', acc', mag');
    ang_pos_hat = quatern2euler(quaternConj(ahrs.quaternion))';
    
    error_pos = position_consign - lin_pos;
    command = kp .* [error_pos(1, 1); error_pos(2, 1)] - ...
              kd .* [lin_vel(1, 1); lin_vel(2, 1)];
          
    controller.consign = [command(2) * cos(ang_pos_hat(3)) + command(1) * sin(ang_pos_hat(3));
                          command(2) * sin(ang_pos_hat(3)) + command(1) * cos(ang_pos_hat(3));
                          ang_pos_hat(3);
                          position_consign(3)];
    
    % Compute attitude controller
    attitude = [ang_pos_hat; lin_pos(3)];
    controller.update(attitude);
    
    % Apply motor model, and then quad model
    [motors_speed] = motors_model(controller.motors_throttle, quad_constants);
    [ang_pos, ang_vel, lin_pos, lin_vel] = quad_model(quad_constants, ang_pos, ang_vel, lin_pos, lin_vel, motors_speed, dt);
    
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

visualize_2D(result)
%visualize_3D(result)