%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% CS221 Programming Assignment 3
%%   Chris Archibald, Oct. 2009
%%   Stanford University
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Returns the probability of a specific discrete state transition, 
% given the input action
%
% Input:
% - t is the transition model
% - s0 is the initial discrete state
% - a0 is the discrete action taken
% - s1 is the end discrete state
%
% Output: 
% - p the probability, according to transition model t, that taking 
%   discrete action a0 in discrete state s0 will lead to discrete state s1


function p = transitionModelProbability(t, s0, a0, s1)  
    p = 0;
    index = transitionModelGetIndex(s0,a0);
    
    
    i = find(t(index).visitedStates == s1);
    if isempty(i)
        This state was never before seen
        p = 0;
    else
        p =  t(index).visitedStatesCount(i)/sum(t(index).visitedStatesCount);
    end

end
