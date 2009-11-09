%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% CS221 Programming Assignment 3
%%   Chris Archibald, Oct. 2009
%%   Stanford University
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Returns the continuous state that results from taking a specified
% continuous action from a specified given state.  This function 
% Simulates the dynamics of the car (in a fairly hacked manner)
%
% Input:
% - s0 is the continuous initial state
% - action is the continuous action applied
%
% Output:
% - s1 is the continuous state that results (one time step of DT later)
%
% DO NOT CHANGE THIS FILE.  Doing so could change the way that the car behaves, which 
% would change the whole assignment


function s1 = simulate(s0, action)
 
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
    
    s1=s0;
    
    steeringAngle = action(ACTION_STEERING);
    driveWheelVel = action(ACTION_WHEEL_VEL);
    
    if steeringAngle > MAX_STEERING
        steeringAngle = MAX_STEERING;
    end
    if steeringAngle < -MAX_STEERING
        steeringAngle = -MAX_STEERING;
    end
    if driveWheelVel < 0
        driveWheelVel = 0; %Brakes
    end
    
    bodyVel = getBodyVelocity(s0);
    
    frontWheelVel = rotate2d(-steeringAngle, bodyVel);
    
    af_prll = 0;
    af_perp = -frontWheelVel(2) / DT;
    af_norm = abs(af_perp);
    if af_norm > G*MU_S_FRONT
        af_perp = G*MU_D_FRONT * (af_perp / af_norm);
    end
    
    ar_prll = (driveWheelVel - bodyVel(1)) / DT;
    ar_perp = -bodyVel(2) / DT;
    ar_norm = sqrt(ar_perp*ar_perp + ar_prll*ar_prll);
    
    if ar_norm > G*MU_S_BACK
        ar_perp = ar_perp * G*MU_D_BACK / ar_norm;
        ar_prll = ar_prll * G*MU_D_BACK / ar_norm;
    end
    
    %Integrate velocities to tires. . .
    frontWheelVel(1) = frontWheelVel(1) + (af_prll*DT);
    frontWheelVel(2) = frontWheelVel(2) + (af_perp*DT);
    bodyVel(1) = bodyVel(1) + (ar_prll *DT);
    bodyVel(2) = bodyVel(2) + (ar_perp * DT);
    
    frontWheelBodyVel = rotate2d(steeringAngle, frontWheelVel);
    frontPos(1) = L + (frontWheelBodyVel(1)*DT);
    frontPos(2) = frontWheelBodyVel(2)*DT;
    
    backPos = bodyVel.*DT;
    
    newTheta = atan2(frontPos(2) - backPos(2), frontPos(1) - backPos(1));
    
    %Update next state
    v1 = rotate2d(s0(STATE_THETA), bodyVel);
    s1(STATE_VX:STATE_VY) = v1;
    s1(STATE_Y) = s0(STATE_Y) + s1(STATE_VY)*DT;
    s1(STATE_THETA) = s0(STATE_THETA) + newTheta;

end
