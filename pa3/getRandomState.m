%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% CS221 Programming Assignment 3
%%   Chris Archibald, Oct. 2009
%%   Stanford University
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Returns a random continuous state
%
% Output:
% - s is a random continuous state, where each state component is valid, as defined by the *_LIMIT constants


function s = getRandomState()

  global STATE_SIZE;
  global STATE_Y;
  global STATE_VX;
  global STATE_VY;
  global STATE_THETA;
  global Y_LIMIT;
  global VX_LIMIT;
  global VY_LIMIT;
  global THETA_LIMIT;
  
  
  s = rand(1,STATE_SIZE);
  
  s(STATE_Y) = 2*Y_LIMIT * s(STATE_Y) - Y_LIMIT;
  s(STATE_VX) = 2*VX_LIMIT * s(STATE_VX) - VX_LIMIT;
  s(STATE_VY) = 2*VY_LIMIT * s(STATE_VY) - VY_LIMIT;
  s(STATE_THETA) = 2*THETA_LIMIT * s(STATE_THETA) - THETA_LIMIT;

end
