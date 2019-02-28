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
- '[abc]' choice in char
- '[a-z]' choice in char with interval
- '(ab|cd)' choice in word
- '^' begin char
- '$' end char

## Known limitations

- Bug when using $ end chars and * multiplier
