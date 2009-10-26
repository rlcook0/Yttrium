%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% CS221 Programming Assignment 2
%%   Olga Russakovsky, Oct. 2009
%%   Stanford University
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% A decision tree is represented as an array of nodes. Note that this is an
% array and not a cell array, so given a decision tree you need to use the
% syntax DecisionTree(i), not DecisionTree{i}, to access the i^th node.
%
% Each node is a struct with the following elements:
%  - pixelNum and threshold correspond to the pixel to split on at this
%    node and the threshold to use
%  - leftChild and rightChild are indices of the left and right child nodes
%    respectively (recall that a digit get passed down to leftChild if its
%    pixel value is strictly greater than threshold, and to the rightChild
%    otherwise) 
%  - positiveCount and totalCount are used to represent the probabilities
%    at the leaf nodes 

function DecisionTreeNode = initializeDecisionTreeNode()
    DecisionTreeNode.pixelNum = -1;
    DecisionTreeNode.threshold = -1;
    DecisionTreeNode.leftChild = -1;
    DecisionTreeNode.rightChild = -1;
    DecisionTreeNode.positiveCount = 0;
    DecisionTreeNode.totalCount = 0;
end
