%CS221 Programming Assignment 4
%Author: Ian Goodfellow


%Run localize(options) to run your experiment
%Options should have been created with makeLocalizeOptions
function localize(options)
	%Parse the image options into more convenient local variables
	showImages = options.imageOutputOptions.showImages;
	saveImages = options.imageOutputOptions.saveImages;
	images = showImages || saveImages;
	framesPerImage = options.imageOutputOptions.framesPerImage;

	%Load and configure the map of obstacles
	obstacleMap = obstacleMapRead(options.mapFilepath);
	obstacleMap.metersPerCell = 0.1;
	
	%Initialize belief map
	beliefMap = beliefMapCreate(obstacleMap, 5);

	%Initialize robot simulator
	robotSim = robotSimulatorCreate();
	robotSim = robotSimulatorReadLogFile(robotSim, options.logFilepath);
	robotSim = robotSimulatorGenerateStates(robotSim, 5, beliefMap);
	
	%Initialize action model
	actionModel = actionModelCreate(options.actionModel, beliefMap, robotSim);
	
	%Initialize sensor model
	sensorModel = sensorModelCreate(options.sensorModel);

	%Initialize the belief map
	beliefMap = beliefMapInitializeBeliefs(beliefMap, robotSim.initialPosition, ...
	options.kidnapped);

	%Generate the image for t=0, when the robot has not processed any observations yet
	t = 0;
	if images 
		makeImage(showImages,saveImages, obstacleMap, beliefMap, robotSim,t);
	end


	%Calculate the error
	if options.outputTotalError
		errorf = fopen('error.txt','w');
		error = calculateError(robotSim.actual(1,:),beliefMap.beliefs);
		fprintf(errorf,'%d\n',error);
	end


	%Run the experiment
	for t=1:size(robotSim.action,1)
		fprintf(1,'Processing frame %d of %d \n',t,size(robotSim.action,1));
		
		if ~ options.history
		%CS221 TASK 4
		% You'll need to handle the 'forgetful robot'
		% (i.e., no history) case here
        
        beliefMap.beliefs = ones(size(beliefMap.beliefs));
        beliefMap.beliefs = beliefMap.beliefs / sum(sum(beliefMap.beliefs));
	
		%END OF CS221 TASK 4
		end


		%Update the belief map by applying the motion and sensor updates
		beliefMap = beliefMapUpdate(beliefMap, t, actionModel, sensorModel, robotSim);

		%Calculate the error
		if options.outputTotalError
			error(numel(error)+1) = calculateError(robotSim.actual(t,:),beliefMap.beliefs);
			fprintf(errorf,'%d\n',error);
		end

		%Make the image
		if images && mod(t,framesPerImage) == 0
			makeImage(showImages,saveImages, obstacleMap, beliefMap, robotSim, t);
		end
	end

	%Plot the error
	if options.outputTotalError
		fclose(errorf);
		figure();
		plot(error);
		title('Error Plot');
		xlabel('Time');
		ylabel('Error');
	end
end




%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Functions in the following section require modification
% for CS221 PA4
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function beliefMap = beliefMapInitializeBeliefs(beliefMap, ...
	initialPosition, bKidnapped)

%This is where you initialize the robots beliefs at the
%start of the experiment. beliefMap.belief and beliefMap.buffer
%are already allocated at this point, but you need to set
%beliefMap.belief to have the correct values.
%In particular, beliefMap.belief should sum to 1

	if ~ bKidnapped
		% CS221 TASK 1
		% In this version of the experiment, the
		% robot knows exactly where it is starting.
		% Use the initialPosition variable to set
		% beliefMap.beliefs correctly

		beliefMap.beliefs = zeros(size(beliefMap.beliefs));
		beliefMap.beliefs(initialPosition(1), initialPosition(2)) = 1;
	else
		% CS221 TASK 3
		% In this version of the experiment, assume
		% the robot does not know where it is starting
		% (ignore initialPosition). Initialize the
		% beliefMap to a uniform distribution over free squares

		beliefMap.beliefs = ones(size(beliefMap.beliefs));
        beliefMap.beliefs = beliefMap.beliefs / sum(sum(beliefMap.beliefs));
	end


	%Error checking code that helps ensure that your code for tasks 1 and 3 works correctly
	if sum(size(beliefMap.beliefs) == size(beliefMap.freeMask)) ~= 2
		error 'You seem to have resized the belief map.';
	end

	if abs(1-sum(sum(beliefMap.beliefs))) > 1e-5
		error 'Your belief map does not sum to 1.';
	end

	if min(min(beliefMap.beliefs)) < 0
		error 'Some of your probabilities are negative.';
	end


	
