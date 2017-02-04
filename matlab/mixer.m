function [ motors_speed ] = mixer(commands)
    motors_speed = matrix_mixer_x * commands;
end

function [ matrix ] = matrix_mixer_x()
    matrix = [ -1 -1 -1  1;
                1 -1  1  1;
                1  1 -1  1;
               -1  1  1  1];
end

function [ matrix ] = matrix_mixer_plus()
    matrix = [ -1  0 -1  1;
                0 -1  1  1;
                1  0 -1  1;
                0  1  1  1];
end
