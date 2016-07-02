# Reference

This document is a detailed and (hopefully) complete reference of the public interface of `rpclib`.
For a tutorial, take a look at the [Primer](primer.md). Also, you can find many examples in the [Cookbook](cookbook.md).

% for c in classes:
${"##"} ${c.name}

```cpp
${"#include "}"${c.includes}"
```
${"### Description"}

${c.brief}

${c.desc}

${"### Public functions"}

| | |
|---------|-------------|
% for f in c.functions:
% if f.type == None:
| | [${f.name}](${"#"}${f.name})${f.argsstr}
% else:
| ${f.type} | [${f.name}](${"#"}${f.name})${f.argsstr}
%endif
% endfor

% for f in c.functions:
${"####"} ${f.name} {: #${f.name}}

${f.brief}
${f.desc}

% endfor

% endfor