end

%Applies the motion and sensor updates to a belief map
function beliefMap = beliefMapUpdate(beliefMap, t, actionModel, sensorModel, robotSim)

	%CS221 TASK 2
	%Implement this function. 
	%1. Use the action model to update the beliefs, using the action at time t
	%   You can get the action from the robotSim object
	%   Be sure to read through the actionModel interface below (search for 'CS221 TASK 2')
	%   You can use beliefMap.buffer to help in this step
	%   Remember that the robot can never move more than one step in any direction per time
	%   step; you must use this fact to reduce the number of terms you must sum over
        %   
	%  Optionally, you may use the fact that the robot is never in an occupied cell
 	%  to get an additional 5X speedup

	%2. Use the sensor model to update the beliefs.
	% See sensorModelGetProbabilities below
	% (search for 'CS221 TASK 2')

    beliefMap.buffer = zeros(size(beliefMap.beliefs));
    
    action = robotSim.action(t,:);
    actionModel = actionModelSetAction(actionModel, action);

    max_x = size(beliefMap.beliefs, 1);
    max_y = size(beliefMap.beliefs, 2);
    
    for x = 1:max_x
        for y = 1:max_y
            if (beliefMap.freeMask(x, y) == 0)
                continue
            end
            
            s_prime = [x y];
            for i = -1:1
                for j = -1:1
                    s = s_prime + [i j];
                    val = beliefMap.beliefs(s(1), s(2));
                    
                    actionModel = actionModelSetState(actionModel, s);
                    prob = actionModelGetProbability(actionModel, s_prime);
                    
                    if (prob > 0)
                        beliefMap.buffer(s_prime(1), s_prime(2)) = ...
                            beliefMap.buffer(s_prime(1), s_prime(2)) + val * prob;
                    end
                end
            end
        end
    end                 
    
    M = sensorModelGetProbabilities(sensorModel, t, robotSim, beliefMap);
    beliefMap.beliefs = beliefMap.buffer .* M;
    beliefMap.beliefs = beliefMap.beliefs / sum(sum(beliefMap.beliefs));
    
	%END CS221 TASK 2


	%Error checking code that helps ensure that your code for task 2 works correctly
	if sum(size(beliefMap.beliefs) == size(beliefMap.freeMask)) ~= 2
		error 'You seem to have resized the belief map.';
	end

	if abs(1-sum(sum(beliefMap.beliefs))) > 1e-5
		error 'Your belief map does not sum to 1.';
	end

	if min(min(beliefMap.beliefs)) < 0
		error 'Some of your probabilities are negative.';
	end


	if sum(sum(abs(beliefMap.beliefs .* (1-beliefMap.freeMask)))) > 0
		error 'You seem to be placing non-zero probability on occupied cells.';
	end	
end

%Estimates the probability of a laser reading giving a distance on the map
%Assumes probability distribution is a truncated gaussian-- it is impossible
%to get a negative reading, but other than that it is a gaussian distribution
%cented on the map distance
function p = laserProbability(reading, mapdist)
	sigma = 5;

	%CS221 TASK 5
	%Implement this function.
	%p should be the probability density at reading
	%of a probability distribution defined by a normal distribution with mean 
	%mapdist and the given sigma, but truncated so that it assigns no 
	%probability mass to negative readings. 
	%You may find the matlab functions normpdf and normcdf useful

    P = normcdf([0 inf], mapdist, sigma);
    after_zero = P(2) - P(1);
    
    p = normpdf(reading, mapdist, sigma) / after_zero;

	%END CS221 TASK 5
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Functions in this section should be called by CS221 students,
% but don't need to be modified in any way.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%Sets the action that the actionModel will condition on
%Of interest for CS221 TASK 2
function actionModel = actionModelSetAction(actionModel, action)

	if max(abs(action)) > 1
		action
	end

	actionModel.action = action;
	actionModel.actionIndex = offsetToIndex(action);
