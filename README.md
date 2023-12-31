# JSBullshitTranspiler
A program to turn JS source files into proper garbage consisting only of symbols but produces the same output when run by a javascript interpreter like node or a browser.
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

Node does not recognize fs or the requires keyword in the obfuscated script. If you need to pass parameters into the script then

1. Write the script as though the parameters exist already and transpile it.
2. Then open the transpiled scriptfile with a text editor. Insert your code above and below the BS script.
3. The BS script will always start with `(()=>{})[`. Find the matching bracket to the `[` and insert after that your parameters as strings.
4. Pass your parameters at the very end into the function call.

It's unlikely that I add options to include such things automatically. PRs welcome though.

For example

```text
let myVar = 42;
(()=>{})[ ... ]('myVarIdentifierAsUsedInTheBSScript', ... )(myVar)
```

You can also obfuscate that first string, too, by creating a file with just the variable identifier and then pasting that in after mangling it. Just remove the quote marks.



This was shamelessly 'inspired' from here, but rewritten in c++:

https://github.com/lowbyteproductions/JavaScript-Is-Weird

which has this accompanying video

https://www.youtube.com/watch?v=sRWE5tnaxlI
