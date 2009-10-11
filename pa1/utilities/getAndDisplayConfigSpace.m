function World = getAndDisplayConfigSpace(World,useFilePoints,n)
%Computes configuration space points and connectivity, and displays.
%  World = getAndDisplayConfigSpace(World,useFilePoints,n).

% Fills the ConfigSpace member of World.
World = populateConfigPoints(World,useFilePoints,n);
StartingConfig = World.robot.Configuration; % Capture starting config

% Display world with start and end configurations
figure(1), clf, hold on, grid on, axis equal;
% Display connectivity for sphere-type robot (no good visualization
% available for arm kind).
if strcmp(World.robot.Type,'Sphere')
    [i,j] = find(triu(World.Connectivity));
    for ii=1:length(i)
        plot(World.Landmarks(1,[i(ii) j(ii)]),...
             World.Landmarks(2,[i(ii) j(ii)]),'g-');
    end
    plot(World.Landmarks(1,:),World.Landmarks(2,:),'b*');
end

% Draw robot goal configuration in red.
World.robot.Color = [1 0 0];
World.robot.Configuration = World.goal.Configuration;
drawWorld(World);
% Draw robot starting configuration in blue.
World.robot.Color = [0 0 1];
World.robot.Configuration = StartingConfig;
drawRobot(World.robot);
title('The world with starting (blue) and goal (red) configurations');