end


%Sets the previous state to condition on ( [ row col ] format )
%Of interest for CS221 TASK 2
function actionModel = actionModelSetState(actionModel, state)

	actionModel.state = state;

	if actionModel.isRandom
		onHorizEdge = (state(1) == 1 || state(1) == ...
		actionModel.rows);
		onVertEdge = (state(2) == 1 || state(2) == ...
		actionModel.cols);

		if onHorizEdge && onVertEdge
			actionModel.denominator = 4;
		elseif onHorizEdge || onVertEdge
			actionModel.denominator = 6;
		else
			actionModel.denominator = 9;
		end	
	else
		actionModel.denominator = 0;

		for ii = -1:1
			for jj =-1:1
				newCoord = state + [ii jj];
				
				if sum(newCoord > 0) == 2

					if newCoord(1) <= actionModel.rows ...
 && newCoord(2) <= actionModel.cols
						actionModel.denominator = ...
						actionModel.denominator + ...
 actionModel.probs(actionModel.actionIndex, offsetToIndex([ii jj]));
					end
				end
			end
		end
	
	end
end


%Returns P( newState | state, action), where state and action
%were set using the functions above
%Note-- this will automatically return 0 if newState is off
%the map or an occupied cell, so you can do bounds-checking
%by testing if the return value is greater than 0
%Of interest for CS221 TASK 2
function p = actionModelGetProbability(actionModel, newState)

	if newState(1) > actionModel.rows
		p = 0;
		return
	end

	if newState(1) < 1 || newState(2) < 1
		p = 0;
		return
	end

	if newState(2) > actionModel.cols
		p = 0;
		return
	end



	if max(abs(newState-actionModel.state)) > 1
		p = 0;
		return
	end


	if actionModel.isRandom
		%Random action model
		p = 1 / actionModel.denominator;
	else
		%Simple action model
		p = actionModel.probs(actionModel.actionIndex, ...
 offsetToIndex(newState-actionModel.state)) / actionModel.denominator;
	end
end

%Returns a matrix M such that M(i,j) = P(o|s) where s
%is cell [i,j] and o is the reading at time t in robotSim
%Of interest for CS221 TASK 2
function M = sensorModelGetProbabilities(sensorModel, t, robotSim, beliefMap)
	
	%Bump sensor reading: assume the robot never bumped into anything
	%so being in a non-free cell gives 0 probability

	M = beliefMap.freeMask;

	%Assume all sensors are independent so we just multiply together
	%the probabilities from each



	if sensorModel.useLaser
		for i=1:size(M,1)
			for j=1:size(M,2)
				if M(i,j) > 0
					for k=1:4
						M(i,j) = M(i,j) * ...
						laserProbability( ...
						robotSim.laser(t,k), ...
						beliefMap.distances(i,j,k));

					end
				end
			end
		end
	end
end



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Functions below this point are not of even the slightest
% interest to CS221 students; they exist only to implement
% the infrastructure of the experiment.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function actionModel = actionModelCreate(type, beliefMap, robotSim)
	if strcmp(type,'random')
		isRandom = 1;
	elseif strcmp(type, 'simple')
		isRandom = 0;
	else
		error 'ActionModel of unknown type requested';
	end

	actionModel = struct('isRandom',isRandom,'rows',...
	size(beliefMap.freeMask,1), 'cols', size(beliefMap.freeMask,2));

	if ~actionModel.isRandom
		%Cheat a bit by training on the test set
		%But use a weak uniform prior so it doesn't do too well
		actionModel.probs = ones(9,9) * 1e-1;

		for t=2:size(robotSim.action,1)
			action = robotSim.action(t,:);
			actionIndex = offsetToIndex(action);

			mvt = robotSim.actual(t,:) - robotSim.actual(t-1,:);
			mvtIndex = offsetToIndex(action);

			actionModel.probs(actionIndex, mvtIndex) = ...
			actionModel.probs(actionIndex, mvtIndex) +1;
		end

		for i=1:9
			actionModel.probs(i,:) = actionModel.probs(i,:) / ...
			sum(actionModel.probs(i,:));
		end

	end
