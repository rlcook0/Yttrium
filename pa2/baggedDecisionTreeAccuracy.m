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

% 1. for each image in the test set, loop through all the trees that should
%    classify that digit
% 2. output label = the digit that the most trees in that batch vote for
%    with confidence > 0.5
% 3. if correct, tp++

% if many bags of trees vote that an image is of a digit, then take the bag
% that votes most for a digit
  
    [numImages x] = size(DigitTestSet.pixels);
    
    tp = 0;
    total = numImages;
    
    % for each image in the test set
    for image = 1:numImages
        for index = 1:10
            a(index) = 0;
        end
        
        % for each bag of trees
        for bagNum = 1:10
            baggedTrees = BaggedDecisionTreesSet{bagNum};
            % for each tree in that bag
            for treeNum = 1:length(baggedTrees)
                % get the tree's confidence it is the given digit
                conf = positiveConfidence(baggedTrees{treeNum}, DigitTestSet.pixels(image, :));
                if (conf > 0.5)
                    a(bagNum) = a(bagNum) + 1;
                end
            end
        end
        
        highestVote = max(a);
        if (highestVote == DigitTestSet.labels(image))
            tp = tp+1;
        end
    end

    accuracy = tp / total;
end



