% Compute evolution of the plant
function [theta, theta_dot, x, x_dot] = plant_step(quad_constants, theta, theta_dot, x, x_dot, motors_speed, dt)

    % Compute forces, torques, and accelerations.
    acc_i     = acceleration(quad_constants, motors_speed, theta);
    omega_dot = angular_acceleration(quad_constants, motors_speed, theta_dot_to_omega(theta_dot, theta));
    
    % Advance system state.
    theta_dot = omega_to_theta_dot(theta_dot_to_omega(theta_dot, theta) + dt * omega_dot, theta); 
    theta     = theta + dt * theta_dot;
    x_dot     = x_dot + dt * acc_i;
    x         = x + dt * x_dot;
    
end

% Compute rotation matrix for a set of angles.
function R = rotation(angles)
    phi   = angles(3);
    theta = angles(2);
    psi   = angles(1);

    R = zeros(3);
    R(:, 1) = [
        cos(phi) * cos(theta)
        cos(theta) * sin(phi)
        - sin(theta)
    ];
    R(:, 2) = [
        cos(phi) * sin(theta) * sin(psi) - cos(psi) * sin(phi)
        cos(phi) * cos(psi) + sin(phi) * sin(theta) * sin(psi)
        cos(theta) * sin(psi)
    ];
    R(:, 3) = [
        sin(phi) * sin(psi) + cos(phi) * cos(psi) * sin(theta)
        cos(psi) * sin(phi) * sin(theta) - cos(phi) * sin(psi)
        cos(theta) * cos(psi)
    ];
end

% Compute thrust given current inputs and thrust coefficient.
function thurst_b = thrust(quad_constants, motors_speed)
    thurst_b = [0; 0; quad_constants.ct * sum(motors_speed.*motors_speed)];
end

% Compute torques, given current inputs, length, drag coefficient, and thrust coefficient.
function tau_b = torques(quad_constants, motors_speed)
    inputs = motors_speed.*motors_speed;
    tau_b = [
        quad_constants.arms_L * quad_constants.ct * (inputs(1) - inputs(3))
        quad_constants.arms_L * quad_constants.ct * (inputs(2) - inputs(4))
        quad_constants.cq * (inputs(1) - inputs(2) + inputs(3) - inputs(4))
    ];
end

function acc_i = acceleration(quad_constants, motors_speed, angles)
    gravity_i = [0; 0; - quad_constants.g * quad_constants.mass];
    R_bi = rotation(angles);
    Thurst_i = R_bi * thrust(quad_constants, motors_speed);
    acc_i = (gravity_i + Thurst_i) / quad_constants.mass;
end

function omega_dot_b = angular_acceleration(quad_constants, motors_speed, omega_b)
    tau_b = torques(quad_constants, motors_speed);
    omega_dot_b = inv(quad_constants.J_b) * (tau_b - cross(omega_b, quad_constants.J_b * omega_b));
end

% Convert derivatives of roll, pitch, yaw to omega.
function omega = theta_dot_to_omega(theta_dot, angles)
    phi   = angles(1);
    theta = angles(2);
    psi   = angles(3);
    W = [1,  0,        -sin(theta)
         0,  cos(phi),  cos(theta)*sin(phi)
         0, -sin(phi),  cos(theta)*cos(phi)];
    omega = W * theta_dot;
end

% Convert omega to roll, pitch, yaw derivatives
function theta_dot = omega_to_theta_dot(omega, angles)
    phi   = angles(1);
    theta = angles(2);
    psi   = angles(3);
    W = [1,  0,        -sin(theta)
         0,  cos(phi),  cos(theta)*sin(phi)
         0, -sin(phi),  cos(theta)*cos(phi)];
    theta_dot = inv(W) * omega;
end
