%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% CS221 Programming Assignment 3
%%   Chris Archibald, Oct. 2009
%%   Stanford University
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Learns a transition model using the simulator() function.  Saves this 
% learned transition model to the MyTModel.mat file
%
% YOU WILL BE MODIFYING THIS FILE
%


function learnTransitionModel()

global EXPLORATION_RUNS;
global DISCRETE_ACTION_COUNT;

%Seed the random number generator so we 
%all have consistent and debuggable results
rand('state', 221);

setVars();

% To avoid running something for many hours, only to find 
% out it is wrong, we have provided debug information for
% you.  Once you have implemented this file, and you think
% it is correct, run with EXPLORATION_RUNS = 10, and then
% load your transition model and pass it into testFirstTen()
% This will see if your transition model is correct.
% If it is, you can then change EXPLORATION_RUNS to 1000000 
% and run it for real.  If your model is not correct, then 
% you can debug now, without loss of your time.
%EXPLORATION_RUNS = 1000000;
EXPLORATION_RUNS = 10;


%Create the transition model
tmodel = transitionModelCreate();

%Timing info (for fun)
tic

%  --------------------------------
%  4.1 EXPLORATION
%  --------------------------------
%  Explore to build up a model

for i=1:EXPLORATION_RUNS
    
    if mod(i,1000) == 0
        %Uncomment to see incremental progress
        %i  
    end

    s0 = getRandomState();
    s0_disc = discretizeState(s0);
    
    for a = 1:DISCRETE_ACTION_COUNT
        action = undiscretizeAction(a);
        
        s1 = simulate(s0, action);
        s1_disc = discretizeState(s1);
        
        tmodel = transitionModelAddCount(tmodel, s0_disc, a, s1_disc);
    end
    
end

toc

%Save transition model 
save 'MyTModel' tmodel;

display('Finished exploring.');

end
