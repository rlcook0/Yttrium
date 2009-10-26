%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% CS221 Programming Assignment 2
%%   Olga Russakovsky, Oct. 2009
%%   Stanford University
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Learns a decision tree of a specified depth by adding one node at a time.
% This function splits the training set and recurvisely calls itself. It
% should initialize the probabilities when it gets to a leaf node. To
% construct a tree starting from the root, pass in [] for DecisionTree.
%
% Do not modify this function
function DecisionTree = growDecisionTree(DecisionTree, DigitSet, ...
   positiveLabel, depth)

node = length(DecisionTree) + 1;
DecisionTree(node) = initializeDecisionTreeNode();

if (depth == 0 || length(DigitSet.labels) <= 1)
    % This is a leaf node, initialize the probabilities
    [positiveCount totalCount] = initializeProbabilities(DigitSet, ...
        positiveLabel);
    DecisionTree(node).positiveCount = positiveCount;
    DecisionTree(node).totalCount = totalCount;
else
    % Find the split to maximize entropy
    [pixelNum threshold] = choosePixelAndThreshold(DigitSet, positiveLabel);
    DecisionTree(node).pixelNum = pixelNum;
    DecisionTree(node).threshold = threshold;
    
    % recurvisely grow each child
    DecisionTree = growChild(DecisionTree, node, DigitSet, ...
        positiveLabel, depth, 0);
    DecisionTree = growChild(DecisionTree, node, DigitSet, ...
        positiveLabel, depth, 1);
end
end

% Creates a child node in the decision tree
% childNum = 0 => left child, values > threshold;
% childNum = 1 => right child, values <= threshold
function DecisionTree = growChild(DecisionTree, node, DigitSet, ...
    positiveLabel, depth, childNum)

threshold = DecisionTree(node).threshold;
pixelNum = DecisionTree(node).pixelNum;

numNodes = length(DecisionTree);

if (childNum == 0)
    indices = find(DigitSet.pixels(:, pixelNum) > threshold);
    DecisionTree(node).leftChild = numNodes+1;
else
    indices = find(DigitSet.pixels(:, pixelNum) <= threshold);
    DecisionTree(node).rightChild = numNodes+1;
end

% split the digit set and use part of it to build the desired child
DigitSet.pixels = DigitSet.pixels(indices, :);
DigitSet.labels = DigitSet.labels(indices);
DigitSet.weights = DigitSet.weights(indices);
DecisionTree = growDecisionTree(DecisionTree, DigitSet, positiveLabel, depth-1);

end