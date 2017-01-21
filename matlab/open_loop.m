clear all
close all

load('quad_constants.mat')

speed = sqrt(quad_constants.mass * quad_constants.g/(4*quad_constants.ct));
motors_speed = [speed; speed; speed; speed];
dt = 0.01;

x         = [0; 0; 10];
x_dot     = [0; 0; 0];
theta     = [0; 0; 0];
theta_dot = [0; 0; 0];

result.t      = 0;
result.dt     = dt;
result.x      = x;
result.vel    = x_dot;
result.theta  = theta;
result.angvel = theta_dot;
result.input  = motors_speed;

for t = dt:dt:20
    [theta, theta_dot, x, x_dot] = plant_step_2(quad_constants, theta, theta_dot, x, x_dot, motors_speed, dt);
    result.t      = [result.t, t];
    result.dt     = [result.dt, dt];
    result.x      = [result.x, x];
    result.vel    = [result.vel, x_dot];
    result.theta  = [result.theta, theta];
    result.angvel = [result.angvel, theta_dot];
    result.input  = [result.input, motors_speed];
end

visualize_3D(result)