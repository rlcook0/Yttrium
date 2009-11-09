%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% CS221 Programming Assignment 3
%%   Chris Archibald, Oct. 2009
%%   Stanford University
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Returns the index into a transitionModel for a given discrete state and action
%
% Input:
% - s is a discrete state
% - a is a discrete action
%
% Output:
% - i is the index into the transitionModel (a number between 1 and DISCRETE_STATE_COUNT * DISCRETE_ACTION_COUNT


function i = transitionModelGetIndex(s,a)
    global DISCRETE_ACTION_COUNT;
    i = (s-1)*DISCRETE_ACTION_COUNT + a;
end