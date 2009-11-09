%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% CS221 Programming Assignment 3
%%   Chris Archibald, Oct. 2009
%%   Stanford University
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Rotates a given vector by the specified angle
% This is a function that is used by the simulator internally

function va = rotate2d(t,v0)

  va = v0;

  ct = cos(t);
  st = sin(t);
  
  va(1) = ct*v0(1) - st*v0(2);
  va(2) = st*v0(1) + ct*v0(2);

end
