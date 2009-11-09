%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% CS221 Programming Assignment 3
%%   Chris Archibald, Oct. 2009
%%   Stanford University
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Tests a transition model that has been learned with only
% 10 iterations of learnTransitionModel()
% If the model is incorrect, text will display to the screen
% saying this
%
% Note: this function assumes that the random seed is
% set to 221 in the beginning of learnTransitionModel.m
%
% Input:
% - tmodel is the learned transition model.
%



function testFirstTen(tmodel)

    display('Testing 10 iteration transition model. . . ');

    pass = true;
    spots = zeros(1,35);
    j=1;
    for i = 1:length(tmodel)
        if tmodel(i).visitedStates(1)
            spots(j) = i;
            j = j+1;
		if tmodel(i).visitedStatesCount(1) ~= 1
			pass = false;
			display('  Transition added twice.');
		end
        end
    end
    
    if sum(spots) == 226108225
        display('  Correct Sum');
    else
        display('  Incorrect Sum');
        pass = false;
    end
    
    
    if max(spots) == 977900
        display('  Correct Max');
    else
        display('  Incorrect Max');
        pass = false;
    end
    
    
    if min(spots) == 225366
        display('  Correct Min');
    else
        display('  Incorrect Min');
        pass = false;
    end
    
    
    if tmodel(490880).visitedStates(1) == 15026
        display('  Correct Spot Check');
    else
        display('  Incorrect Spot Check');
        pass = false;
    end
    
    if pass
        display('Tests passed');
    else
        display('Test failed');
    end
    
end