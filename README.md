# JSBullshitTranspiler
A program to turn JS source files into proper garbage consisting only of symbols but produces the same output when run by a javascript interpreter like node.

The transpiled programs will not run in web browsers because it calls functions that were constructed from a string, which modern browsers prohibit.

The output is the same filename as the input, but prefixed with `BS_`. If such a file exists, it is overwritten in place.

To build:
```bash
make
```

To run
```bash
./main myJavascriptScript.js
```

Then execute the transpiled script
```bash
node ./BS_myJavascriptScript.js
```

This was shamelessly 'inspired' from here, but rewritten in c++:

https://github.com/lowbyteproductions/JavaScript-Is-Weird

which has this accomanying video

https://www.youtube.com/watch?v=sRWE5tnaxlI
