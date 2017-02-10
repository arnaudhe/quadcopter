classdef attitude_controller < handle
    
    %% Public properties
    properties (Access = public)
        sample_period   = 0.01;   % For integrate/derivate
        kp              = [2; 2; 4; 5];
        kd              = [1; 1; 6; 5];
        ki              = [1; 1; 0; 1];
        consign         = [0; 0; 0; 15];
        motors_throttle = [0; 0; 0; 0];
        previous        = [0; 0; 0; 0];
        integrates      = [0; 0; 0; 0];
        offsets         = [0; 0; 0; 500];
    end

    %% Public methods
    methods (Access = public)
        
        function obj = attitude_controller(varargin)
            for i = 1:2:nargin
                if  strcmp(varargin{i}, 'sample_period')
                    obj.sample_period = varargin{i+1};
                elseif  strcmp(varargin{i}, 'consign')
                    obj.consign = varargin{i+1};
                elseif  strcmp(varargin{i}, 'initial')
                    obj.previous = varargin{i+1};
                elseif strcmp(varargin{i}, 'quad_constants')
                    quad_constants = varargin{i+1};
                    obj.offsets = [0; 0; 0; ((sqrt((quad_constants.mass*quad_constants.g)/(4*quad_constants.ct))-quad_constants.b)/quad_constants.cr)];
                else
                    error('Invalid argument');
                end
            end
        end
        
        function obj = update(obj, attitude)
            error          = obj.consign - attitude;
            obj.integrates = obj.integrates + error * obj.sample_period;
            derivates      = (obj.previous - attitude) / obj.sample_period;
            obj.previous   = attitude;

            % Compute attitude regulator command
            commands = obj.kp .* error + ...
                       obj.ki .* obj.integrates + ...
                       obj.kd .* derivates + ...
                       obj.offsets;

            % Compute motors thottles
            obj.motors_throttle = mixer(commands);
        end
    end
end