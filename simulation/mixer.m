function [ motors_speed ] = mixer(commands)
    motors_speed = matrix_mixer * commands;
end

function [ matrix ] = matrix_mixer()
    matrix = [ -1 -1 -1  1;
                1 -1  1  1;
                1  1 -1  1;
               -1  1  1  1];
end