function [ motors_speed ] = motors_model( motors_throttle, quad_constants )

    [motors_speed] = [motor_model(motors_throttle(1), quad_constants);
                      motor_model(motors_throttle(2), quad_constants);
                      motor_model(motors_throttle(3), quad_constants);
                      motor_model(motors_throttle(4), quad_constants)];
end

function [ speed ] = motor_model( throttle, quad_constants )

    if throttle > 100
        throttle = 100;
    end
    
    if throttle >= quad_constants.minThr
        speed = quad_constants.cr * throttle + quad_constants.b;
    else
        speed = 0;
    end
end
    

