%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% CS221 Programming Assignment 3
%%   Chris Archibald, Oct. 2009
%%   Stanford University
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Creates a transition model that can be used to store
% transition probabilities
%
% A transition model is an array of DISCRETE_STATE_COUNT*DISCRETE_ACTION_COUNT
% structs, each of which contains two arrays: visitedStates and
% visitedState
%    - visitedStates is an array which stores all of the discrete states that have
%      been seen after taking the action in the state corresponding to this entry in the 
%      transition model
%    - visitedStatesCount is an array which stores the number of times that each state
%      has been visited 
% Example:  tmodel(n) stores the discribution over resulting states for
%           some (state,action) pair.  Each n corresponds to a specific
%           (state,action) combination
% Example:  tmodel(12).visitedStatesCount(3) stores the number of times that 
%           tmodel(12).visitedStates(3) (which is a discrete state) was seen as a result
%           of taking the discrete action in the discrete state which corresponds to 12
%
%
%
% Output:
%  - tmodel is the created transition model


function tmodel = transitionModelCreate()

    global DISCRETE_STATE_COUNT;
    global DISCRETE_ACTION_COUNT;
    
    %Create a transition model
    
    num = DISCRETE_STATE_COUNT*DISCRETE_ACTION_COUNT;
    
    
    tmodel = repmat(struct('visitedStates', zeros(1,3), 'visitedStatesCount', zeros(1,3)), 1, num);

end
