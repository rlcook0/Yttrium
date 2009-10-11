function robot = computeArmPoints(robot)
%Compute points of the joints in an robot.
%  ROBOT = computeArmPoints(ROBOT) populates the Points member of ROBOT
%  with the display points.

n = length(robot.Lengths);
P = zeros(2,n+1);
P(:,1) = [0;0];
P(1,2:n+1) = cumsum(robot.Lengths);
% Apply thetas
for i=1:n
    ang = robot.Configuration(i);
    R = [cos(ang) -sin(ang); sin(ang) cos(ang)];
    T = bsxfun(@minus, P(:,1+i:end), P(:,i));
    P(:,1+i:end) = bsxfun(@plus, R*T, P(:,i));
end
robot.Points = P;
