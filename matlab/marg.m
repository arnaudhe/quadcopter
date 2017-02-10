function [ acc_b, gyr_b, mag_b ] = marg( ang_pos, ang_vel)

    mag_i = [1; 0; 0];
    acc_i = [0; 0; 1];
    
    Rbi   = rotation(ang_pos)';
    acc_b = Rbi * acc_i + (rand(3, 1) * 0.01);
    gyr_b = ang_vel     + ((rand(3, 1) + 5) * 0.01);
    mag_b = Rbi * mag_i + (rand(3, 1) * 0.01);
    
end

