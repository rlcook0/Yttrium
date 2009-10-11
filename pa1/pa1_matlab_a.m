% CS221  Fall 2009  Stanford University
% Programming Assignment #1
%% Section 1. Read in world from configuration file

addPaths();
% Select one of these configuration files to load.
world0 = 'sphereWorld0.xml';
world1 = 'sphereWorld1.xml';
world2 = 'sphereWorld2.xml';

World = readAndDisplayWorld(world0);
% World will contain members World.robot, World.goal, and World.obstacles.
% World.robot.Configuration specifies the robot's starting configuration,
% and World.goal.Configuration specifies the desired goal configuration of
% the robot. World.obstacles is a cell array, each member of which
% describes a spherical obstacle via its center and radius.

%% Section 2. Compute and draw potential field

Field = getAndDrawPotentialField(World);
% Gets and draws the value of the potential field. Field has members Xs and
% Ys which define the sample points, and Values which is the matrix of
% values at the sample points.
%
% *** YOU MUST IMPLEMENT ***
%   getPotentialValue(World,config)
%
% The above function calls getPotentialValue(World,config) in its inner
% loop to assemble the value matrix. You must implement this function to
% return a potential value for every configuration. The potential field
% must be zero at the goal, infinite at invalid positions, and serve to
% stear the object away from obstacles, as well.

%% Section 3. Run potential field search and draw resulting path.

path = potentialFieldSearch(World,Field);
% Executes a search for the goal by looking for the point on an eight-point
% grid around the current position which has the lowest potential cost. Has
% a maximum limit of 1000 steps, after which, if the robot has not arrived
% at the goal, it assumes it is in a local minima and exits.
% 
% Draws the resulting path on both the world and the potential field.

