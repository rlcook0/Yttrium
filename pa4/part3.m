options = makeLocalizeOptions();
options.actionModel = 'simple';
options.sensorModel = 'laser';
options.kidnapped = 0;
options.history = 0;
options.imageOutputOptions.showImages = 0;
options.imageOutputOptions.saveImages = 1;
options.imageOutputOptions.framesPerImage = 1;
options.outputTotalError = 1;

localize(options);