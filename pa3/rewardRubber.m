%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% CS221 Programming Assignment 3
%%   Chris Archibald, Oct. 2009
%%   Stanford University
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Returns the reward corresponding to a given discrete state
% This is a different reward that encourages the car to drive
% erratically.  Specifically, this returns reward(state), plus a term
% that rewards sideways motion.
%
% Input:
% - si is the discrete state
%
% Output:
% - r is the reward corresponding to this state



function r = rewardRubber(si)
  s = undiscretizeState(si);

  v = getBodyVelocity(s);

  r = reward(si) + v(2)*v(2)*4;
end
