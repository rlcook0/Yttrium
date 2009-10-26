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

% Train bagged trees and evaluate at multiple depths and with multiple
% numbers of bags
depths = 4:2:14;

figure;
%title('Performance of Bagged Decision Trees', 'fontsize', 24);
p = 1;
for depth = depths
    disp('***************************************');
    disp(sprintf('TRAINING TREES OF DEPTH %i...', depth));  
    disp('***************************************');

    tic
    
    trainingAccuracies = [];
    testSetAccuracies = [];
    BaggedDecisionTreesSet = {};
    for numBags = 1:100
        disp(sprintf('  Adding tree %i...', numBags));  
        
        for label=0:9
            disp(sprintf('    For label %i...', label));
            BaggedDecisionTreesSet{label+1}{numBags} = ...
                addBaggedDecisionTree(DigitSet, label, depth);
        end

        % Testing code
        disp(sprintf('  Testing classifier set with %i bagged decision trees per label...', numBags));
        acc = baggedDecisionTreeAccuracy(BaggedDecisionTreesSet, DigitSet);
        disp(sprintf('   ... training set accuracy of %f', acc));
        trainingAccuracies = [trainingAccuracies acc];
    
        acc = baggedDecisionTreeAccuracy(BaggedDecisionTreesSet, DigitTestSet);
        disp(sprintf('   ... test set accuracy of %f', acc));
        testSetAccuracies = [testSetAccuracies acc];
        
        % Note: to save time, we terminate this loop if test set accuracy
        % converges before reaching 100 bags.  
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
    subplot(3, 2, p);
    p = p+1;
    plot(1:numBags, trainingAccuracies, 'b.--', 'MarkerSize', 5);
    hold on;
    plot(1:numBags, testSetAccuracies, 'r*--', 'MarkerSize', 5);
    title(sprintf('Depth %i', depth));
    xlabel('Number of bags');
    ylabel('Accuracy');
    axis([1, numBags, 0, 1]);
end

