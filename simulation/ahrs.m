classdef ahrs < handle
    
    %% Public properties
    properties (Access = public)
        sample_period = 0.01;       % Discrete period
        quaternion    = [1 0 0 0];  % output quaternion describing the Earth relative to the sensor
        beta          = 1;          % algorithm gain
    end

    %% Public methods
    methods (Access = public)
        
        function obj = ahrs(varargin)
            for i = 1:2:nargin
                if  strcmp(varargin{i}, 'sample_period')
                    obj.sample_period = varargin{i+1};
                elseif  strcmp(varargin{i}, 'quaternion')
                    obj.quaternion = varargin{i+1};
                elseif  strcmp(varargin{i}, 'beta')
                    obj.beta = varargin{i+1};
                else
                    error('Invalid argument');
                end
            end;
        end
        
        function obj = update(obj, gyr, acc, mag)
            q = obj.quaternion; % short name local variable for readability

            if(norm(acc) == 0) % handle NaN
                return;
            end
            acc = acc / norm(acc);	% normalise acceleration

            if(norm(mag) == 0) 	% handle NaN
                return; 
            end
            mag = mag / norm(mag);	% normalise magnitude

            % Reference direction of Earth's magnetic feild
            h = quaternProd(q, quaternProd([0 mag], quaternConj(q)));
            b = [0 norm([h(2) h(3)]) 0 h(4)];

            % Gradient decent algorithm corrective step
            F = [2*(q(2)*q(4) - q(1)*q(3)) - acc(1)
                 2*(q(1)*q(2) + q(3)*q(4)) - acc(2)
                 2*(0.5 - q(2)^2 - q(3)^2) - acc(3)
                 2*b(2)*(0.5 - q(3)^2 - q(4)^2) + 2*b(4)*(q(2)*q(4) - q(1)*q(3)) - mag(1)
                 2*b(2)*(q(2)*q(3) - q(1)*q(4)) + 2*b(4)*(q(1)*q(2) + q(3)*q(4)) - mag(2)
                 2*b(2)*(q(1)*q(3) + q(2)*q(4)) + 2*b(4)*(0.5 - q(2)^2 - q(3)^2) - mag(3)];
            
            J = [ -2*q(3),                 	2*q(4),                  -2*q(1),                         2*q(2)
                   2*q(2),                 	2*q(1),                   2*q(4),                         2*q(3)
                   0,                      -4*q(2),                  -4*q(3),                         0
                  -2*b(4)*q(3),             2*b(4)*q(4),             -4*b(2)*q(3)-2*b(4)*q(1),       -4*b(2)*q(4)+2*b(4)*q(2)
                  -2*b(2)*q(4)+2*b(4)*q(2),	2*b(2)*q(3)+2*b(4)*q(1),  2*b(2)*q(2)+2*b(4)*q(4),       -2*b(2)*q(1)+2*b(4)*q(3)
                   2*b(2)*q(3),             2*b(2)*q(4)-4*b(4)*q(2),  2*b(2)*q(1)-4*b(4)*q(3),        2*b(2)*q(2)];
            
            step = (J'*F);
            step = step / norm(step);	% normalise step magnitude

            % Compute rate of change of quaternion
            qDot = 0.5 * quaternProd(q, [0 gyr(1) gyr(2) gyr(3)]) - obj.beta * step';

            % Integrate to yield quaternion
            q = q + qDot * obj.sample_period;
            obj.quaternion = q / norm(q); % normalise quaternion
        end
    end
end