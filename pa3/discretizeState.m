%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% CS221 Programming Assignment 3
%%   Chris Archibald, Oct. 2009
%%   Stanford University
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Returns the discrete state for a given continuous state
%
% Input:
% - state is a STATE_SIZE vector 
%
% Output:
% - si is the descretized state, a number between 1 and DISCRETE_STATE_COUNT


function si = discretizeState(state)

    global Y_COUNT;
    global VX_COUNT;
    global VY_COUNT;
    global THETA_COUNT;
    
    global STATE_Y;
    global STATE_VX;
    global STATE_VY;
    global STATE_THETA;
    
    global Y_LIMIT;
    global VX_LIMIT;
    global VY_LIMIT;
    global THETA_LIMIT;
    
    y = ceil(Y_COUNT*(state(STATE_Y) + Y_LIMIT)/(2*Y_LIMIT));
    vx = ceil(VX_COUNT*(state(STATE_VX) + VX_LIMIT)/(2*VX_LIMIT));
    vy = ceil(VY_COUNT*(state(STATE_VY) + VY_LIMIT)/(2*VY_LIMIT));
    theta = ceil(THETA_COUNT*(wrapToPi(state(STATE_THETA)) + THETA_LIMIT)/(2*THETA_LIMIT));
    
    si = getStateIndex(y,vx,vy,theta);
end
