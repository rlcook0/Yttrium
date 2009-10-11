function root = parseXML(fname)
%Parse XML from a file.
%  ROOT = PARSEXML(FNAME) parse FNAME into a tree of XML nodes. Assumes no
%  line contains more than one tag, but a tag can be spread across multiple
%  lines, e.g.:
%
%  <Node1 Value1="Things" Value2="Other things"
%         Value3="Arg">
%   <Child1 Value1="Huh" />
%  </Node1>
%
%  Only two field names are reserved and should not be used in XML. The
%  first is "Name" (case-sensitive) which, for the first node about, is
%  taken to be "Node1". This does not disallow the node from having another
%  "name" field within it. The second is "Inline", used for test contained
%  inside the tag, e.g., "<Text>The quick, brown fox ...</Text>" gets an
%  Inline field of "The quick, brown fox ...".
%
%  STAIR VISION LIBRARY
%  Copyright (c) 2009, Stanford University
%
%  AUTHOR(S): Paul Baumstarck <pbaumstarck@stanford.edu>

fp = fopen(fname,'r');
% Skip the "<?xml ...>" line.
[gs,rets] = readwhile(fp,'<\?');
if isempty(gs)
    root = [];
    return;
end
root = parseNode(gs,fp);
fclose(fp);


function node = parseNode(gs,fp)
%Parse a node given a starting string and the location in the file pointer.
%  NODE = PARSENODE(GS,GP).

% While gs has no closing '>', append fgetl's.
if isempty(regexp(gs,'>$', 'once'))
    [gs1,ix,rets] = readuntil(fp,'>$');
    for i=1:length(rets)
        gs = [gs ' ' rets{i}];
    end
    gs = [gs ' ' gs1];
end
% TODO paulb: Possibly make this more efficient, but it hurts to make
% quote-sensitive whitespace splitting ...
gs = strtrim(gs);

% Get name.
node = [];
if ~isempty(regexp(gs,'<\w+\s*>','once'))
    % The node only contains its name.
    T = regexp(gs,'<(\w+\s*)>','tokens');
    gt = T{1}{1};
    node.Name = strtrim(gt);
    % Trim gs of name.
    gs = gs(length(gt)+3:end);
else
    % The node should contain spaces, so parse for name and attributes.
    T = regexp(gs,'\s+','split');
    % Remove null entries.
    i = 1;
    while i <= length(T)
        if isempty(T{i})
            T = { T{1:i-1} T{i+1:end} };
        else
            i=i+1;
        end
    end
    node.Name = T{1}(2:end);
    if node.Name(end) == '>'
        node.Name = node.Name(1:end-1);
    end
    if strcmp(node.Name,'!--')
        % Comment node. Move to next node.
        gs = readuntil(fp,'<');
        node = parseNode(gs,fp);
        return;
    end
    T = {T{2:end}};

    % Compact "-bracketed spaced entries.
    i = 2;
    while i <= length(T)
        if isempty(regexp(T{i},'=', 'once'))
            T{i-1} = [T{i-1} ' ' T{i}];
            T = { T{1:i-1} T{i+1:end} };
        else
            i=i+1;
        end
    end

    % Consume all attributes.
    while true
        for i=1:length(T)
            name = regexp(T{i},'^\w+','match');
            if isempty(name)
                continue;
            end
            value = regexp(T{i},'(?<=\=")[^\"]*(?=\")','match');
            if isempty(value)
                value = {''};
            end
            node = setfield(node,name{1},value{1});
        end
        if isempty(regexp(gs,'>$', 'once'))
            gs = fgetl(fp);
        else
            break;
        end
    end
end

% Check if singleton or one-line node.
if ~isempty(regexp(gs,'/>$', 'once'))
    % This is a singleton node ("<... />") so we're done with it.
    return;
end
if ~isempty(regexp(gs,['</' node.Name '>'], 'once'))
    pos = regexp(gs,['</' node.Name '>'], 'once');
    % Save any creamy center as inline text.
    if pos > 1
        node.Inline = strtrim(gs(1:pos-1));
    end
    % Node is closed, so return.
    return;
end

% Search for closing tag.
gs = fgetl(fp);
if ~isempty(regexp(gs,['</' node.Name '>'], 'once'))
    return;
end
% Otherwise, we have a child node or text; recurse.
while isempty(regexp(gs,['</' node.Name '>'], 'once'))
    if isempty(regexp(gs,'^\s*<(?!/)', 'once'))
        % No open tag, so save as inline text.
        if ~isfield(node,'Inline')
            node.Inline = strtrim(gs);
        else
            node.Inline = [node.Inline strtrim(gs)];
        end
    else
        % Is a child node, so parse and push back.
        if ~isfield(node,'children')
            node.children = cell(0);
        end
        node.children{end+1} = parseNode(gs,fp);
    end
    gs = fgetl(fp);
end
if isfield(node,'Inline') && isempty(node.Inline)
    node = rmfield(node,'Inline');
end


