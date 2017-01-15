load('quad_constants.mat')

speed = 4400
motors_speed = [speed + 200, speed + 200, speed - 200, speed - 200];
dt = 0.01;

% Initial conditions
x         = [0; 0; 10];
x_dot     = [0; 0; 0];
theta     = [0; 0; 0];
theta_dot = [0; 0; 0];

for t = 0:dt:10
    [x, x_dot, theta, theta_dot] = plant_step(quad_constants, theta, theta_dot, x, x_dot, motors_speed, dt);
    x
end