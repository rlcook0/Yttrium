%CS221 Programming Assignment 4
%Author: Ian Goodfellow

%Call this function to create an options struct, used to control an experiment run with localize.m
%After creating the options struct, you may control the behavior of localize.m by modifying the values
%in the struct
function options = makeLocalizeOptions()
	imageOutputOptions = struct('showImages',1,'saveImages',0,'framesPerImage',1);

	options = struct('imageOutputOptions',imageOutputOptions, ...
		   'outputTotalError',0, ...
		   'mapFilepath','gates1.ppm', ...
		   'logFilepath','robot1.log', ...
		   'sensorModel','bump', ... %Options are 'bump' or 'laser'
	 	   'actionModel','random',...%Options are 'random' or 'simple'
		   'history', 1, ...
		   'kidnapped', 0 ...
                   );
end