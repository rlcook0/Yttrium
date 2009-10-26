%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% CS221 Programming Assignment 2
%%   Paul Baumstarck, Oct. 2009
%%   Stanford University
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Displays a random subset of 20 digits from a DigitSet

function displayDigits(DigitSet)

figure(1), clf;
numberRows = 4;
numberCols = 5;
ixes = ceil(length(DigitSet.weights) * rand(1,numberRows*numberCols));
for i=1:length(ixes)
   ix = ixes(i);
   subplot(numberRows,numberCols,i);
   imagesc(reshape(DigitSet.pixels(ix,:),[14 14])');
   axis image, set(gca,'XTick',[],'YTick',[]);
   title(sprintf('%d',DigitSet.labels(ix)));
end

end
