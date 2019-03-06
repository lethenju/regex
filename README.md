# RegEx

Simple Regular Expressions parser in C.

You can test if a string matches a regular expression

## Usage

See the `test.c` file to understand how you can use the project

Here is a simple example

```c
#include <stdio.h>
#include "regex.h"


int main(int argc, char* argv[])
{
    char* test_string = "test@gmail.com"

    if (regex(test_string, "^.*@(gmail|yahoo|microsoft).com))
        printf("This mail is good\n");
    return 0;
}
```
## Installation

Clone the repository and :
```sh
> make all   # compile lib and test
> make test  # compile test
> make lib   # compile lib
> make clean # delete object files and executables
```

## Features

For now, not every regex feature is implemented.

Only thoses are working fine :
- '.' any char
- '*' multiplier 0 to any
- '+' multiplier 1 to any
- '?' multiplier 0 to 1
- '{nb}' multiplier to nb
- '[abc]' choice in char
- '[a-z]' choice in char with interval
- '(ab|cd)' choice in word
- '^' begin char
- '$' end char
- '\d' digit
- '\D' not a digit
- '\w' word character
- '\W' not a word character

## Limitations

- need to escape antislashes for \d etc : '\\d' and so we cannot escape antislash as a whole
- no support for inverser in [] or ()
- no support for multiplier after a character class or a [] or ()
- no support for [-] with $
- no support for {2,} or {2,3}
- no support for greediness and lazyness in multipliers (greedy by default)
- no support for \s, \S
- no support for escaping normal characters
- no support for referencing group words (\1, \2)
