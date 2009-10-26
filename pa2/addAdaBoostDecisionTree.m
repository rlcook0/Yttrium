%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% CS221 Programming Assignment 2
%%   Olga Russakovsky, Oct. 2009
%%   Stanford University
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Runs one iteration of the AdaBoost algorithm by adding one DecisionTree
% classifier to the ensemble and updating the weights. This
% allows you to easily graph the accuracy as a function of the number of
% trees.
%
% Refer to lecture notes 7 for details about the algorithm (this
% function corresponds to the update for a single value of b). Note,
% however, that you do not need to implement the sampling step explicitly
% here, since along the way (e.g., in initializeProbabilities and
% choosePixelAndThreshold functions) you were already considering the
% weight of the training examples that reach each node (you were, right?).
%
% Input:
% - DigitSet is the set of all pixels
%       DigitSet.pixels is a M x N matrix of pixel values, where M is the
%       number of example images and N is the number of pixels within each
%       example image 
%       DigitSet.labels is a M-dimensional vector of example labels (0-9)
%       DigitSet.weights is a M-dimensional vector of example weights
% - positiveLabel is the label of the positive class for this decision tree
% (0-9)
% - depth is the depth of the DecisionTree
%
% Output:
% - A DecisionTree of the appropriate depth (similar to 'h' in the notes)
% - a weight for this classifier (corresponds to alpha in the notes)
% - the DigitSet with modified training example weights (D_b(1) ... D_b(m)
%   in the notes) 

function [DecisionTree, alpha, DigitSet] = ...
    addAdaBoostDecisionTree(DigitSet, positiveLabel, depth)

% This is the hypothesis that you learn on the sampled training examples
DecisionTree = growDecisionTree([], DigitSet, positiveLabel, depth);

% CS221 TODO: finish implementing this function

end

