%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% CS221 Programming Assignment 3
%%   Chris Archibald, Oct. 2009
%%   Stanford University
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Returns the body velocity for a given state
% This is a function that is used by the simulator internally


function va = getBodyVelocity(state)
  global STATE_VX;
  global STATE_VY;
  global STATE_THETA;
  
  v0 = state(STATE_VX:STATE_VY);
  va = rotate2d(-state(STATE_THETA),v0) ;
end
