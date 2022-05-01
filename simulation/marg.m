function [ acc_b, gyr_b, mag_b ] = marg( ang_pos, ang_vel)

    mag_i = [1; 0; 0];
    acc_i = [0; 0; 1];
    
    Rbi   = rotation(ang_pos)';
    acc_b = Rbi * acc_i + sensor_rand(-0.01, 0.01, 0);
    gyr_b = ang_vel     + sensor_rand(-0.01, 0.01, 0.05);
    mag_b = Rbi * mag_i + sensor_rand(-0.01, 0.01, 0);
    
end

function [ output ] = sensor_rand(min, max, offset)
    output = min + (min - max) .* rand(3, 1) + ones(3, 1) * offset;
end