end

%Converts (x,y) meters coordinates to (row,col) belief map cell coordinates
function coords = beliefMapCoordsFromLogCoords(beliefMap,logCoords)
	coords(1) = floor(logCoords(2)/beliefMap.metersPerCell);
	coords(2) = floor(logCoords(1)/beliefMap.metersPerCell);
	%Need to flip y
	coords(1) = size(beliefMap.freeMask,1) - coords(1);
end

function map = beliefMapCreate(obstacleMap, cellWidth)
	map = struct('cellWidth', cellWidth, ...
			'metersPerCell',cellWidth * obstacleMap.metersPerCell);
	map.freeMask = zeros( floor( size(obstacleMap.types) / cellWidth ));

	map.beliefs = map.freeMask;
	map.buffer = map.freeMask;



	FREE = 2;


	for j=1:size(map.freeMask,2)
		for i=1:size(map.freeMask,1)	
			oi = (i-1)*cellWidth+1;
			oj = (j-1)*cellWidth+1;

			if obstacleMap.types( oi, oj) == 2
				map.freeMask(i,j) = 1;

				map.distances(i,j,:) = obstacleMap.distances(oi,oj,:);

			
				for fi=oi:(oi+cellWidth-1)
					for fj=oj:(oj+cellWidth-1)
						for dist=1:4
							map.distances(i,j,dist) = min( ...
							map.distances(i,j,dist), ...
							obstacleMap.distances(i,j,dist));
						end
					end
				end
			end
		end
	end

	map.distances = map.distances / cellWidth;


	%viz = zeros([size(map.distances,1) size(map.distances,2) 3]);

	%for i=1:size(viz,1)
	%	for j=1:size(viz,2)
	%		viz(i,j,1) = map.distances(i,j,4) / max(max(max(map.distances(:,:,4))));
	%		viz(i,j,2) = map.freeMask(i,j);
	%	end
	%end

	%imshow(viz);
	%keyboard	

	%if max(max(map.freeMask)) == 0
	%	error 'belief map contains no free cells'
	%end
end

function e = calculateError(truePosition, beliefs)
	dists = beliefs;
	for i=1:size(dists,1)
		for j=1:size(dists,2)
			dists(i,j) = norm([i j] - truePosition);
		end
	end

	e = sum(sum(dists.*beliefs));
end

%Maps an integer in [1:9] to a pair [r,c] with r,c in [-1:1]
function o = indexToOffset(i)
	o = [ floor((i-1)/3) - 1, rem(i-1,3) -1 ];
end

function makeImage(showImages,saveImages, obstacleMap, beliefMap, robotSim, t)
	if t == 0
		t = 1;
	end

	image = obstacleMap.image;

	cw = beliefMap.cellWidth;

	beliefs = round(255*beliefMap.beliefs/max(max(beliefMap.beliefs)));


	for i = 1:size(beliefMap.beliefs,1)
		for j = 1:size(beliefMap.beliefs,2)
			for ii = (i-1)*cw+1:i*cw
				for jj = (j-1)*cw+1:j*cw
					b = beliefs(i,j);
					if b > 0
						image(ii,jj,1) = 255;
						image(ii,jj,2) = 255-b;
						image(ii,jj,3) = 255-b;	
					end	

					if i == robotSim.actual(t,1) && j ...
