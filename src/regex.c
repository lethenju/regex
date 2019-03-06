#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef enum
{
    REGEX_MODE_NORMAL,
    REGEX_MODE_BEGIN,
    REGEX_MODE_END
} regex_mode;

static int hook = 0;
static regex_mode mode = REGEX_MODE_NORMAL;

int handle_dot(char *string1, char *string2)
{
    if (strlen(string1) > 1 && strlen(string2) > 1)
        return regex_int((string1 + 1), (string2 + 1));
    else
        return 1;
}

int handle_choice_word(char *string1, char *string2)
{
    char *ptr_to_first_word = string2 + 1;
    int nb_w = 0;
    // first round to know nb of words
    while (*string2 != ')')
    {
        if (*string2 == '|')
            nb_w++;
        string2++;
    }
    char **possible_words = malloc(nb_w);

    // secound round to actually allocate
    string2 = ptr_to_first_word;
    char *ptr_to_last_word = ptr_to_first_word;
    nb_w = 0;
    while (*string2 != ')')
    {
        if (*string2 == '|')
        {
            *(possible_words + nb_w) = strdup(ptr_to_last_word);
            char *word_end = *(possible_words + nb_w) + (string2 - ptr_to_last_word);
            *word_end = '\0';
            nb_w++;
            string2++; // stepping the |
            ptr_to_last_word = string2;
        }
        string2++;
    }
    *(possible_words + nb_w) = strdup(ptr_to_last_word);
    char *word_end = *(possible_words + nb_w) + (string2 - ptr_to_last_word);
    *word_end = '\0';
    nb_w++;
    ptr_to_last_word = string2;
    int word_index;
    for (word_index = 0; word_index < nb_w; word_index++)
    {
        // We now have the different word possibilities.
        // We now need to regex_int(string1, each word) to know if its okay
        mode = REGEX_MODE_BEGIN; // we shouldnt check if the word exist somewhere in string1, but we have to be strict
        if (regex_int(string1, *(possible_words + word_index)))
        {
            // So its okay, we found a matching word
            // ISSUE : it will find it on all the string, we have to ensure it find it at the good spot
            // Maybe add "^"
            if (strlen(string2) == 1)
                return 1; // if string2 = ')' it means the regex is over and we're done
            // Now we have to simply step after the word to continue the regex
            if (regex_int(string1 + strlen(*(possible_words + word_index)), string2 + 1))
                return 1;
            // NOTE : we're not returning directly regex, before if it is 0, we still have to check the other words.
        }
    }
    return 0; // we shouldnt check if the word exist somewhere in string1, but we have to be strict
}

int handle_choice_char(char *string1, char *string2)
{
    char *possible_chars = string2 + 1;
    int nb = 0;
    // we need to find the other brace to deduce the possible chars
    while (*string2 != ']')
    {
        nb++;
        string2++;
    }
    // So now for one char it can be anything from possible_chars
    int i;
    for (i = 0; i < nb; i++)
    {
        if (*string1 == *(possible_chars + i) || // Same char
            (*(possible_chars + i) == '-'        // Interval
             && *string1 > *(possible_chars + i - 1) && *string1 < *(possible_chars + i + 1)))
        {
            if (strlen(string2) == 1)
                return 1;
            return regex_int(string1 + 1, string2 + 1); // ok! continue
        }
    }
    // not ok, we stop
    hook = 0;
    return 0;
}

int handle_multiplier(char *string1, char *string2, int number_min, int number_max)
{
    if (strlen(string2 - 1) < 2)
        return -1; // ERROR : lonely multiplier
    // getting the char before the multiplier as a separate string
    char *to_compare = strdup(string2 - 1);
    // getting the char to be compared (will move) as a separate string
    char *compared_to = strdup(string1);
    to_compare[1] = '\0';
    compared_to[1] = '\0';
    char *ptr = string1;
    int count = 0;

    while (strlen(string1) > 1 
    && regex_int(compared_to, to_compare)) // until we can exit the '*'.
    // For now we will exit the multiplier as soon as the regex can continue or if the string is over
    {
        count++;
        string1++;
        compared_to = strdup(string1);
        compared_to[1] = '\0';
    }
    hook = 0; // reset hook
    if (count < number_min) return 0;
    if (number_max > 0 && count > number_max) return 0;
    if (strlen(string2) > 1)
    {
        mode = REGEX_MODE_NORMAL; // Workaround! to be fixed
        return regex_int(ptr + number_min, string2 + 1);
    }
    return 1;
}

int handle_char_d(char* string1, char* string2)
{
    // transform \d in [0-9]
    char* new_string = malloc(sizeof(string2)+3);
    strcpy(new_string, "[0-9]");
    strcpy(new_string+5, string2+2);
    return regex_int(string1,new_string);
}

int handle_char_D(char* string1, char* string2)
{
    // transform \D in [ -/:-~] space to slash, ":" to tilde
    char* new_string = malloc(sizeof(string2)+6);
    strcpy(new_string, "[ -/:-~]");
    strcpy(new_string+8, string2+2);
    return regex_int(string1,new_string);
}

