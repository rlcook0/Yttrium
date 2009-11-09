%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% CS221 Programming Assignment 3
%%   Chris Archibald, Oct. 2009
%%   Stanford University
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Returns a continuous action corresponding to a given
% discrete action
% 
% Input: 
% - actionIndex is the discrete action
%
% Output: 
% - action is a continuous action corresponding to actionIndex.
%   it will be a vector of length ACTION_SIZE


function action = undiscretizeAction(actionIndex)
    global DISCRETE_ACTION_COUNT;
    global STEERING_COUNT;
    global ACTION_SIZE;
    global STEERING_ANGLES;
    global ACTION_STEERING;
    global ACTION_WHEEL_VEL;
    global WHEEL_VELOCITIES;
    
    
    actionIndex = min(max(actionIndex, 1), DISCRETE_ACTION_COUNT);
    
    action = zeros(1,ACTION_SIZE);
    
    wheelveli = ceil(actionIndex/STEERING_COUNT);
    steeri = mod(actionIndex-1, STEERING_COUNT) + 1;
    
    action(ACTION_STEERING) = STEERING_ANGLES(steeri);
    action(ACTION_WHEEL_VEL) = WHEEL_VELOCITIES(wheelveli);

end
