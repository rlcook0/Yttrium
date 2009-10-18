function value = getPotentialValue(World,config)
%Get potential value for configuration
%  value = getPotentialValue(World,config) returns a potential field value
%  for the given configuration point, config, which is a row vector. The
%  form of the potential field must be as described in the handout.
%
%  Remember, you only have to implement this for the spherical robot, so
%  the configuration points provided will correspond to [x,y] locations in
%  the world.
%
%  The World.obstacles object contains two members to make your job easier.
%  The first is World.obstacles.Centers, which is a 2-by-N matrix
%  containing the center points of all obstacles. The other is
%  World.obstacles.Radii, which has the radii of all obstacles collected in
%  a row vector.

% Constant
alpha = 3;
repulsive_field_distance = 1;

repulsive_potential = 0;
robot_radius = World.robot.Radius;
for obstacle_num = 1:length(World.obstacles.Centers)
    center = World.obstacles.Centers(:,obstacle_num);
    radius = World.obstacles.Radii(obstacle_num);
    
    % Euclidean distance.
    distance_from_center = sqrt(sum((center' - config) .^ 2));
    distance_from_edge = distance_from_center -(radius + robot_radius);
    
    if (distance_from_edge <= 0)
        % You are inside an obstacle.
        value = inf;
        return;
    elseif (distance_from_edge < repulsive_field_distance)
        % Close enough to feel the pain.
        repulse = 1 / (1 + distance_from_edge)^2 - (1/4);
        repulsive_potential = repulsive_potential + repulse;
    end
end

% Euclidian distance.
attractive_potential = sqrt(sum((World.goal.Configuration - config) .^ 2));

value = attractive_potential + alpha * repulsive_potential;

