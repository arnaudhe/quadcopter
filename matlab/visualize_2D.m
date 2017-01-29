function visualize(result)

figure

subplot(2,2,1)
plot(result.t, result.ang_pos)
title('Angular position')

subplot(2,2,2)
plot(result.t, result.ang_vel)
title('Angular velocity')

subplot(2,2,3)
plot(result.t, result.lin_pos)
title('Linear position')

subplot(2,2,4)
plot(result.t, result.lin_vel)
title('Linear velocity')

end

