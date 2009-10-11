function animatePath(World,path,plotTitle)
%Animate a motion path.
%  animatePath(WORLD,PATH,PLOTTITLE) animtes the PATH of the robot in the
%  WORLD, and assigned the title PLOTTILE to the plot.

% Set robot starting position
pathi = 1;
World.robot.Configuration = World.Landmarks(:,path(pathi));
% Set movement vector and norm limit
World.robot.vector = World.Landmarks(:,path(2)) - World.Landmarks(:,path(1));
World.robot.vectorNorm = norm(World.robot.vector);
World.robot.vector = World.robot.vector / World.robot.vectorNorm;
World.robot.movedNorm = 0;
if strcmp(World.robot.Type,'Sphere')
    dmove = 0.2;
else
    dmove = 0.03;
end
while true
    cla, hold on, grid on, axis equal;
    title(plotTitle);
    if strcmp(World.robot.Type,'Sphere')
        plot(World.Landmarks(1,:),World.Landmarks(2,:),'b*');
        for i=1:length(path)-1
            pts = World.Landmarks(:,path([i i+1]));
            plot(pts(1,:),pts(2,:),'r-','LineWidth',2);
        end
    end
    drawWorld(World);
    if pathi == length(path)
        break;
    end
    % Update robot position
    World.robot.Configuration = World.robot.Configuration + dmove * World.robot.vector;
    World.robot.movedNorm = World.robot.movedNorm + dmove;
    if World.robot.movedNorm >= World.robot.vectorNorm
        % Snap position to next movement point
        pathi = pathi + 1;
        World.robot.Configuration = World.Landmarks(:,path(pathi));
        if pathi < length(path)
            World.robot.vector = World.Landmarks(:,path(pathi + 1)) ...
                - World.Landmarks(:,path(pathi));
            World.robot.vectorNorm = norm(World.robot.vector);
            World.robot.vector = World.robot.vector / World.robot.vectorNorm;
            World.robot.movedNorm = 0;
        else
            % Draw one more time.
        end
    end
    pause(0.01);
end

