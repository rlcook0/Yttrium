function World = readAndDisplayWorld(filename)
%Reads in a world and displays it.
%  WORLD = readAndDisplayWorld(FILENAME) reads in and returns WORLD, which
%  has fields populated for the robot, goal, and obstacles. Also displays
%  in figure 1 the setup of the world, with the robot's starting
%  configuration in blue, and its goal configuration in red.

% Read in world
World = readWorld(filename);

% Capture robot starting configuration
StartingConfig = World.robot.Configuration;

% Draw world in starting configuration.
figure(1), clf, hold on, grid on, axis equal;
% Draw robot's goal configuration in red.
World.robot.Color = [1 0 0];
World.robot.Configuration = World.goal.Configuration;
drawWorld(World);
% Draw robot's starting configuration in blue.
World.robot.Color = [0 0 1];
World.robot.Configuration = StartingConfig;
drawRobot(World.robot);
title('The world with starting (blue) and goal (red) configurations');

