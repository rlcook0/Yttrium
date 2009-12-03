options = makeLocalizeOptions();
options.actionModel = 'simple';
options.sensorModel = 'laser';
options.kidnapped = 1;
options.history = 1;
options.imageOutputOptions.showImages = 0;
options.imageOutputOptions.saveImages = 0;
options.imageOutputOptions.framesPerImage = 200;
options.outputTotalError = 1;

localize(options)

% 