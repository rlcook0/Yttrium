%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% CS221 Programming Assignment 2
%%   Olga Russakovsky, Oct. 2009
%%   Stanford University
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Runs the bagging algorithm for one iteration, creating a new tree. This
% allows you to easily graph the accuracy as a function of the number of
% trees. You can refer to lecture notes 7 for details.
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
% - A DecisionTree of the appropriate depth constructed using bagging
%
function DecisionTree = addBaggedDecisionTree(DigitSet, positiveLabel, depth)

% CS221 TODO: implement this function


end


