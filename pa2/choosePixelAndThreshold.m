%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% CS221 Programming Assignment 2
%%   Olga Russakovsky, Oct. 2009
%%   Stanford University
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Uses entropy to decide which pixel to split on
%
% Input:
% - DigitSet is the set of all pixels at this node in the tree
%       DigitSet.pixels is a M x N matrix of pixel values, where M is the
%       number of example images and N is the number of pixels within each
%       example image 
%       DigitSet.labels is a M-dimensional vector of example labels (0-9)
%       DigitSet.weights_thresholds is a M-dimensional vector of example weights_thresholds
% - positiveLabel is the label of the positive class for this decision tree
% (0-9)
%
% Output:
% - pixelNum is the pixel number to split on (between 1 and N)
% - threshold is the split threshold (0.1, 0.2, ..., 0.9)

function [pixelNum threshold] = choosePixelAndThreshold(DigitSet, positiveLabel)

% CS221 TODO: implement this function
% -----------------------------------
% Debugging:
%   The following lines of code
%       DigitSet = loadDigits('data/training-10k-images.idx3', 'data/training-10k-labels.idx1');
%       [pixelNum threshold] = choosePixelAndThreshold(DigitSet, 3)
%   should return
%       pixelNum = 34
%       threshold = 0.2000

% Useful values.
numPixels = size(DigitSet.pixels, 2);
%numImages = size(DigitSet.pixels, 1);
weightedNumImages = sum(DigitSet.weights);
thresholds =  0.1:0.1:0.9;
num_positiveClass = sum((DigitSet.labels == positiveLabel) .* DigitSet.weights);

% Compute which pixels are over which threshold.
pixels_thresholds = repmat(DigitSet.pixels, [1 1 length(thresholds)]);
threshold3d = reshape(thresholds, [1 1 length(thresholds)]);
thresholds_pixels = repmat(threshold3d, [size(pixels_thresholds, 1) size(pixels_thresholds, 2)]);
pixels_over_thresholds = pixels_thresholds > thresholds_pixels;

% Weight each pixel over a threshold, then sum it over all images.
weights_thresholds = repmat(DigitSet.weights, [1 numPixels length(thresholds)]);
weighted_pixels_over_thresholds = weights_thresholds .* pixels_over_thresholds;
pixels_threshold_values = sum(weighted_pixels_over_thresholds, 1);

% Do the same for only images in the positive class.
weighted_inclass = repmat(DigitSet.labels == positiveLabel, [1 numPixels length(thresholds)]) .* weighted_pixels_over_thresholds;
pixels_inclass_values = sum(weighted_inclass, 1);

% Compute probability values for each pixel at each threshold.
p1 = pixels_inclass_values ./ pixels_threshold_values;
p2 = (num_positiveClass - pixels_inclass_values) ./ (weightedNumImages - pixels_threshold_values);

% Convert probability to entropy.
p1_entropy = -p1 .* log2(p1) - (1 - p1) .* log2(1 - p1);
p2_entropy = -p2 .* log2(p2) - (1 - p2) .* log2(1 - p2);

% Lose NaN entries...
p1_entropy(isnan(p1_entropy)) = 0;
p2_entropy(isnan(p2_entropy)) = 0;

% Convert to information gain-relevant entropy.
total_entropy = pixels_threshold_values .* p1_entropy + (weightedNumImages - pixels_threshold_values) .* p2_entropy;

% Find the minimum entropy threshold,pixel pair.
[entropy indexes] = min(total_entropy, [], 3);
[~, pixIdx] = min(entropy);

% We're done!
pixelNum = pixIdx;
threshold = thresholds(indexes(pixIdx));

end

% Important note:
%  This function will get called millions of times as you are running
%  experiments; thus, you might want to consider some optimization tricks.
%  Be careful, start by first implementing the simplest possible solution
%  you can think of, and verify that your results are correct. 
%
% After you're done, you might want to consider that for-loops in Matlab
% are extremely inefficient, and so eliminating even just one inner loop
% will often greatly speed up your code. For instance, consider this trick: 
%    For each pixel p, you will be iterating over the possible thresholds
%    from 0.1 to 0.9 and computing the entory at each one. Letting
%           A = repmat(DigitSet.pixels(:, p), 1, length(thresholds))
%           B = repmat(thresholds, length(DigitSet.pixels(:, p)), 1)
%           greaterThans = (A > B);
%     will create a NxT array, where N is the number of training examples
%     and threshold3d is the number of thresholds, and greaterThans(i, j) will be 1
%     if the p^th pixel of training example i is greater than the j^th
%     threshold, and 0 otherwise.
%     You can then use it to construct other useful NxT arrays, e.g.,
%           weighted = repmat(DigitSet.weights_thresholds, 1, length(thresholds)) .* greaterThans;
%     where weighted(i, j) will correspond to the weight of the i^th
%     training example if its p^th pixel value is greater than
%     thresholds(j), and 0 otherwise. You can then sum over the appropriate
%     dimension using
%           sum(weighted, 1)
%     to obtain an array of weights_thresholds, one per threshold, that is useful for
%     this problem.
% You will need more than the arrays discussed here to efficiently
% implement the function, but this should help you get started. In general,
% you should:
%      (1) avoid using for-loops whenever possible and instead use
%          operations on arrays (e.g., 
%              [m i] = min(v)
%          will return the mininum element m of a vector v, and the
%          corresponding index i)
%      (2) avoid using if-statements if possible
%
% Please come by office hours if this is unclear or you need more help.

