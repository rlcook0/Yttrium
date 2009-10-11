function World = drawWorld(World)
%Draw a world.
%  drawWorld(WORLD) draws the world.

% Draw obstacles
obstacles = World.obstacles;
angs = 0:pi/16:2*pi;
for i=1:length(obstacles.Radii)
    pts = bsxfun(@plus, obstacles.Radii(i) * [cos(angs);sin(angs)], ...
        obstacles.Centers(:,i));
    % Draw filled points.
    fill(pts(1,:),pts(2,:),0.3*[1 1 1]);
end

% Draw robot
World.robot = drawRobot(World.robot);

% Set drawing bounds
axis(World.axis);
