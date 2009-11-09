%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% CS221 Programming Assignment 3
%%   Chris Archibald, Oct. 2009
%%   Stanford University
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Returns the number of times that a given discrete state and action pair
% have been sampled, i.e. the number of times that this random state was generated
% assuming that each action is tried in every random state, as directed in the 
% assignment
%
% Input: 
% - tmodel0 is the transition model
% - s0 is the discrete state of interest
% - a0 is the discrete action of interest
%
% Output: 
% - p is the number of times that this state action pair has been sampled


function p = transitionModelGetSampleCount(tmodel0, s0, a0)
    global DISCRETE_ACTION_COUNT;

    index = (s0 - 1)*DISCRETE_ACTION_COUNT + a0;
    p = sum(tmodel0(index).visitedStatesCount);
end
