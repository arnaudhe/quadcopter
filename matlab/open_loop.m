clear all
close all

load('quad_constants.mat')

speed = sqrt(quad_constants.mass * quad_constants.g/(4*quad_constants.ct));
motors_speed = [speed + 10; speed + 10; speed; speed];
dt = 0.01; 

lin_pos = [0; 0; 10];
lin_vel = [0; 0; 0];
ang_pos = [0; 0; 0];
ang_vel = [0; 0; 0];

result.t       = 0;
result.dt      = dt;
result.lin_pos = lin_pos;
result.lin_vel = lin_vel;
result.ang_pos = ang_pos;
result.ang_vel = ang_vel;
result.input   = motors_speed;

for t = dt:dt:20-dt
    [ang_pos, ang_vel, lin_pos, lin_vel] = plant_step_2(quad_constants, ang_pos, ang_vel, lin_pos, lin_vel, motors_speed, dt);
    result.t       = [result.t, t];
    result.dt      = [result.dt, dt];
    result.lin_pos = [result.lin_pos, lin_pos];
    result.lin_vel = [result.lin_vel, lin_vel];
    result.ang_pos = [result.ang_pos, ang_pos];
    result.ang_vel = [result.ang_vel, ang_vel];
    result.input   = [result.input, motors_speed];
end

visualize_2D(result)