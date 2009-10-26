%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% CS221 Programming Assignment 2
%%   Olga Russakovsky, Oct. 2009
%%   Stanford University
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Used to initialize proabilities at a leaf node of the decision tree
%
% Input:
% - DigitSet is the set of all training examples at this node in the tree
%       DigitSet.pixels is a M x N matrix of pixel values, where M is the
%       number of example images and N is the number of pixels within each
%       example image 
%       DigitSet.labels is a M-dimensional vector of example labels (0-9)
%       DigitSet.weights is a M-dimensional vector of example weights
% - positiveLabel is the label of the positive class for this decision tree
% (0-9)
%
% Output:
% - positiveCount is the combined weight of the training examples that
%   reached this leaf node and whose label matches the positiveLabel
% - totalCount is the total combined weight of all examples that reached
%   this leaf node

function [positiveCount totalCount] = initializeProbabilities(DigitSet, positiveLabel)

% CS221 TODO: implement this function
% -----------------------------------
% Debugging:
%   The following lines of code
%       DigitSet = loadDigits('data/training-10k-images.idx3', 'data/training-10k-labels.idx1');
%       [positiveCount totalCount] = initializeProbabilities(DigitSet, 3)
%   should return
%       positiveCount = 1008;
%       totalCount = 10000;


end
