%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% CS221 Programming Assignment 2
%%   Olga Russakovsky, Oct. 2009
%%   Stanford University
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Outputs the confidence of assigning the digit to the positive class
% according to the given DecisionTree 
%
% Input:
% - DecisionTree is the output of the growDecisionTree function, and is an
%   array of DecisionTreeNodes (see initializeDecisionTreeNode.m)
% - Digits is just an N-dimensional vector of pixel values

function conf = positiveConfidence(DecisionTree, Digit)

% CS221 TODO: implement this function
% -----------------------------------
% Debugging:
%   The following lines of code
%       DigitSet = loadDigits('data/training-1k-images.idx3', 'data/training-1k-labels.idx1');
%       DecisionTree = growDecisionTree([], DigitSet, 0, 2);
%       conf = positiveConfidence(DecisionTree, DigitSet.pixels(1, :));
%   should return
%       conf = 0.0352

currentNode = DecisionTree(1);
while (currentNode.pixelNum >= 0)
    if (Digit(currentNode.pixelNum) > currentNode.threshold)
        currentNode = DecisionTree(currentNode.leftChild);
    else
        currentNode = DecisionTree(currentNode.rightChild);
    end
end

conf = currentNode.positiveCount / currentNode.totalCount;

end

