function visualize(result)

figure

subplot(4,3,1)
plot(result.t, result.ang_vel(1,:), 'b')
title('P')
grid on

subplot(4,3,2)
plot(result.t, result.ang_vel(2,:), 'r')
title('Q')
grid on

subplot(4,3,3)
plot(result.t, result.ang_vel(3,:), 'g')
title('R')
grid on

subplot(4,3,4)
plot(result.t, result.ang_pos(1,:), 'b')
title('Phi')
grid on

subplot(4,3,5)
plot(result.t, result.ang_pos(2,:), 'r')
title('Theta')
grid on

subplot(4,3,6)
plot(result.t, result.ang_pos(3,:), 'g')
title('Psi')
grid on

subplot(4,3,7)
plot(result.t, result.lin_vel(1,:), 'b')
title('U')
grid on

subplot(4,3,8)
plot(result.t, result.lin_vel(2,:), 'r')
title('V')
grid on

subplot(4,3,9)
plot(result.t, result.lin_vel(3,:), 'g')
title('W')
grid on

subplot(4,3,10)
plot(result.t, result.lin_pos(1,:), 'b')
title('X')
grid on

subplot(4,3,11)
plot(result.t, result.lin_pos(2,:), 'r')
title('Y')
grid on

subplot(4,3,12)
plot(result.t, result.lin_pos(3,:), 'g')
title('Z')
grid on


end

