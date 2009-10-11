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

alpha = 1;

repulsive_potential = 0;
robot_radius = World.robot.Radius;
for obstacle_num = 1:length(World.obstacles.Centers)
    center = World.obstacles.Centers(:,obstacle_num);
    radius = World.obstacles.Radii(obstacle_num);
    
    distance = sqrt(sum((center' - config) .^ 2));
    
    if (distance < radius + robot_radius)
        value = inf;
        return;
    elseif (distance -(radius + robot_radius) < 1)
        dist = distance -(radius + robot_radius);
        repulse = 1 / (1 + dist)^2 - (1/4);
        repulsive_potential = repulsive_potential + repulse;
    end
end

attractive_potential = sqrt(sum((World.goal.Configuration - config) .^ 2));

value = attractive_potential + alpha * repulsive_potential;

