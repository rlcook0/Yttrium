function conn = computeVisibility(World,configPoints)
%Compute visibility between configuration space points.
%  CONNECTIVITY = computeVisibility(WORLD,CONFIGPOINTS) works for both
%  sphere-type robots and arm-type robots.

    n = size(configPoints,2);
    conn = sparse(n,n);
    % Compute visibility
    Centers = World.obstacles.Centers;
    robot = World.robot;
    if strcmp(robot.Type,'Sphere')
        Radii = World.robot.Radius + World.obstacles.Radii;
    elseif strcmp(robot.Type,'Arm')
        Radii = World.obstacles.Radii;
        % Check if the configuration positions lie within an obstacle
        % first.
        realizable = true(1,n);
        for i=1:n
            robot.Configuration = configPoints(:,i);
            robot = computeArmPoints(robot);
            P = robot.Points;
            for j=1:size(P,2)-1
                if lineCollide(P(:,j),P(:,j+1),Centers,Radii)
                    realizable(i) = false;
                    break;
                end
            end
        end
        % Set number of animation steps for checking arm movement
        % visibility.
        nsteps = 100;
        normThresh = 0.1;
    end
    % Check between all configuration points.
    for i=1:n-1
        for j=i+1:n
            if strcmp(robot.Type,'Sphere')
                % Spherical robot: check collision with travel segment
                if ~lineCollide(configPoints(:,i),configPoints(:,j),...
                    Centers,Radii)
                    % Connect points
                    conn(i,j) = 1;
                    conn(j,i) = 1;
                end
            elseif strcmp(robot.Type,'Arm')
                if ~realizable(i) || ~realizable(j)
                    % Either end position is invalid; no connection.
                    continue;
                end
                % Run arm through range of motion, checking for collisions.
                lastRobot = robot;
                lastRobot.Configuration = configPoints(:,i);
                lastRobot = computeArmPoints(lastRobot);
                dConfig = (configPoints(:,j) - configPoints(:,i)) / nsteps;
                robot.Configuration = configPoints(:,i) + dConfig;
                k = 1;
                shares = 1;
                collide = false;
                while k <= nsteps
                    % Check realizability of this intermediate position.
                    robot = computeArmPoints(robot);
                    P = robot.Points;
                    for m=1:size(P,2)-1
                        if lineCollide(P(:,m),P(:,m+1),Centers,Radii)
                            collide = true;
                            break;
                        end
                        % Check non-collision of end point-to-end point
                        % segments
                        if lineCollide(P(:,m+1),...
                            lastRobot.Points(:,m+1),Centers,Radii)
                            collide = true;
                            break;
                        end
                    end
                    if collide
                        break;
                    end
                    if k >= nsteps
                        break;
                    end
                    % Check if distance between end effector for first and
                    % last positions is greater or less than threshold.
                    d = norm(robot.Points(:,end) - lastRobot.Points(:,end));
                    if d < normThresh
                        % Increase simulation time step
                        shares = min(10,2 * shares);
                    else
                        % Decrease simulation time step
                        shares = max(1, floor(shares/2));
                    end
                    % Step to next position
                    k = k + shares;
                    lastRobot = robot;
                    if k >= nsteps
                        % Set as end position and check
                        robot.Configuration = configPoints(:,j);
                        k = nsteps;
                    else
                        robot.Configuration = robot.Configuration + ...
                            shares * dConfig;
                    end
                    % Plot connectivity test
%                     figure(1), clf, hold on, grid on, axis equal;
%                     W = World;
%                     W.robot.Configuration = configPoints(:,i);
%                     W.robot.Color = [1 0 0];
%                     drawworld(W);
%                     W.robot.Configuration = configPoints(:,j);
%                     W.robot.Color = [0 0 1];
%                     drawworld(W);
%                     W.robot = robot;
%                     W.robot.Color = 0.5*[1 1 1];
%                     drawworld(W);
%                     W.robot = lastRobot;
%                     drawworld(W);
%                     pause(0.01);
                end
                if ~collide
                    % Connect points
                    conn(i,j) = 1;
                    conn(j,i) = 1;
                end
            end
        end
    end
end


function collide = lineCollide(p1,p2,Centers,Radii)
%Detects if the line defined by the two points collides with any of the
%obstacles.

    ln1 = p2-p1;
    linenorm = norm(ln1);
    ln1 = ln1/linenorm;
    ln2 = [-ln1(2); ln1(1)]; % Compute perpendicular vector
    % Project all obstacles onto perpendicular
    dists = abs(ln2' * bsxfun(@minus, Centers, p1));
%     radii = robot.Radius + World.obstacles.Radii;
    possibles = find(dists < Radii);
    % Check collision positions on possibles
    collide = false;
    for p=possibles
        offset = ln1' * (Centers(:,p) - p1);
        if offset < 0
            % Point is down past pt1
            if norm(Centers(:,p) - p1) < Radii(p)
                collide = true;
                break;
            end
        elseif offset > linenorm
            % Point is up past pt2
            if norm(Centers(:,p) - p2) < Radii(p)
                collide = true;
                break;
            end
        else
            % Obstacle is between the two end points, meaning collision
            % detected.
            collide = true; 
            break;
        end
    end
end

% function neverUsed()
%     %%
%     figure(1), clf, hold on, grid on, axis equal;
%     W = World;
%     W.robot = robot;
%     drawworld(World);
% end

