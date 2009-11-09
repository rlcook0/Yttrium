%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% CS221 Programming Assignment 3
%%   Chris Archibald, Oct. 2009
%%   Stanford University
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Returns the reward corresponding to a given discrete state 
% The basic reward penalizes for moving farther down the road (i.e.,
% positive X velocity), or for moving away from the center-line.  It
% gives a reward for facing and driving in the opposite direction
% (i.e., facing down the negative X axis, and large negative X
%
% Input:
% - si is the discrete state
%
% Output:
% - r is the reward corresponding to this state



function r = reward(si)
  global STATE_Y;
  global STATE_VX;
  global STATE_VY;
  global STATE_THETA;
  
  s = undiscretizeState(si);
  
  %Stay near center line
  yReward = -s(STATE_Y)*s(STATE_Y);
  
  vxReward = -0.1*s(STATE_VX);
  
  if s(STATE_VX) < 0
      vxReward = s(STATE_VX)*s(STATE_VX)*2;
  end
  
  vyReward = -s(STATE_VY)*s(STATE_VY);
  
  headingReward = 0;
  
  r = 2*yReward + vxReward + 0.1*vyReward + 5*headingReward;

end
