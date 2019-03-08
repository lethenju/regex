#ifndef REGEX_H
#define REGEX_H

/** tests a regex against an input string
 *  @param input : the input string to test
 *  @param regex : the regex used to test the input string
 *  @return 1 if the input is okay, 0 if it isnt, -1 if the regex is incorrect.
 */
int regex(char* input, char* regex);

#endif
