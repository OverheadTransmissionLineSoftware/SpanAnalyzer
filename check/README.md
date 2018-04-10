The following tools check the code and identify errors. Linux is the only
platform that scripts have been adapted for.

## OCLint
OCLint checks the logic statements.

Runs a static code analysis.
```
cd check
./run-oclint.sh <path_to_oclint_exectutable>
```

View the report by opening the `check/oclint_log.html` file in a browser. See
the OCLint [website](http://oclint.org/) for more details.

## cpplint
cpplint checks conformance to the formatting guidelines.

Runs a formatting check.
```
cd check
./run-cpplint.sh
```

Output will be displayed in the terminal. See the
[website](https://google.github.io/styleguide/cppguide.html) for more details.
