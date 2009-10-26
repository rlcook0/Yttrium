%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% CS221 Programming Assignment 2
%%   Paul Baumstarck, Oct. 2009
%%   Stanford University
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Displays a single digit, stored as an array of pixel values.
% Standard usage: displayDigit(DigitSet.pixels(i, :))

function displayDigit(Digit)
  figure, clf;
  imagesc(reshape(Digit, [14 14])');
  axis image, colormap gray;
end
