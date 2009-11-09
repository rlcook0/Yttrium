%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% CS221 Programming Assignment 3
%%   Chris Archibald, Oct. 2009
%%   Stanford University
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Returns the expected value of taking a discrete action in a given discrete state
%
% Input:
% - tmodel0 is the transition model, (see transitionModelCreate.m) 
% - s0 is the initial discrete state
% - a0 is the discrete action to take
% - vf is the value function, a vector of length DISCRETE_STATE_COUNT
% Output:
% - v is the expected value of taking discrete action a0 in discrete state s0, where 
%   transitions occur according to tmodel0 and the value of states is given by vf


function v = expectedValue(tmodel0, s0, a0, vf)
    v = 0;

    index = transitionModelGetIndex(s0,a0);
    
    l = length(tmodel0(index).visitedStates);
    
    allp = sum(tmodel0(index).visitedStatesCount);
    
    for i = 1:l
        s1 = tmodel0(index).visitedStates(i);
        if s1
            p = tmodel0(index).visitedStatesCount(i) / allp;
            v = v + (p * vf(s1));
        end
    end
     
end
