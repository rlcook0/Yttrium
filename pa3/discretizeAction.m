%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% CS221 Programming Assignment 3
%%   Chris Archibald, Oct. 2009
%%   Stanford University
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Returns the discrete action for a given continuous action
%
% Input:
% - action is a ACTION_SIZE vector 
%
% Output:
% - actionIndex is the discretized Action, a number between 1 and DISCRETE_ACTION_COUNT


function actionIndex = discretizeAction(action)

  global STEERING_COUNT;
  global ACTION_STEERING;
  global STEERING_ANGLE_CUTOFFS;
  global WHEEL_VEL_COUNT;
  global ACTION_WHEEL_VEL;
  global WHEEL_VELOCITY_CUTOFFS;
  
  
  steeri = 0;
  veli = 0;
  
  for steeri = 1:STEERING_COUNT
      if action(ACTION_STEERING) <= STEERING_ANGLE_CUTOFFS(steeri)
          break
      end
  end
  
  for veli = 1:WHEEL_VEL_COUNT
      if action(ACTION_WHEEL_VEL) <= WHEEL_VELOCITY_CUTOFFS(veli)
          break;
      end
  end
  
  
  actionIndex = getActionIndex(steeri,veli);
end
