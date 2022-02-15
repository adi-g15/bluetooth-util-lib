* Both .h and .cpp have comments
* `check_for_errors()` instead of `error_check()`, function names should be verbs
* 1. For public functions that user may call, use `camelCase`
  2. For all internal functions, use `_` as word separator
* All variable names, lower case with `_` as separator

* variable name should have unit also, for eg. `u32 time_ms;`
* pointer names should have `*` before name, not with data type, eg. `char *ptr`
* globals all caps with `_`

* struct/class names `camelCase`
* enum names should be `CamelCase`, eg `PinStateType`
* Enum labels should be in Upper Case with _ Word Separators

### Formatting

* One statement, one variable on one line
* Function `{` on next line
* if,while,do,for `{` on same line
* `}` always on next line, and add comments what they are closing
* No space between `functionName` and `(…)`
* Space between `if` and `(…)`

* Max 78 chars on each line
* Never use `goto`, `?:`
* Never use bare naked numbers, instead define a constant, use #define or use enum

* Use const keyword

* Use `.h` for headers
* Check every system call for an error return, unless you know you wish to ignore errors,
In which case you can cast the return to (void).