int handle_char_w(char* string1, char* string2)
{
    // transform \w in [0-9A-Za-z ]
    char* new_string = malloc(sizeof(string2)+10);
    strcpy(new_string, "[0-9A-Za-z ]");
    strcpy(new_string+12, string2+2);
    return regex_int(string1,new_string);
}

int handle_char_W(char* string1, char* string2)
{
    // transform \D in [ -/:-@[-`{-~] space to slash, ":" to tilde
    char* new_string = malloc(sizeof(string2)+12);
    strcpy(new_string, "[!-/:-@[-`{-~]");
    strcpy(new_string+14, string2+2);
    return regex_int(string1,new_string);
}


int regex_int(char *string1, char *string2)
{
    switch (*(string2 + 1))
    {
    case '*':;
        return handle_multiplier(string1, string2 + 1, 0, 0);
    case '+':;
        return handle_multiplier(string1, string2 + 1, 1, 0);
    case '?':;
        return handle_multiplier(string1, string2 + 1, 0, 1);
    case '{':; // We need to know what is the number asked
        char *old_str2 =  strdup(string2);
        char *numbers = strdup(string2 + 2);
        int count = 0;
        while (*(string2+1) != '}')
        {
            string2++;
            count++;
        }
        numbers[count-1] = '\0'; // terminating number
        int nb = atoi(numbers);
        // We should transform the "{138}" by "{"
        // to do that, we have to copy all that is after the * to after the {
        // then to tranform the { by a *
        if (strlen(old_str2+count+2) > 0) // old char + { + the count after
        {                                 // we're actually verifying there is something after '}'
            strcpy(old_str2+2, old_str2+count+2);
        } else {
            old_str2[2] = '\0';
        }
        return handle_multiplier(string1, old_str2 + 1, nb, 0);
        break;
    }
    switch (*string2)
    {
    case '\\':;
        switch (*(string2+1))
        {
            case 'd':;
                return handle_char_d(string1, string2);
            case 'D':;
                return handle_char_D(string1, string2);
            case 'w':;
                return handle_char_w(string1, string2);
            case 'W':;
                return handle_char_W(string1, string2);
        }
    case '.':
        return handle_dot(string1, string2);
    case '(':
        return handle_choice_word(string1, string2);
    case '[':;
        return handle_choice_char(string1, string2);
    }

    if (mode == REGEX_MODE_BEGIN)
    {
        if (*string1 == *string2)
            if (strlen(string1) > 1 && strlen(string2) > 1)
                return regex_int((string1 + 1), (string2 + 1));
            else
                return 1;
        return 0;
    }

    if (strlen(string1) > 1 && strlen(string2) > 1 && *string1 == *string2)
    {
        hook = 1;
        return regex_int((string1 + 1), (string2 + 1));
    }
    else if (*string1 == *string2 && strlen(string2) == 1)
        return 1;
    else if (strlen(string1) == 1)
    {
        hook = 0;
        return 0;
    }
    else if (hook == 0)
        return regex_int((string1 + 1), (string2));
    hook = 0;
    return 0;
}

int regex(char *string1, char *string2)
{
    int i;

    // begin & end char
    if (*string2 == '^')
    {
        mode = REGEX_MODE_BEGIN;
        string2++;
        if (*(string2 + strlen(string2) - 1) == '$') // getting last char
        {
            char* string1_backup = strdup(string1); // cause both strings will be heavily modified during the regex
            char* string2_backup = strdup(string2);
            // Both ^ and $ : return with ^ alone and $ alone 
            int with_end = regex(string1, string2);
            string2_backup[strlen(string2_backup) - 1] = '\0'; // removing $
            int with_begin = regex(string1_backup, string2_backup);
            return with_end*with_begin;
        }
    }
    else if (*string2 == '*' || *string2 == '+')
    {
        return -1;
    }
    else if (*(string2 + strlen(string2) - 1) == '$') // getting last char
    {
        mode = REGEX_MODE_END;
        char *word_end = (string2 + strlen(string2) - 1);
        // To make it work, we should maybe turn the regex upside down : we should check every char from the end..
        // the issue is that the multipliers wont work
        // example : ooour ->  o*ur$
        // We should check the r then the u then the o, multiplied.
        // So we need to transform the strings to
        //           ruooo -> ^ruo*
        // string1 gets inversed easily
        // string2 gets inversed, then inverse the char '*' with the char after it, then change the * with ^
        // And the {} and [-] shouldnt get inversed
        char *string1_i = malloc(sizeof(string1));
        char *string2_i = malloc(sizeof(string2));

        for (i = strlen(string1) - 1; i >= 0; i--)
        {
            string1_i[i] = *string1;
            string1++;
        }

        for (i = strlen(string2) - 1; i >= 0; i--)
        {
            string2_i[i] = *string2;
            string2++;
        }
        *string2_i = '^';
        // now we need to take care of multipliers
        // TODO Take care of '{}' multiplier
        for (i = 0; i < strlen(string2_i); i++)
        {
            if (string2_i[i] == '*')
            {
                // INVERSING
                string2_i[i] = string2_i[i + 1];
                string2_i[i + 1] = '*';
                break;
            }
        }
        return regex(string1_i, string2_i);

    } // Doesnt support both modes at the same time

    int result = regex_int(string1, string2);
    hook = 0;
    mode = REGEX_MODE_NORMAL;
    return result;
}
