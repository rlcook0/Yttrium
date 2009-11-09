%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% CS221 Programming Assignment 3
%%   Chris Archibald, Oct. 2009
%%   Stanford University
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Adds an observed transition to the given transition model
% 
% Input: 
% - tmodel1 is the transition model 
% - s0 is the discrete initial state
% - a0 is the discrete action taken
% - s1 is the discrete resulting state
%
% Output: 
% - tmodel1 is the transition model, with this new count added
%   to account for the transition from s0 to s1 after taking 
%   action a0
%
% Usage: 
%    tmodel = transitionModelAddCount(tmodel,s0,a0,s1)
%
%    CAUTION: Make sure that you update your transition model
%             with the output of this function, otherwise
%             none of the counts will actually persist


function tmodel1 = transitionModelAddCount(tmodel1, s0, a0, s1)

    index = transitionModelGetIndex(s0,a0);

    %Find the index of this state and any empty spots
    i = find(tmodel1(index).visitedStates == s1,1);
    j = find(tmodel1(index).visitedStates == 0,1);
    
    if isempty(i)
        %This state has not been visited before
        if isempty(j)
            %There are no more preallocated spots
            %Must append this state to the list of visitedStates
            tmodel1(index).visitedStates(end + 1) = s1;
            tmodel1(index).visitedStatesCount(end + 1) = 1;
        else
            %Use the first preallocated spot
            ji = min(j);
            tmodel1(index).visitedStates(ji) = s1;
            tmodel1(index).visitedStatesCount(ji) = 1;
        end
    else
        %This state has been visited before. Add one to its count
        tmodel1(index).visitedStatesCount(i) = tmodel1(index).visitedStatesCount(i) + 1;
    end
    

end
