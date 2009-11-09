%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% CS221 Programming Assignment 3
%%   Chris Archibald, Oct. 2009
%%   Stanford University
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% (Very simply) Displays a car (actually a triangle, but whatever) at the location and
% heading specified

function showCar(carx, cary, cartheta)

    vert = [carx-3 cary-1 0; carx-3 cary+1 0; carx+3 cary 0;];% carx+3 cary-1 0;];
    fac = [1 2 3];
    P = patch('Faces', fac, 'Vertices', vert, 'facecolor', 'r');
    
    rotate(P,[0 0 1], cartheta, [carx cary 0]);
    axis([-20 50 -10 10]);
    axis equal;

end
