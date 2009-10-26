%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% CS221 Programming Assignment 2
%%   Olga Russakovsky, Oct. 2009
%%   Stanford University
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% CS221 TODO: Modify as needed to switch between 1k and 10k examples
numExamples = '1k';

trainingImages = sprintf('data/training-%s-images.idx3', numExamples);
trainingLabels = sprintf('data/training-%s-labels.idx1', numExamples);

testImages = 'data/test-1k-images.idx3';
testLabels = 'data/test-1k-labels.idx1';

figureTitle = sprintf('Training on %s examples', numExamples); 

DigitSet = loadDigits(trainingImages, trainingLabels);
DigitTestSet = loadDigits(testImages, testLabels);
    
% Train and evaluate the tree at multiple depths
depths = 4:2:14;  % 4, 6, 8, 10, 12, 14
trainingAccuracies = [];
testSetAccuracies = [];

% CS221 Debugging
% ---------------
% After you implement the 4 required functions for part (a), uncomment
% the following 4 lines and run pa2_matlab_a. Make sure the trainingImages
% and trainingLabels above are set to use 1k examples, not 10k. 
%    depths = 1:3;
%    DigitSet.pixels = DigitSet.pixels(1:100, :);
%    DigitSet.labels = DigitSet.labels(1:100);
%    DigitSet.weights = DigitSet.weights(1:100);
% Your output should be:
%   Loading 1000 digits
%   Loading 1000 digits
%   Training trees of depth 1...
%   Testing...
%    ... training set accuracy of 0.450000
%    ... test set accuracy of 0.356000
%   Training trees of depth 2...
%   Testing...
%    ... training set accuracy of 0.780000
%    ... test set accuracy of 0.441000
%   Training trees of depth 3...
%   Testing...
%    ... training set accuracy of 0.940000
%    ... test set accuracy of 0.512000

tic
for depth = depths
    % Training code
    disp(sprintf('Training trees of depth %i...', depth));
    DecisionTrees = {};
    for label=0:9
        DecisionTrees{label+1} = growDecisionTree([], DigitSet, ...
            label, depth);
    end

    % Testing code
    disp('Testing...');
    acc = decisionTreeAccuracy(DecisionTrees, DigitSet);
    disp(sprintf(' ... training set accuracy of %f', acc));
    trainingAccuracies = [trainingAccuracies acc];
    
    acc = decisionTreeAccuracy(DecisionTrees, DigitTestSet);
    disp(sprintf(' ... test set accuracy of %f', acc));
    testSetAccuracies = [testSetAccuracies acc];
end
toc

% Plotting code
figure;
plot(depths, trainingAccuracies, 'b.--', 'MarkerSize', 30, 'LineWidth', 2);
hold on;
plot(depths, testSetAccuracies, 'r*--', 'MarkerSize', 30, 'LineWidth', 2);
ylim([0 1]);
title(figureTitle);
xlabel('Tree depth');
ylabel('Accuracy');
legend('Training set accuracy', 'Test set accuracy', 'location', 'East');
hold off;
