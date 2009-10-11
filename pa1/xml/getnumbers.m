function ret = getnumbers(gs,ix)
%Get all numbers from a string.
%  RET = GETNUMBERS(GS,IX=-1) return a vector of the read numbers. Works
%  with integers, floats, and scientific notation. If IX is specified,
%  returns only the IX'th number in the string.
%
%  STAIR VISION LIBRARY
%  Copyright (c) 2009, Stanford University
%
%  AUTHOR(S): Paul Baumstarck <pbaumstarck@stanford.edu>

if iscell(gs)
    ret = cell(size(gs));
    for i=1:length(gs)
        ret{i} = getnumbers(gs{i});
    end
else
    ret = regexp(gs,'-?\d*\.?\d*(e(+|-)?\d+)?','match');
    for i=1:length(ret)
        ret{i} = str2num(ret{i});
    end
    ret = cell2mat(ret);

    if nargin >= 2 && ~isempty(ix)
        ret = ret(ix);
    end
end


