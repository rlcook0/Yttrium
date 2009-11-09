%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% CS221 Programming Assignment 3
%%   Chris Archibald, Oct. 2009
%%   Stanford University
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Returns a continuous state corresponding to a given
% discrete state
% 
% Input: 
% - si is the discrete state
%
% Output: 
% - state is a continuous state corresponding to si.
%   it will be a vector of length STATE_SIZE

function state = undiscretizeState(si)

    global Y_COUNT;
    global VX_COUNT;
    global VY_COUNT;
    global THETA_COUNT;
    global DISCRETE_STATE_COUNT;
    global STATE_Y;
    global STATE_VX;
    global STATE_VY;
    global STATE_THETA;
    global Y_LIMIT;
    global VX_LIMIT;
    global VY_LIMIT;
    global THETA_LIMIT;
    
    si = min(max(si,1),DISCRETE_STATE_COUNT);
    
    yi = mod(si-1,Y_COUNT) + 1;
    vxi = mod(ceil(si/Y_COUNT)-1, VX_COUNT) + 1;
    vyi = mod(ceil(si/(Y_COUNT*VX_COUNT))-1, VY_COUNT) + 1;
    thetai = ceil(si/(Y_COUNT*VX_COUNT*VY_COUNT));
    
    state(STATE_THETA) = (thetai - 0.5)*2*THETA_LIMIT/THETA_COUNT -THETA_LIMIT;
    state(STATE_VY) = (vyi-0.5)*2*VY_LIMIT/VY_COUNT - VY_LIMIT;
    state(STATE_VX) = (vxi-0.5)*2*VX_LIMIT/VX_COUNT - VX_LIMIT;
    state(STATE_Y) = (yi-0.5)*2*Y_LIMIT/Y_COUNT - Y_LIMIT;

end
