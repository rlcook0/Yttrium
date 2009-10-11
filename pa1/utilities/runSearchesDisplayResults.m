function [paths,pathNames,costs] = runSearchesDisplayResults(World)
%Runs the PRM searches on the configuration space.
%  [paths,pathNames,costs] = runSearches(World) returns a cell array of
%  paths and an array of their associated names, and of costs.

[BFpath,BFcost,BFnodes] = bestFirstSearch(World);
[ASpath,AScost,ASnodes] = AstarSearch(World);
[UCpath,UCcost,UCnodes] = uniformCostSearch(World);
[ASIpath,ASIcost,ASInodes] = AstarInadmissibleSearch(World);

if ~isPathValid(World,BFpath)
    error('Best first path is invalid.');
end
if ~isPathValid(World,ASpath)
    error('A* search path is invalid.');
end
if ~isPathValid(World,UCpath)
    error('Uniform-cost path is invalid.');
end
if ~isPathValid(World,ASIpath)
    error('A* with inadmissible heuristic path is invalid.');
end

fprintf('Best first:      %0.2f cost, %d steps, %d nodes expanded\n', ...
    BFcost, numel(BFpath)-1, BFnodes);
fprintf('A*:              %0.2f cost, %d steps, %d nodes expanded\n', ...
    AScost, numel(ASpath)-1, ASnodes);
fprintf('Uniform-cost:    %0.2f cost, %d steps, %d nodes expanded\n', ...
    UCcost, numel(UCpath)-1, UCnodes);
fprintf('A* inadmissible: %0.2f cost, %d steps, %d nodes expanded\n', ...
    ASIcost, numel(ASIpath)-1, ASInodes);

% Draw recovered paths

figure(1), clf;
% For sphere robot, draw best-first path in magenta, A* path in red.
paths = {BFpath, ASpath, UCpath, ASIpath};
pathNames = {'Best first', 'A*', 'Uniform-cost', 'A* Inadmissible'};
costs = [BFcost AScost UCcost ASIcost];
% For arm robot, draw paths in different subplots as color sequences.
for j=1:length(paths)
    path = paths{j};
    subplot(2,2,j), cla, hold on, grid on, axis equal;
    title(sprintf('%s - Cost: %0.2f',pathNames{j},costs(j)));
    if strcmp(World.robot.Type,'Sphere')    
        % For sphere bot, plot all paths on one slide.
        [ik,jk] = find(triu(World.Connectivity));
        for ii=1:length(ik)
            plot(World.Landmarks(1,[ik(ii) jk(ii)]),...
                 World.Landmarks(2,[ik(ii) jk(ii)]),'g:');
        end
        plot(World.Landmarks(1,:),World.Landmarks(2,:),'b*');

        for i=1:length(path)-1
            pts = World.Landmarks(:,path([i i+1]));
            plot(pts(1,:),pts(2,:),'r-','LineWidth',4);
        end
    else
        myRobot = World.robot;
        colours = jet(length(path));
        % Draw first arm pose manually
        myRobot.Configuration = World.Landmarks(:,path(1));
        myRobot = computeArmPoints(myRobot);
        myRobot.Color = colours(1,:);
        plotArm(myRobot);
        lastRobot = myRobot;
        % Draw successive positions in gradually changing colors.
        for i=2:length(path)
            myRobot.Configuration = World.Landmarks(:,path(i));
            myRobot = computeArmPoints(myRobot);
            myRobot.Color = colours(i,:);
            plotArm(myRobot);
            fusionColour = mean(colours([i-1 i],:),1);
            for k=1:size(myRobot.Points,2)
                quiver(lastRobot.Points(1,:),lastRobot.Points(2,:),...
                    -(lastRobot.Points(1,:) - myRobot.Points(1,:)),...
                    -(lastRobot.Points(2,:) - myRobot.Points(2,:)),0,...
                     'Color',fusionColour);
            end
            lastRobot = myRobot;
        end
    end
    drawWorld(World);
end

