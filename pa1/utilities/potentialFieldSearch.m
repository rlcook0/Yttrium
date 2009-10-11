function path = potentialFieldSearch(World,Field)
%Runs a search for a path on a potential field.
%  path = potentialFieldSearch(World).

% Maximum number of steps to run the search for before giving up
maxSteps = 1000;
% x and y increment to search around current location in
step = 0.1;
config = World.robot.Configuration';
goalConfig = World.goal.Configuration';
path = config;
while norm(config - goalConfig) >= step
    % Check for invalidity of search
    if getPotentialValue(World,config') == inf
        error('Search entered invalid location. Your obstacle avoidance is not substantial enough.');
    end
    % Check for overflow max size.
    if size(path,2) > maxSteps;
        break;
    end
    
    % Check which move on an 8-point grid is best.
    Vals = reshape([getPotentialValue(World,config' + step*[-1 -1]);
                    getPotentialValue(World,config' + step*[-1  0]);
                    getPotentialValue(World,config' + step*[-1  1]);
                    getPotentialValue(World,config' + step*[ 0 -1]);
                    inf;
                    getPotentialValue(World,config' + step*[ 0  1]);
                    getPotentialValue(World,config' + step*[ 1 -1]);
                    getPotentialValue(World,config' + step*[ 1  0]);
                    getPotentialValue(World,config' + step*[ 1  1])],...
                    [3 3]);
    [i,j] = find(Vals==min(Vals(:)),1);
    config = config + step*[j-2; i-2];
    path = [path config];
end

% Draw world overlaid with path
figure(1), clf;
subplot(121), hold on, grid on, axis equal;
plot(path(1,:),path(2,:),'k-','LineWidth',3);
% Draw robot's goal configuration in red.
World.robot.Color = [1 0 0];
World.robot.Configuration = World.goal.Configuration;
drawWorld(World);
% Draw robot's starting configuration in blue.
World.robot.Color = [0 0 1];
World.robot.Configuration = path(:,1)';
drawRobot(World.robot);

% Draw potential field overlaid with path.
subplot(122), hold on;
imagesc(Field.Values), axis image, colormap jet;
xFactor = size(Field.Values,2) / (World.axis(2) - World.axis(1));
yFactor = size(Field.Values,1) / (World.axis(4) - World.axis(3));
hold on, plot( xFactor * (path(1,:) - World.axis(1)),...
               yFactor * (path(2,:) - World.axis(3)),...
               'k-','LineWidth',3);
xt = get(gca,'XTick');
yt = get(gca,'YTick');
set(gca,'XTick',xt,'XTickLabel',Field.Xs(xt+1));
set(gca,'YTick',yt,'YTickLabel',Field.Ys(yt+1));

if size(path,2) > maxSteps;
    subplot(121);
    title('INVALID PATH: Overflowed maximum number of steps');
else
    subplot(121), title('World with path');
    subplot(122), title('Potential field with path');
end

