classdef pid < handle
    
    %% Public properties
    properties (Access = public)
        sample_period = 0.01;   % For integrate/derivate
        integrate     = 0;      % For integrate
        previous      = 0;      % For derivative
        output        = 0;
        parameters    = [0 0 0];
    end

    %% Public methods
    methods (Access = public)
        
        function obj = pid(varargin)
            for i = 1:2:nargin
                if  strcmp(varargin{i}, 'sample_period')
                    obj.sample_period = varargin{i+1};
                elseif  strcmp(varargin{i}, 'parameters')
                    obj.parameters = varargin{i+1};
                else
                    error('Invalid argument');
                end
            end
        end
        
        function obj = update(obj, error)
            obj.integrate = obj.integrate + error * obj.sample_period;
            derivate      = (error - obj.previous) / obj.sample_period;
            obj.previous  = error;
            
            obj.output = obj.parameters(1) * error + ...
                         obj.parameters(2) * obj.integrate + ...
                         obj.parameters(3) * derivate;
        end
    end
end