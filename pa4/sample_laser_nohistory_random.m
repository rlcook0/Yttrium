options = makeLocalizeOptions();
options.actionModel = 'random';
options.sensorModel = 'laser';
options.kidnapped = 0;
options.history = 0;
options.imageOutputOptions.showImages = 0;
options.imageOutputOptions.saveImages = 0;
options.imageOutputOptions.framesPerImage = 200;
options.outputTotalError = 1;

localize(options)

% 