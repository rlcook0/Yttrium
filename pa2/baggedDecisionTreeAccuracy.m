%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% CS221 Programming Assignment 2
%%   Olga Russakovsky, Oct. 2009
%%   Stanford University
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Evaluates the accuracy of the bagged decision tree classifier set, where
% 'accuracy' is the percent of correctly classified test set digits. For
% each digit x, let a_k(x) be the number of decision trees with positive
% class representing digit k that classify the digit into the positive
% class; then the output label should be the argmax over k of a_k(x),
% breaking ties by picking the lowest digit possible.
% 
% Input:
% - BaggedDecisionTreesSet is a set of 10 BaggedDecisionTrees, one for each
%   label. Due to indexing in Matlab being 1-based instead of 0-based,
%   BaggedDecisionTreesSet{i} is the collection of bagged decision trees
%   for the (i-1)st label. BaggedDecisionTreesSet{i}{j} is the j^th
%   DecisionTree constructed using bagging for classifying the i^th label.
% - DigitTestSet is a a set of test digits
%       DigitTestSet.pixels is a M x N matrix of pixel values, where M is
%       the number of example images and N is the number of pixels within
%       each example image 
%       DigitTestSet.labels is a M-dimensional vector of example labels
%       (0-9) DigitTestSet.weights is a M-dimensional vector of example
%       weights (all 1 in this case; feel free to ignore this field)

function accuracy = baggedDecisionTreeAccuracy(BaggedDecisionTreesSet, DigitTestSet)

% CS221 TODO: implement this function


end