== robotSim.actual(t,2) && ( (rem(ii,2) == 0 && rem(jj,2) == 0) || ...
(rem(ii,2) == 1 && rem(jj,2) == 1))
						image(ii,jj,1) = 0;
						image(ii,jj,2) = 255;
						image(ii,jj,3) = 0;
					end		

					if i == robotSim.deadReckon(t,1) && j...
== robotSim.deadReckon(t,2) &&  rem(jj,2) == 0
						image(ii,jj,1) = 0;
						image(ii,jj,2) = 255;
						image(ii,jj,3) = 255;
					end	
				end
			end
		end	
	end

	

	if showImages
		imshow(image)
		pause(0.01)
	end

	if saveImages
		imwrite(image,['images/frame_' num2str(t) '.jpg'],'jpg');
	end
	
end

function map = obstacleMapRead(filepath)
	map = struct('image',0, 'metersPerCell', 0.1);
	%map.image is the raw bitmap from the ppm
	warning off
	map.image = imread(filepath);
	warning on

	%map.types(i,j) gives the type of the cell at pixel (i,j)
	UNKNOWN = 0;
	OCCUPIED = 1;
	FREE = 2;
	map.types = zeros( [size(map.image,1) size(map.image,2)] );
	
	%Read the map types out of the color code in the map
	for i=1:size(map.types,1)
		for j=1:size(map.types,2)
			if map.image(i,j,1) == 0 ...
			&& map.image(i,j,2) == 0 ...
			&& map.image(i,j,3) == 255
				map.types(i,j) = UNKNOWN;
			%I did not make up the color code, 192 was the value used
			% in the C++ version
			elseif map.image(i,j,1) < 192
				map.types(i,j) = OCCUPIED;
			else
				map.types(i,j) = FREE;
			end
		end
	end

	if max(max(map.types == FREE)) < 1
		error 'ppm file contains no free cells'
	end


	NORTH = 1;
	SOUTH = 2;
	WEST = 3;
	EAST = 4;

	%This is copied from the C++ version
	MAX_DISTANCE = 50;

	%map.distances(i,j,k) gives the shortest distance to a wall in direction k
	%where k represents NORTH, SOUTH, WEST, or EAST
	map.distances = 100 * ones( [size(map.types) 4]);
	
	%Note: for my code, north is decreasing i, west is decreasing j
	%for the C++ code, north is decreasing j, west is decreasing i

	%Set the north distance, so sweep from north to south
	for j=1:size(map.distances,2)
		lastMarkType = UNKNOWN;
		lastMark = 0;
		
		for i=1:size(map.distances,1)
			if (map.types(i,j) ~= FREE)
				lastMarkType = map.types(i,j);
				lastMark = i;
			else
				if lastMarkType == UNKNOWN
					map.distances(i,j,NORTH) = MAX_DISTANCE;
				else
					map.distances(i,j,NORTH) = i - lastMark;	
				end
			end
		end
	end

	%Set the south distance, so sweep from south to north
	for j=1:size(map.distances,2)
		lastMarkType = UNKNOWN;
		lastMark = size(map.distances,1)+1;
		for i=size(map.distances,1):-1:1
			if map.types(i,j) ~= FREE
				lastMarkType = map.types(i,j);
				lastMark = i;
			elseif lastMarkType == UNKNOWN
				map.distances(i,j,SOUTH) = MAX_DISTANCE;
			else
				map.distances(i,j,SOUTH) = lastMark - i;
			end
		end
	end

	%Get the west distance, so sweep from west to east
	for i = 1:size(map.distances,1)
		lastMarkType = UNKNOWN;
		lastMark = 0;

		for j=1:size(map.distances,2)
			if map.types(i,j) ~= FREE
				lastMarkType = map.types(i,j);
				lastMark = j;
			elseif lastMarkType == UNKNOWN
				map.distances(i,j,WEST) = MAX_DISTANCE;
			else
				map.distances(i,j,WEST) = j - lastMark;
			end
		end

	end

	%Get the east distance, so sweep from east to west
	for i = 1:size(map.distances,1)
		lastMarkType = UNKNOWN;
		lastMark = size(map.distances,2)+1;

		for j= size(map.distances,2):-1:1
			if map.types(i,j) ~= FREE
				lastMarkType = map.types(i,j);
				lastMark = j;
			elseif lastMarkType == UNKNOWN
				map.distances(i,j,EAST) = MAX_DISTANCE;
			else
				map.distances(i,j,EAST) = lastMark - j;
			end
		end
	end

