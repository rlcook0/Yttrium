%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% CS221 Programming Assignment 2
%%   Olga Russakovsky, Oct. 2009
%%   Stanford University
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% For this part of the assignment you will only be using the 1k dataset, no
% need to switch to 10k training examples
trainingImages = 'data/training-1k-images.idx3';
trainingLabels = 'data/training-1k-labels.idx1';

testImages = 'data/test-1k-images.idx3';
testLabels = 'data/test-1k-labels.idx1';

DigitSet = loadDigits(trainingImages, trainingLabels);
DigitTestSet = loadDigits(testImages, testLabels);

% Use trees of depth 4 only (you can experiment to see what happens with 1)
depth = 4;

% Initializing the weights
DigitSet.weights = DigitSet.weights / length(DigitSet.weights);

% Duplicate the DigitSet so AdaBoost can adjust the example weights
% independently for each label as it's training the classifier
DigitSets = repmat(DigitSet, 10, 1);

trainingAccuracies = [];
testSetAccuracies = [];
AdaBoostTreesSet = {};
for numBags = 1:200
    disp(sprintf('  Adding tree %i...', numBags));

    for label=0:9
        disp(sprintf('    For label %i...', label));
        [DecisionTree, alpha, DigitSets(label+1)] = ...
            addAdaBoostDecisionTree(DigitSets(label+1), label, depth);
        AdaBoostTreesSet{label+1}{numBags}.tree = DecisionTree;
        AdaBoostTreesSet{label+1}{numBags}.weight = alpha;
    end

    % Testing code
    disp(sprintf('  Testing AdaBoost classifier with %i decision trees per label...', numBags));
    acc = adaBoostAccuracy(AdaBoostTreesSet, DigitSet); % pass in the unweighted set
    disp(sprintf('   ... training set accuracy of %f', acc));
    trainingAccuracies = [trainingAccuracies acc];

    acc = adaBoostAccuracy(AdaBoostTreesSet, DigitTestSet);
    disp(sprintf('   ... test set accuracy of %f', acc));
    testSetAccuracies = [testSetAccuracies acc];

    % Note: to save time, we terminate this loop if test set accuracy
    % converges before reaching 200 bags.
    n = length(testSetAccuracies);
    if (n > 2)
        accPrev2 = testSetAccuracies(n-2);
        accPrev = testSetAccuracies(n-1);
        if(abs(acc - accPrev) < 0.005 && ...
                abs(accPrev - accPrev2) < 0.005)
            break;
        end
    end
end
toc

% Plotting code
figure;
plot(1:numBags, trainingAccuracies, 'b.--', 'MarkerSize', 30, 'LineWidth', 2);
hold on;
plot(1:numBags, testSetAccuracies, 'r*--', 'MarkerSize', 30, 'LineWidth', 2);
title(sprintf('AdaBoost performance, trees of depth %i', depth));
xlabel('Number of weak classifiers');
ylabel('Accuracy');
legend('Training set accuracy', 'Test set accuracy', 'location', 'East');
hold off;



