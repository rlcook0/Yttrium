%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% CS221 Programming Assignment 3
%%   Chris Archibald, Oct. 2009
%%   Stanford University
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Displays the effect of controlling the car using a given policy
%
% Input:
% - pistar is the policy that will drive the car
%
% Comments: The car is displayed as an elongated triangle, with the point showing the 
%           direction that the car is facing.  The location of the car at each time 
%           step is overlayed on top of each other so that from the last plot (which
%           you will turn in) we can tell basically what the car did. 
%           It is currently set to run for a constant number of time steps, feel
%		    free to adjust this for your specific car if it is not set high enough.
%


function runPolicy(pistar)

    global STATE_VX;
    global STATE_VY;
    global STATE_THETA;
    global DT;
    
    %Run the simulation to its conclusion, save all of the data
    
    if nargin == 0
	 display('Please input a policy.');
	 return;
    end

    setVars();
    
    s = setStateToInitial();
    states = zeros(100,4);
    xs = [];
    ys = [];
    
    carx = 0;
    cary = 0;
    
    display('Running simulation');
    
    for t=1:100
        states(t,:) = s;
        si = discretizeState(s);
        ai = pistar(si);
        a = undiscretizeAction(ai);
        
        s = simulate(s,a)
        
        carx = carx + s(STATE_VX)*DT
        cary = cary + s(STATE_VY)*DT
        xs(t) = carx;
        ys(t) = cary;
        
    end
    
    display('Displaying simulation');
    
    %Display the simulation data in a window

    clf; %Clear the figure
    plot(0,0);
    hold on;
    
    %Modify the time if the car does not have long enough to finish
    %its maneuver
    for t=1:100
        showCar(xs(t),ys(t),states(t,STATE_THETA)*180/pi);
        pause(0.25);
    end

end
