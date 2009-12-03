options = makeLocalizeOptions();
options.actionModel = 'random';
options.sensorModel = 'bump';
options.kidnapped = 0;
options.history = 1;
options.imageOutputOptions.showImages = 0;
options.imageOutputOptions.saveImages = 0;
options.imageOutputOptions.framesPerImage = 200;
options.outputTotalError = 1;

localize(options)

% PASSES