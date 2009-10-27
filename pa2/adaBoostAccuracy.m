%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% CS221 Programming Assignment 2
%%   Olga Russakovsky, Oct. 2009
%%   Stanford University
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Evaluates the accuracy of the classifier set constructed using AdaBoost,
% where 'accuracy' is the percent of correctly classifier test set digits.
% For each digit x, let a_k(x, j) be 1 if the j^th tree trained for the
% positive class k classifies x into the positive class, and -1 otherwise
% (note the change from before!). Then let a_k(x) be a weighted sum of
% a_k(x, j) using the weights provided for each decision tree by AdaBoost.
% Finally, as in baggedDecisionTreesAccuracy, we take the output label to
% be the argmax over k of a_k(x), breaking ties by picking the lowest digit
% possible.  
% 
% Input:
% - AdaBoostTreesSet is a set of 10 AdaBoostTreesSet, one for each
%   label. Due to indexing in Matlab being 1-based instead of 0-based,
%   AdaBoostTreesSet{i} is the collection of bagged decision trees
%   for the (i-1)st label.
%       - AdaBoostTreesSet{i}{j}.tree is the j^th DecisionTree constructed
%         during AdaBoost for classifying the i^th label.
%       - AdaBoostTreesSet{i}{j}.weight is the weight of this j^th decision
%         tree
% - DigitTestSet is a a set of test digits
%       DigitTestSet.pixels is a M x N matrix of pixel values, where M is
%       the number of example images and N is the number of pixels within
%       each example image 
%       DigitTestSet.labels is a M-dimensional vector of example labels
%       (0-9) DigitTestSet.weights is a M-dimensional vector of example
%       weights (all 1 in this case; feel free to ignore this field)

function accuracy = adaBoostAccuracy(AdaBoostTreesSet, DigitTestSet)

% CS221 TODO: implement this function
    [numImages x] = size(DigitTestSet.pixels);
    
    tp = 0;
    total = numImages;
    
    % for each image in the test set
    for image = 1:numImages
        
        
        highestVote = -inf;
        digitVoted = -1;
        
        % for each bag of trees
        for bagNum = 1:10
            
        
            baggedTrees = AdaBoostTreesSet{bagNum};
            % for each tree in that bag
            
            a_j = zeros(length(baggedTrees),1);
            
            for treeNum = 1:length(baggedTrees)
                % get the tree's confidence it is the given digit
                t = baggedTrees{treeNum};
                conf = positiveConfidence(t.tree, DigitTestSet.pixels(image, :));
                if (conf > 0.5)
                    a_j(treeNum) = t.weight;
                else
                    a_j(treeNum) = - t.weight;
                end
            end
            
            a_k = sum(a_j);
            
            % keep track of the highest digit voted for by the bags
            % representing that class

            if (a_k > highestVote)
                highestVote = a_k;
                digitVoted = bagNum - 1;
            end
            
        
        end
        
        if (digitVoted == DigitTestSet.labels(image))
            tp = tp+1;
        end
        
    end

    accuracy = tp / total;
end
