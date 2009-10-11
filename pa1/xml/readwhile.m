function [gs,rets] = readwhile(fp,str,past)
%Read while fp matches str.
%  [GS,RETS] = READWHILE(FP,STR,PAST = 0) calls FGETL on FP while STR is
%  found, and then goes PAST number of FGETLs past. GS is the last line
%  read, and RETS is all lines read before in the call.
%
%  STAIR VISION LIBRARY
%  Copyright (c) 2009, Stanford University
%
%  AUTHOR(S): Paul Baumstarck <pbaumstarck@stanford.edu>

rets = {};
gs = fgetl(fp);
while ~isempty(regexp(gs,str))
    rets{end+1} = gs; %#ok<*AGROW>
    gs = fgetl(fp);
end

if nargin < 3 || isempty(past)
    past = 0;
end
for i=1:past
    rets{end+1} = gs;
    gs = fgetl(fp);
end

