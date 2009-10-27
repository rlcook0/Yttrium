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
%       DigitSet.weights is a M-dimensional vector of example weights
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

% [positiveCount totalCount] = initializeProbabilities(DigitSet, positiveLabel);
% start = totalCount * entropy(positiveCount / totalCount);

[numImages numPixels] = size(DigitSet.pixels);

thresholds =  0.1:0.1:0.9;
all_inclass = sum((DigitSet.labels == positiveLabel) .* DigitSet.weights);

% bestEnt = inf;
% pixelNum = -1;

% bestInfGain = inf;
% 
% arr = -1;
% notarr = -1;

Q = repmat(DigitSet.pixels, [1 1 length(thresholds)]);
t = reshape(thresholds, [1 1 length(thresholds)]);
R = repmat(t, [size(Q, 1) size(Q, 2)]);

diff = Q > R;
weights = repmat(DigitSet.weights, [1 numPixels length(thresholds)]);
mult = weights .* diff;
summed_mult = sum(mult, 1);

inclass_mult = repmat(DigitSet.labels == positiveLabel, [1 numPixels length(thresholds)]) .* mult;
summed_inclass_mult = sum(inclass_mult, 1);

p1_mult = summed_inclass_mult ./ summed_mult;
p2_mult = (all_inclass - summed_inclass_mult) ./ (numImages - summed_mult);

p1_ent_mult = -p1_mult .* log2(p1_mult) - (1 - p1_mult) .* log2(1 - p1_mult);
p2_ent_mult = -p2_mult .* log2(p2_mult) - (1 - p2_mult) .* log2(1 - p2_mult);

p1_ent_mult(isnan(p1_ent_mult)) = 0;
p2_ent_mult(isnan(p2_ent_mult)) = 0;

total_ent_mult = summed_mult .* p1_ent_mult + (numImages - summed_mult) .* p2_ent_mult;

[ents indexes] = min(total_ent_mult, [], 3);
[~, pixIdx] = min(ents);

pixelNum = pixIdx;
threshold = thresholds(indexes(pixIdx));

% for pixel = 1:numPixels
%     A = repmat(DigitSet.pixels(:, pixel), 1, length(thresholds));
%     B = repmat(thresholds, length(DigitSet.pixels(:, pixel)), 1);
%     pixelsOverThreshold = (A > B);
%     weighted = repmat(DigitSet.weights, 1, length(thresholds)) .* pixelsOverThreshold;
%     summed = sum(weighted, 1);
%     
%     inclass = repmat(DigitSet.labels == positiveLabel, 1, length(thresholds)) .* weighted;
%     summed_inclass = sum(inclass, 1);
%     
%     p1 = summed_inclass ./ summed;
%     p2 = (all_inclass - summed_inclass) ./ (numImages - summed);
%     
%     p1_ent = arrayfun(@(x) entropy(x), p1);
%     p2_ent = arrayfun(@(x) entropy(x), p2);
%     
%     total_ent = summed .* p1_ent + (numImages - summed) .* p2_ent;
%     [val, thresh] = min(total_ent);
%     
%     if (val < bestEnt)
% %         disp([summed_inclass; summed; (all_inclass - summed_inclass); (numImages - summed)]);
%        
%         bestEnt = val;
%         pixelNum = pixel;
% %         notarr = pixelNum;
%         threshold = thresholds(thresh);
%     end
%     
%     
% %     for thresh = 0.1:0.1:0.9
% %         mlplus1 = 0;
% %         ml1 = 0;
% %         
% %         mlplus2 = 0;
% %         ml2 = 0;
% % 
% %         for image = 1:numImages
% %             if (DigitSet.pixels(image, pixel) > thresh)
% %                 if (DigitSet.labels(image) == positiveLabel)
% %                     mlplus1 = mlplus1 + DigitSet.weights(image);
% %                 end
% %                 ml1 = ml1 + DigitSet.weights(image);
% %             else
% %                 if (DigitSet.labels(image) == positiveLabel)
% %                     mlplus2 = mlplus2 + DigitSet.weights(image);
% %                 end
% %                 ml2 = ml2 + DigitSet.weights(image);
% %             end
% %         end
% % 
% %         p1 = mlplus1 / ml1;
% %         p2 = mlplus2 / ml2;
% %         %infgain = start - ml1 * entropy(p1) - ml2 * entropy(p2);
% %         infgain = ml1 * entropy(p1) + ml2 * entropy(p2);
% %         if (infgain < bestInfGain)
% %             arr = pixel;
% %             bestInfGain = infgain;
% %         end
% %     end
% end
% % 
% if (pixelNum ~= test_pixelNum)
%     disp([pixelNum; test_pixelNum]);
%     disp([threshold; test_threshold]);
%     
%     disp(reshape(indexes, [length(indexes) 1 1]));
%     disp(reshape(ents, [length(ents) 1 1]));
%     
% %     disp(total_ent_mult);
% end
% % 
% % pixelNum = test_pixelNum;
% % threshold = test_threshold;
% 
% end
% 
% function entropy = entropy(p)
%     if (p == 0 || p == 1 || isnan(p))
%         entropy = 0;
%     else
%         entropy = -p * log2(p) - (1 - p) * log2(1 - p);
%     end
% end
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
%     and T is the number of thresholds, and greaterThans(i, j) will be 1
%     if the p^th pixel of training example i is greater than the j^th
%     threshold, and 0 otherwise.
%     You can then use it to construct other useful NxT arrays, e.g.,
%           weighted = repmat(DigitSet.weights, 1, length(thresholds)) .* greaterThans;
%     where weighted(i, j) will correspond to the weight of the i^th
%     training example if its p^th pixel value is greater than
%     thresholds(j), and 0 otherwise. You can then sum over the appropriate
%     dimension using
%           sum(weighted, 1)
%     to obtain an array of weights, one per threshold, that is useful for
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

