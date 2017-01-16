function visualize(result)

close all

subplot(2,2,1)
plot(result(:, 1), result(:, 2),'g',result(:, 1), result(:, 3),'b',result(:, 1), result(:, 4),'r')
title('theta')

subplot(2,2,2)
plot(result(:, 1), result(:, 5),'g',result(:, 1), result(:, 6),'b',result(:, 1), result(:, 7),'r')
title('theta_dot')

subplot(2,2,3)
plot(result(:, 1), result(:, 8),'g',result(:, 1), result(:, 9),'b',result(:, 1), result(:, 10),'r')
title('x')

subplot(2,2,4)
plot(result(:, 1), result(:, 11),'g',result(:, 1), result(:, 12),'b',result(:, 1), result(:, 13),'r')
title('x_dot')

end

