function World = readWorld(filename)
%Read in world information from an XML file.
%  WORLD = readWorld(FILENAME) reads in the world describes in filename.
%  Populates WORLD.robot, WORLD.goal, and WORLD.obstacles members.

% Read in world XML configuration file.
World = parseXML(filename);
% Parse specific robot, goal, and obstacles nodes.
robot = getChildNode(World,'Robot');
goal = getChildNode(World,'Goal');
obstacles = getChildNode(World,'Obstacles');
axises = getChildNode(World,'Axis');
% Convert string parameters to numeric parameters where necessary
robot.Configuration = getnumbers(robot.Configuration);
if strcmp(robot.Type,'Sphere')
    robot.Radius = getnumbers(robot.Radius);
elseif strcmp(robot.Type,'Arm')
    robot.Lengths = getnumbers(robot.Lengths);
    % Convert angle configuration to radians
    robot.Configuration = robot.Configuration / 180 * pi;
end
goal.Configuration = getnumbers(goal.Configuration);
if strcmp(robot.Type,'Arm')
    % Convert angle configuration to radians
    goal.Configuration = goal.Configuration / 180 * pi;
end
obstacles = obstacles.children;
obs.Centers = zeros(2,length(obstacles));
obs.Radii = zeros(1,length(obstacles));
for i=1:length(obstacles)
    obs.Centers(:,i) = getnumbers(obstacles{i}.Center);
    obs.Radii(i) = getnumbers(obstacles{i}.Radius);
end
obstacles = obs;
% Add members to World
World.robot = robot;
World.goal = goal;
World.obstacles = obstacles;
World.axis = getnumbers(axises.Inline);

