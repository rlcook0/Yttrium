% CS221  Fall 2009  Stanford University
% Programming Assignment #1
%% Section 1. Read in world from configuration file

addPaths();
% Select one of these configuration files to load.
world0 = 'sphereWorld0.xml';
world1 = 'sphereWorld1.xml';
world2 = 'sphereWorld2.xml';
world3 = 'sphereWorld3.xml';
world4 = 'armWorld1.xml';
world5 = 'armWorld2.xml';

World = readAndDisplayWorld(world3);
% World will contain members World.robot, World.goal, and World.obstacles.
% World.robot.Configuration specifies the robot's starting configuration,
% and World.goal.Configuration specifies the desired goal configuration of
% the robot. World.obstacles is a cell array, each member of which
% describes a spherical obstacle via its center and radius.

%% Section 2. Get configuration space points and compute connectivity

useFilePoints = true;
m = 30;
World = getAndDisplayConfigSpace(World,useFilePoints,m);
% Adds members ConfigPoints and Connectivity to World. ConfigPoints is a
% matrix with individual configuration space points along columns. The
% first column will always contain the robot's starting configuration, and
% the last column will contain the goal configuration.
%
% While ConfigPoints is an n-by-m matrix, Connectivity is an m-by-m sparse
% matrix with values 1 at (i,j) and (j,i) only if the configuration space
% points i and j are mutually visible.
% 
% If useFilePoints is true, the functions uses the configuration space
% points from the file; if false, it generates m random points.
%
% For spherical robots, this function also draws the configuration space
% points as blue stars, with the mutually visible ones connected by green
% lines.

%% Section 3. Run all searches and draw recovered paths

[paths,pathNames] = runSearchesDisplayResults(World);
% Runs the four PRM-based searches on the given world and displays their
% results in a four-tile figure, each showing one of the paths.
%
% *** YOU MUST IMPLEMENT ***
%   bestFirstSearch(World)
%   AstarSearch(World)
%   uniformCostSearch(World)
%   AstarInadmissibleSearch(World)
% You must implement the four above search functions for the above function
% to work. Each search must return a path through the given configuration
% space (World.ConfigPoints) leading from the starting configuration
% (World.ConfigPoints(:,1)) to the goal (World.ConfigPoints(:,end)).


%% Section 4. Animate recovered paths

% % This will animate each path sequentially on a single figure, pausing
% % between each.
% figure(2), clf;
% for i=1:length(paths)
%     animatePath(World, paths{i}, pathNames{i});
%     if i < length(paths)
%         pause;
%     end
% end
%  
