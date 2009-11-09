%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% CS221 Programming Assignment 3
%%   Chris Archibald, Oct. 2009
%%   Stanford University
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Returns the discrete state corresponding to a given discrete y value, discrete vx value, discrete
% vy value and discrete theta value 
%
% Input:
% - yi is the index of the discrete y value
% - vxi is the index of the discrete vx value
% - vyi is the index of the discrete vy value
% - thetai is the index of the discrete theta value
%
% Output:
% - si is the discretized state, a number between 1 and DISCRETE_STATE_COUNT



function si = getStateIndex(yi,vxi,vyi,thetai)

    global Y_COUNT;
    global VX_COUNT;
    global VY_COUNT;
    global THETA_COUNT;
    global DISCRETE_STATE_COUNT;
    
    yi = min(max(yi,1), Y_COUNT);
    vxi = min(max(vxi,1), VX_COUNT);
    vyi = min(max(vyi,1), VY_COUNT);
    thetai = min(max(thetai,1), THETA_COUNT);
    
    si = ((thetai-1)*Y_COUNT*VX_COUNT*VY_COUNT) + ((vyi-1)*Y_COUNT*VX_COUNT) + ((vxi-1)*Y_COUNT) +yi;
    
end
