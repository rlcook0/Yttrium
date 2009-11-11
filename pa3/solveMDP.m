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
    NUM_ITERATIONS = 60;
    
    % Initialize the value function
    prevValueFunction = zeros(1,DISCRETE_STATE_COUNT);
    valueFunction = zeros(1,DISCRETE_STATE_COUNT);
    % and the optimal policy
    pistar = zeros(1,DISCRETE_STATE_COUNT);
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    %
    % YOUR CODE HERE:  
    % 4.2 Implement the (synchronous) value iteration algorithm
    %
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    % For every state s belonging to S, update
    %   V(s) := R(s) + max gamma sum Ps(s')V(s')
    
    for i = 1:NUM_ITERATIONS
        curr_iteration = i
        for s = 1:DISCRETE_STATE_COUNT
            
            % calculate the maximum expected value over all actions from
            % this state
            max_ev = -inf;
            for a = 1:DISCRETE_ACTION_COUNT
                ev = expectedValue(tmodel, s, a, prevValueFunction);
                if (ev > max_ev)
                    max_ev = ev;
                end
            end
            
            valueFunction(s) = rewardRubber(s) + (max_ev * GAMMA);
        end
        
        % now that synchronous updates are complete, set prevValueFunction
        % to valueFunction
        prevValueFunction = valueFunction;
    end
    
    s0 = setStateToInitial();
    s0_disc = discretizeState(s0);
    
    % print out V(s0)
    valueFunction(s0_disc)
    
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
    
    for s=1:DISCRETE_STATE_COUNT
        max_sum = -inf;
        max_a   = 0.0;
        
        for a=1:DISCRETE_ACTION_COUNT
            sum = expectedValue(tmodel, s, a, valueFunction);
            if (sum > max_sum)
                max_sum = sum;
                max_a = a;
            end 
        end
        
        pistar(s) = max_a;
    end
    
    
    % Save the policy and value function
    vf = valueFunction; %vf should be the final value function
    
    %Change this filename to avoid overwriting  previous policies
    save 'OptimalPolicy6' pistar vf;
    
    display('Optimal Policy 6 saved');

end
