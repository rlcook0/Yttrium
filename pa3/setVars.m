%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% CS221 Programming Assignment 3
%%   Chris Archibald, Oct. 2009
%%   Stanford University
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% This function initializes all of the global variables that
% are used throughout the assignment.  Check here to see 
% what the value of something is
%
% DO NOT CHANGE ANY OF THESE VALUES, as it might break things


function setVars()

    global STATE_Y;
    global STATE_VX;
    global STATE_VY;
    global STATE_THETA;
    
    global ACTION_STEERING;
    global ACTION_WHEEL_VEL;
    
    global MAX_STEERING;
    global DT;
    
    global MU_S_BACK;
    global MU_D_BACK;
    global MU_S_FRONT;
    global MU_D_FRONT;
    global G;
    global L;
    
    global STATE_SIZE;
    global ACTION_SIZE;
    
    global Y_COUNT;
    global VX_COUNT;
    global VY_COUNT;
    global THETA_COUNT;
    
    global DISCRETE_STATE_COUNT;
    
    global Y_LIMIT;
    global VX_LIMIT;
    global VY_LIMIT;
    global THETA_LIMIT;
    
    global STEERING_COUNT;
    global WHEEL_VEL_COUNT;
    
    global DISCRETE_ACTION_COUNT;
    
    global STEERING_ANGLES;
    global WHEEL_VELOCITIES;
    global STEERING_ANGLE_CUTOFFS;
    global WHEEL_VELOCITY_CUTOFFS;
    
    STATE_Y = 1;
    STATE_VX = 2;
    STATE_VY = 3;
    STATE_THETA = 4;
    
    ACTION_STEERING = 1;
    ACTION_WHEEL_VEL = 2;
    
    MAX_STEERING = 1.0;
    DT = 0.25;
    
    MU_S_BACK = 0.9;
    MU_D_BACK = 0.3;
    MU_S_FRONT = 1.2;
    MU_D_FRONT = 0.8;
    G = 9.81;
    L = 1.5;
    
    STATE_SIZE = 4;
    ACTION_SIZE = 2;
    
    Y_COUNT = 10;
    VX_COUNT = 10;
    VY_COUNT = 10;
    THETA_COUNT = 30;
    
    DISCRETE_STATE_COUNT = Y_COUNT*VX_COUNT*VY_COUNT*THETA_COUNT;
    
    Y_LIMIT = 20;
    VX_LIMIT = 10;
    VY_LIMIT = 6;
    THETA_LIMIT = pi;
    
    STEERING_COUNT = 7;
    WHEEL_VEL_COUNT = 5;
    
    DISCRETE_ACTION_COUNT = STEERING_COUNT*WHEEL_VEL_COUNT;
    
    STEERING_ANGLES = [-MAX_STEERING -MAX_STEERING*0.5 -MAX_STEERING*0.25 0 MAX_STEERING*0.25 MAX_STEERING*0.5 MAX_STEERING];
    WHEEL_VELOCITIES = [0 1 3 5 10];
    STEERING_ANGLE_CUTOFFS = [-MAX_STEERING*0.75 -MAX_STEERING*0.375 -MAX_STEERING*0.125 MAX_STEERING*0.125 MAX_STEERING*0.375 MAX_STEERING*0.75 MAX_STEERING];
    WHEEL_VELOCITY_CUTOFFS = [0.5 2 4 7.5 10];
end