%{
	norths = zeros( [ size(map.distances,1) size(map.distances,2) ] );
	for i=1:size(norths,1)
		for j=1:size(norths,2)
			norths(i,j) = map.distances(i,j,SOUTH);
		end
	end

	imshow(norths * 255 / max(max(norths)));

	keyboard
%}
end

%Inverse of indexToOffset
function i = offsetToIndex(o)
	i = (o(1)+1)*3 +o(2)+2;
end

function robotSim = robotSimulatorCreate()
	robotSim = struct();
end

%Sets the initial state and downsamples the log data into a more manageable number of states
function robotSim = robotSimulatorGenerateStates(robotSim, logEntriesPerState, beliefMap)
	robotSim.initialPosition = ...
		beliefMapCoordsFromLogCoords(beliefMap,robotSim.log.actual(1,:));
	prevEntry = 1;

	deadReckonToTruth = robotSim.log.actual(1,:) - robotSim.log.deadReckon(1,:);

	prevDeadReckonCoord = beliefMapCoordsFromLogCoords(beliefMap,robotSim.log.actual(1,:));

	idx = 1;
	for i=1:logEntriesPerState:size(robotSim.log.deadReckon,1)
		deadReckonCoord = beliefMapCoordsFromLogCoords( ...
		beliefMap, ...
		robotSim.log.deadReckon(i,:) + deadReckonToTruth);

		robotSim.action(idx, :) = deadReckonCoord - prevDeadReckonCoord;

		if max(abs(robotSim.action(idx, :))) > 1
			error 'Time spaced too widely-- you are getting actions with steps greater than 1'
		end

		robotSim.laser(idx,:) = robotSim.log.laser(i,:) / beliefMap.metersPerCell;
		robotSim.deadReckon(idx,:) = deadReckonCoord;
		robotSim.actual(idx,:) = beliefMapCoordsFromLogCoords(beliefMap,robotSim.log.actual(i,:));

		prevDeadReckonCoord = deadReckonCoord;
		idx = idx + 1;
	end
end

function robotSim = robotSimulatorReadLogFile(this, filepath)
	robotSim = this;
	
	robotSim.log=struct();

	logData = load(filepath);


	NORTH = 1;
	SOUTH = 2;
	WEST = 3;
	EAST = 4;

	robotSim.log.deadReckon = logData(:,1:2);
	robotSim.log.laser(:,EAST) = logData(:,3);
	robotSim.log.laser(:,NORTH) = logData(:,4);
	robotSim.log.laser(:,WEST) = logData(:,5);
	robotSim.log.laser(:,SOUTH) = logData(:,6);
	robotSim.log.actual = logData(:,7:8);
end

function sensorModel = sensorModelCreate(type)
	sensorModel = struct('useLaser',0);

	sensorModel.useLaser = strcmp(type,'laser');

	if sensorModel.useLaser  == 0
		if ~ strcmp(type,'bump')
			error 'Unrecognized SensorModel type requested'
		end
	end
end


function checkDistances(beliefMap, robotSim)

	if size(beliefMap.distances,3) ~= 4
		error 'checkDistances: beliefMap contains wrong number of laser readings'
	end

	for i=1:218
		[beliefMap.distances(robotSim.actual(i,1),robotSim.actual(i,2),1),...
beliefMap.distances(robotSim.actual(i,1),robotSim.actual(i,2),2), ...
beliefMap.distances(robotSim.actual(i,1),robotSim.actual(i,2),3), ...
beliefMap.distances(robotSim.actual(i,1),robotSim.actual(i,2),4); ...
		robotSim.laser(i,:)  ]
		
	end
end
