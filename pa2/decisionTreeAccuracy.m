%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% CS221 Programming Assignment 2
%%   Olga Russakovsky, Oct. 2009
%%   Stanford University
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Evaluates the accuracy of the decision tree classifier set, where
% 'accuracy' is the percent of correctly classified test set digits. Each
% digit is classified by all 10 decision trees, and is assigned a label
% corresponding to the decision tree which returns the highest probability.
% 
% Input:
% - DecisionTrees is a set of 10 decision trees, one for each label. Due to
% indexing in Matlab being 1-based instead of 0-based, DecisionTrees(i) is
% the decision tree for the (i-1)st label
% - DigitTestSet is a a set of test digits
%       DigitTestSet.pixels is a M x N matrix of pixel values, where M is the
%       number of example images and N is the number of pixels within each
%       example image 
%       DigitTestSet.labels is a M-dimensional vector of example labels (0-9)
%       DigitTestSet.weights is a M-dimensional vector of example weights
%       (all 1 in this case; feel free to ignore this field)

function accuracy = decisionTreeAccuracy(DecisionTrees, DigitTestSet)

% CS221 TODO: implement this function
% -----------------------------------
% Debugging:
%   The following lines of code
%      DigitSet = loadDigits('data/training-1k-images.idx3', 'data/training-1k-labels.idx1');
%      DigitSet.pixels = DigitSet.pixels(1:10, :);
%      DigitSet.labels = DigitSet.labels(1:10);
%      DigitSet.weights = DigitSet.weights(1:10);
%      for label=0:5
%        DecisionTrees{label+1} = growDecisionTree([], DigitSet, label, 1);
%      end
%      accuracy = decisionTreeAccuracy(DecisionTrees, DigitSet)
%   should return
%       accuracy = 0.3000

[numImages x] = size(DigitTestSet.pixels);

tp = 0;
total = numImages;

% loop through all the images
for image = 1:numImages
    bestScore = -inf;
    bestDigit = -1;
    
    % loop through each of the trees, seeking out the tree with the highest
    % confidence. that tree will determine our vote for what digit this
    % image might be
    for treeNum = 1:length(DecisionTrees)
        conf = positiveConfidence(DecisionTrees{treeNum}, DigitTestSet.pixels(image, :)');
        if (conf > bestScore)
            bestDigit = treeNum - 1;
            bestScore = conf;
        end
    end
    
    % if our guess is correct, add one to the number of correctly
    % identified digits
    if (bestDigit == DigitTestSet.labels(image))
        tp = tp + 1;
    end
    if (bestScore == 0)
        best = bestScore
    end
end

accuracy = tp / total;


