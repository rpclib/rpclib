# rpclib - modern msgpack-rpc for C++

Welcome! `rpclib` is a msgpack-rpc implementation written using modern C++. The goal of this library is to provide a simple, no-nonsense RPC solution.

## What does it look like?

Here is a very simple example, a kind of hello world for network-related libraries, the "echo"
server and client:

<div id="front-code">
<pre><code class="cpp hljs">#include "rpc/server.h"
#include &lt;string&gt;
using std::string;

int main() {
  rpc::server srv(8080);

  srv.bind("echo", [](string const& s) {
    return string("> ") + s;
  });

  srv.run();
  return 0;

</code>
</pre>

<pre id="second-code"><code class="cpp hljs">#include "rpc/client.h"
#include &lt;iostream&gt;
#include &lt;string&gt;
using std::string;

int main() {
  rpc::client c(&quot;localhost&quot;, 8080);

  string input, result;
  while (std::getline(std::cin, input)) {
    if (!input.empty()) {
      result = c.call(&quot;echo&quot;, input).as&lt;string&gt;();
      std::cout &lt;&lt; result &lt;&lt; std::endl;
    }
  }
}
</code>
</pre>
</div>

## Chapters

<ul id="front-menu">
    <li><a href="gettingstarted" class="btn btn-primary btn-lg">Getting started <br><p class="btn-desc">How to set up your environment to start using.<code>rpclib</code></p></a></li>
    <li><a href="primer" class="btn btn-primary btn-lg">Primer<br><p class="btn-desc">A longer tutorial introduction to using the library, intended for newcomers.</p></a></li>
    <li><a href="cookbook" class="btn btn-primary btn-lg">Cookbook<br><p class="btn-desc">Lots of examples of common tasks the library was designed for handling</p></a></li>
    <li><a href="compiling" class="btn btn-primary btn-lg">Compiling<br><p class="btn-desc">Basic and advanced building options.</p></a></li>
    <li><a href="glossary" class="btn btn-primary btn-lg">Glossary<br><p class="btn-desc">Collection of definitions used by the library.</p></a></li>
    <li><a href="reference" class="btn btn-primary btn-lg">Reference<br><p class="btn-desc">Complete reference of the <code>rpclib</code> API.</p></a></li>
    <li><a href="spec" class="btn btn-primary btn-lg">MsgPack-RPC specification<br><p class="btn-desc">The specification of the Msgpack-RPC protocol, as <code>rpclib</code> implements it</p></a></li>
    <li><a href="roadmap" class="btn btn-primary btn-lg">Roadmap<br><p class="btn-desc">Planned features</p></a></li>
</ul>

<div style="clear:both"></div>

