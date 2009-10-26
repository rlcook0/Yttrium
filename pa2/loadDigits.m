%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% CS221 Programming Assignment 2
%%   Olga Russakovsky, Oct. 2009
%%   Stanford University
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Given an imageFilename (e.g., 'training-1k-images.idx3') and a
% labelsFilename (e.g., 'training-1k-labels.idx1'), parses both files and
% returns a struct DigitSet:
%       DigitSet.pixels is a K x N matrix of pixel values, where K is the
%       number of example images and N is the number of pixels within each
%       example image 
%       DigitSet.labels is a K-dimensional vector of example labels (0-9)
%       DigitSet.weights is a K-dimensional vector of example weights

function DigitSet = loadDigits(imageFilename, labelsFilename)
    DigitSet.pixels = readImages(imageFilename);
    numExamples = length(DigitSet.pixels);
    DigitSet.labels = readLabels(labelsFilename, numExamples);
    DigitSet.weights = ones(numExamples, 1);
end

function digits = readImages(filename)
    fid = fopen(filename);
    mn = readBinaryInt(fid);
    if (mn ~= 2051)
        disp(sprintf('Wrong file format'));
        return;
    end
    num_digits = readBinaryInt(fid);
    num_rows = readBinaryInt(fid);
    num_cols = readBinaryInt(fid);
    
    disp(sprintf('Loading %i digits', num_digits));
    digits = zeros(num_digits, num_rows * num_cols);
    for i=1:num_digits
        %if mod(i,100) == 0
         %   disp(sprintf('       ...%i', num_digits-i));
        %end
        digits(i, :) = readDigit(fid, num_rows, num_cols);
    end
end


function labels = readLabels(filename, num_digits)
    fid = fopen(filename);
    mn = readBinaryInt(fid);
    if (mn ~= 2049)
        disp(sprintf('Wrong file format'));
        return;
    end
    num_labels = readBinaryInt(fid);
    if (num_digits ~= num_labels)
        disp(sprintf('readLabels: The image file and the labels file have different length'))
        return;
    end
    
    for i=1:num_labels
        labels(i) = readLabel(fid);
    end
    labels = labels';
end


function n = readBinaryInt(fid)
    A = fread(fid, 4, 'uint8');
    n = A(1) * 256^3 + A(2) * 256^2 + A(3) * 256 + A(4);
end

function D = readDigit(fid, num_rows, num_cols)
    D = fread(fid, num_rows * num_cols, 'uint8');
    D = D / 255;
end

function n = readLabel(fid)
    n = fread(fid, 1, 'uint8');
end