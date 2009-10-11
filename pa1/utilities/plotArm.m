function robot = plotArm(robot)
%Plot a multi-joint robot arm.
%  ROBOT = plotArm(ROBOT) plots the arm and returns a modified version of
%  the arm with the Points member populated.

n = numel(robot.Lengths);
robot = computeArmPoints(robot);
if isfield(robot,'Color')
    colour = robot.Color;
else
    colour = 0.5*[1 1 1];
end
% Draw segments
for i=1:n
    plot(robot.Points(1,i+[0 1]),robot.Points(2,i+[0 1]),'-',...
        'Color',colour,'LineWidth',2);
end
% Draw base and joints
plot(0,0,'.','Color',colour,'MarkerSize',50,'MarkerFaceColor',colour);
for i=2:n
    plot(robot.Points(1,i),robot.Points(2,i),'ko',...
        'MarkerSize',6,'MarkerFaceColor',colour);
end
% Draw grip at end
plot(robot.Points(1,end),robot.Points(2,end),'ko',...
    'MarkerSize',6,'MarkerFaceColor',colour);

