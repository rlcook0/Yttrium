function World = populateConfigPoints(World,useFilePoints,n)
%Creates configuration points.
%  WORLD = populateConfigPoints(WORLD) populates the
%  WORLD.Landmarks member with configuration points, and the
%  WORLD.Connectivity member with the sparse connectivity matrix between
%  those points. Landmarks is a 2-by-N matrix for a sphere-type robot
%  and a 3-by-N matrix for an arm-type robot, with N the number of
%  configuration points. The first column of the configuration matrix is the
%  robot's starting position defined by the world, and the last column is
%  the goal configuration.
%
%  WORLD = populateConfigPoints(WORLD,USEFILEPOINTS,N) will use the
%  configuration points provided in the original XML configuration is the
%  argument is true. If false, it randomly generates N valid points. It
%  generates sets of N configuration points until it obtains a set where
%  the goal is reachable from the starting configuration. A message will be
%  displayed whenever it starts checking a new set of N points. If too many
%  of these messages appear, consider increasing N to increase the chance
%  that set of points where the goal is reachable will be derived.

StartingConfig = World.robot.Configuration;
if useFilePoints
    % Use provided configuration space points
    points = getChildNode(World,'Landmarks');
    if strcmp(World.robot.Type,'Sphere')
        m = 2;
    elseif strcmp(World.robot.Type,'Arm')
        m = 3;
    end
    configPoints = zeros(m,length(points.children));
    for i=1:size(configPoints,2)
        configPoints(:,i) = getnumbers(points.children{i}.Inline);
    end
    % Add start and goal states to configuration space
    configPoints = [StartingConfig' configPoints ...
                World.goal.Configuration'];
    % Compute visibility
    connectivity = computeVisibility(World,configPoints);
else
    % Randomly generate points
    while true
        % Loop until we have generated some points where the goal
        % configuration is reachable from the start.
        
        % Generation random configuration space points.
        if strcmp(World.robot.Type,'Sphere')
            if isempty(n)
                n = 20;
            end
%             configPoints = 6 * rand(2,n) - 1;
            spans = [World.axis(2)-World.axis(1);
                     World.axis(4)-World.axis(3)];
            configPoints = bsxfun(@plus, ...
                bsxfun(@times, spans, rand(2,n)), ...
                World.axis([1 3])');
        elseif strcmp(World.robot.Type,'Arm')
            if isempty(n)
                n = 30;
            end
            configPoints = zeros(3,n);
%             configPoints = 2 * pi * rand(3,n) - pi;
            for k=1:n
                while true
                    pt = 2*pi*rand(3,1) - pi;
                    % Only take realizable positions.
                    if ~isempty(computeVisibility(World,[pt pt]))
                        break;
                    end
                end
                configPoints(:,k) = pt;
            end
        end
        % Add start and goal states to configuration space.
        configPoints = [StartingConfig' configPoints ...
                    World.goal.Configuration']; %#ok<AGROW>
        % Compute visibility
        connectivity = computeVisibility(World,configPoints);

        % Check if goal is reachable from starting configuration.
        reachablep = [];
        reachable = 1;
        while numel(reachable) ~= numel(reachablep)
            reachablep = reachable;
            reachable = unique([reachable(:);
                find(any(connectivity(:,reachable),2))]);
        end
        if ~ismember(size(configPoints,2),reachable)
            disp('Goal configuration was not reachable. Generating new random configuration points ...');
        else
            break;
        end
    end
end
World.Landmarks = configPoints;
World.Connectivity = connectivity;

