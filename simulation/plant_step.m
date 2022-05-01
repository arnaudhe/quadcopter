% Compute evolution of the plant
function [ang_pos_i, ang_vel_i, lin_pos_i, lin_vel_i] = plant_step(quad_constants, ang_pos_i, ang_vel_i, lin_pos_i, lin_vel_i, motors_speed, dt)

    % Compute forces, torques, and accelerations.
    lin_acc_i = acceleration(quad_constants, motors_speed, ang_pos_i);
    ang_acc_b = angular_acceleration(quad_constants, motors_speed, ang_vel_i_to_ang_vel_b(ang_vel_i, ang_pos_i));
    
    % Advance system state.
    ang_vel_i = ang_vel_b_to_ang_vel_i(ang_vel_i_to_ang_vel_b(ang_vel_i, ang_pos_i) + dt * ang_acc_b, ang_pos_i); 
    ang_pos_i = ang_pos_i + dt * ang_vel_i;
    lin_vel_i = lin_vel_i + dt * lin_acc_i;
    lin_pos_i = lin_pos_i + dt * lin_vel_i;
    
end

% Compute thrust given current inputs and thrust coefficient.
function thrust_b = thrust(quad_constants, motors_speed)
    square = motors_speed.*motors_speed;
    thrust_b = thrust_matrix(quad_constants) * square; 
end

% Compute torques, given current inputs, length, drag coefficient, and thrust coefficient.
function tau_b = torques(quad_constants, motors_speed)
    square = motors_speed.*motors_speed;
    tau_b = torques_matrix_x(quad_constants) * square;
end

function M_x = torques_matrix_x(quad_constants)
    ct = quad_constants.d * quad_constants.ct * sin(pi/4);
    cq = quad_constants.cq;
    M_x = [-ct,  ct,  ct, -ct;
           -ct, -ct,  ct,  ct;
           -cq,  cq, -cq,  cq];
end

function M_plus = torques_matrix_plus(quad_constants)
    ct = quad_constants.d * quad_constants.ct;
    cq = quad_constants.cq;
    M_plus = [-ct,   0,  ct,   0;
                0, -ct,   0,  ct;
              -cq,  cq, -cq,  cq];
end

function T_x = thrust_matrix(quad_constants)
    ct = quad_constants.ct;
    T_x = [ 0,  0,  0,  0;
            0,  0,  0,  0;
           ct, ct, ct, ct];
end

function acc_i = acceleration(quad_constants, motors_speed, angles)
    gravity_i = [0; 0; - quad_constants.g * quad_constants.mass];
    R_bi = rotation(angles);
    thrust_i = R_bi * thrust(quad_constants, motors_speed);
    acc_i = (gravity_i + thrust_i) / quad_constants.mass;
end

function omega_dot_b = angular_acceleration(quad_constants, motors_speed, omega_b)
    tau_b = torques(quad_constants, motors_speed);
    omega_dot_b = quad_constants.J_binv * (tau_b - cross(omega_b, quad_constants.J_b * omega_b));
end

% Convert derivatives of roll, pitch, yaw to omega.
function omega = ang_vel_i_to_ang_vel_b(theta_dot, angles)
    phi   = angles(1);
    theta = angles(2);
    psi   = angles(3);
    W = [1,  0,        -sin(theta)
         0,  cos(phi),  cos(theta)*sin(phi)
         0, -sin(phi),  cos(theta)*cos(phi)];
    omega = W * theta_dot;
end

% Convert omega to roll, pitch, yaw derivatives
function theta_dot = ang_vel_b_to_ang_vel_i(omega, angles)
    phi   = angles(1);
    theta = angles(2);
    psi   = angles(3);
    W = [1,  0,        -sin(theta)
         0,  cos(phi),  cos(theta)*sin(phi)
         0, -sin(phi),  cos(theta)*cos(phi)];
    theta_dot = inv(W) * omega;
end
