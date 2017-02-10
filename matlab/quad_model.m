function [ang_pos_i, ang_vel_b, lin_pos_i, lin_vel_b] = quad_model(quad_constants, ang_pos_i, ang_vel_b, lin_pos_i, lin_vel_b, motors_speed, dt)
    % P Q R in units of rad/sec
    P = ang_vel_b(1);
    Q = ang_vel_b(2);
    R = ang_vel_b(3);
    
    % Phi The Psi in radians
    Phi = ang_pos_i(1);
    The = ang_pos_i(2);

    % Obtain dP dQ dR
    OMb_bi = [ 0, -R,  Q;
               R,  0, -P;
              -Q,  P,  0];
          
    % CALCULATE MOMENT AND THRUST FORCE
    ang_acc_b = angular_acceleration(quad_constants, motors_speed, ang_vel_b);
    
    H_Phi = [1, tan(The)*sin(Phi), tan(The)*cos(Phi);
             0,          cos(Phi),         -sin(Phi);
             0, sin(Phi)/cos(The), cos(Phi)/cos(The)];
         
    ang_vel_i = H_Phi * ang_vel_b;

    % Compute Rotation Matrix. We use a Z-Y-X rotation
    Rib = rotation(ang_pos_i);
    
    % Compute Velocity and Position derivatives of body frame
    lin_acc_b = acceleration(quad_constants, motors_speed, ang_pos_i, OMb_bi, lin_vel_b);
    lin_vel_i = Rib * lin_vel_b;  % Units OK SI: Velocity of body frame w.r.t inertia frame (FOR POSITION)

    ang_pos_i = ang_pos_i + ang_vel_i * dt;
    ang_vel_b = ang_vel_b + ang_acc_b * dt;
    lin_pos_i = lin_pos_i + lin_vel_i * dt;
    lin_vel_b = lin_vel_b + lin_acc_b * dt;
    
end

function lin_acc_b = acceleration(quad_constants, motors_speed, angles, OMb_bi, lin_vel_b)
    Rbi       = rotation(angles)';
    thrust_b  = thrust(quad_constants, motors_speed);
    gravity_i = [0; 0; -quad_constants.g];
    gravity_b = Rbi * gravity_i;
    lin_acc_b = (1/quad_constants.mass) * thrust_b + gravity_b - OMb_bi * lin_vel_b;
end

function omega_dot_b = angular_acceleration(quad_constants, motors_speed, omega_b)
    tau_b = torques(quad_constants, motors_speed);
    omega_dot_b = quad_constants.J_binv * (tau_b - cross(omega_b, quad_constants.J_b * omega_b));
end

% Compute thrust given current inputs and thrust coefficient.
function thrust_b = thrust(quad_constants, motors_speed)
    square = motors_speed.*motors_speed;
    thrust_b = thrust_matrix(quad_constants) * square;
end

% Compute torques, given current inputs, length, drag coefficient, and thrust coefficient.
function tau_b = torques(quad_constants, motors_speed)
    square = motors_speed.*motors_speed;
    tau_b = torques_matrix(quad_constants) * square;
end

function M = torques_matrix(quad_constants)
    ct = quad_constants.d * quad_constants.ct * sin(pi/4);
    cq = quad_constants.cq;
    M = [-ct,  ct,  ct, -ct;
         -ct, -ct,  ct,  ct;
         -cq,  cq, -cq,  cq];
end

function T = thrust_matrix(quad_constants)
    ct = quad_constants.ct;
    T = [ 0,  0,  0,  0;
          0,  0,  0,  0;
         ct, ct, ct, ct];
end

