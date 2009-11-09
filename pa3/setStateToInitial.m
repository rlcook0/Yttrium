%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% CS221 Programming Assignment 3
%%   Chris Archibald, Oct. 2009
%%   Stanford University
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Returns the intitial state used in this programming assignment
%
% Output:
% - astate is the initial state


function astate = setStateToInitial()

  global STATE_SIZE;
  global STATE_VX;
  
  astate = zeros(1,STATE_SIZE);
  astate(STATE_VX) = 10;
end
