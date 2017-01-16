clear all
close all

load('quad_constants.mat')

speed = sqrt(quad_constants.mass * quad_constants.g/(4*quad_constants.ct));
motors_speed = [speed, speed, speed, speed];
dt = 0.01;

% Initial conditions
x         = [0; 0; 10];
x_dot     = [0; 0; 0];
theta     = [0; 0; 0];
theta_dot = [0; 0; 0];

result = [0, theta', theta_dot', x', x_dot'];

for t = 0:dt:10
    [theta, theta_dot, x, x_do] = plant_step(quad_constants, theta, theta_dot, x, x_dot, motors_speed, dt);
    result = [result; [t, theta', theta_dot', x', x_dot']];
end

visualize(result)