%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% CS221 Programming Assignment 3
%%   Chris Archibald, Oct. 2009
%%   Stanford University
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Returns the discrete action corresponding to a given discrete steering index and 
% wheel velocity index
%
% Input:
% - steeri is the index of the discrete steering direction
% - wheelveli is the index of the discrete wheel velocity
%
% Output:
% - actionIndex is the descretized Action, a number between 1 and DISCRETE_ACTION_COUNT



function actionIndex = getActionIndex(steeri, wheelveli)
  
    global STEERING_COUNT;
    global WHEEL_VEL_COUNT;
    
    
    steeri = min(max(steeri, 1), STEERING_COUNT);
    wheelveli = min(max(wheelveli, 1), WHEEL_VEL_COUNT);
    
    actionIndex = (wheelveli-1)*STEERING_COUNT + steeri;
end
