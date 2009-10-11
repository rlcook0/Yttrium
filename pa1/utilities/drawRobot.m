function robot = drawRobot(robot)
%Draw a robot.
%  ROBOT = drawRobot(ROBOT) draws the robot and returns a modified version
%  of the ROBOT with a Points member containing the plotted points.

    if strcmp(robot.Type,'Sphere')
        angs = 0:pi/16:2*pi;
        robot.points = bsxfun(@plus, robot.Radius(1) * [cos(angs);sin(angs)], ...
            robot.Configuration(:));
        % Draw filled points
        if isfield(robot,'Color')
            colour = robot.Color;
        else
            colour = [0 0 1];
        end
        fill(robot.points(1,:),robot.points(2,:),colour);
    elseif strcmp(robot.Type,'Arm')
        robot = plotArm(robot);
    end
end

