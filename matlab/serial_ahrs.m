clear all
close all

s = serial('/dev/tty.SLAB_USBtoUART');
set(s,'BaudRate',115200);

fopen(s);

flushinput(s)

addpath('quaternion_library');
ahrs = ahrs('sample_period', 0.01, 'beta', 3);

euler = [0 0 0]';

for i = 0:1:1000
    i
    out = fgetl(s);
    tokens = strsplit(out,';');
    values = str2double(tokens);
    if (size(values, 2) == 9)
        acc = [values(1) values(2) values(3)];
        gyr = [values(4) values(5) values(6)];
        mag = [values(7) values(8) values(9)];
        ahrs.update(gyr, acc, mag);
        euler = [euler quatern2euler(quaternConj(ahrs.quaternion))'];
    end 
end

plot(euler')

clear s
newobjs = instrfind;
fclose(newobjs);