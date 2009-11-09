%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% CS221 Programming Assignment 3
%%   Chris Archibald, Oct. 2009
%%   Stanford University
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Solves the MDP represented by the input transition model
%
% Input: 
% - tmodel is the transition model, which should have been 
%   saved to a file by learnTransitionModel.m
%
% Output: 
% - pistar is the final policy found. This policy is also saved to a file
%   along with the final value function for later use.
%
% Usage:  To call this function, type 'load MyTModel' on matlab's command
% line (assuming that you didn't change the name of the file to which the 
% transition model is saved). Then call solveMDP(tmodel), where tmodel is the name
% of the saved transition model that was just loaded.  This function will 
% save the optimal policy found and the value function. 
% The policy can be viewed in action by typing 'load OptimalPolicy' on the command line
% and then calling 'runPolicy(pistar)'. 
%
%
% YOU WILL BE MODIFYING THIS FILE
%


function pistar = solveMDP(tmodel)

    global DISCRETE_STATE_COUNT;
    global DISCRETE_ACTION_COUNT;
 
    %Check for transition model
    if nargin == 0
	 display('Please input a transition model.');
	 return;
    end
   
    setVars();
    
    %  --------------------------------
    %  4.2 SOLVE THE MDP
    %  --------------------------------
    %  Run value iteration to find the value of each state
    
    GAMMA = 0.8;
    
    % Initialize the value function
    prevValueFunction = zeros(1,DISCRETE_STATE_COUNT);
    % and the optimal policy
    pistar = zeros(1,DISCRETE_STATE_COUNT);
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    %
    % YOUR CODE HERE:  
    % 4.2 Implement the (synchronous) value iteration algorithm
    %
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    display('Solved the MDP');
    
    %  --------------------------------
    %  4.3 Compute Optimal Policy
    %  --------------------------------
    %  Use your computed value function from above to compute the best action
    %  for every state
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    %
    % YOUR CODE HERE:  
    % 4.3 Compute the optimal policy pistar
    %
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    
    % Save the policy and value function
    vf = prevValueFunction; %vf should be the final value function
    
    %Change this filename to avoid overwriting  previous policies
    save 'OptimalPolicy' pistar vf;
    
    display('Optimal Policy saved');

end
