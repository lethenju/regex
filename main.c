#include <stdio.h>
#include <string.h>
#include <assert.h>

typedef enum 
{
    REGEX_MODE_NORMAL,
    REGEX_MODE_BEGIN,
    REGEX_MODE_END
} regex_mode;

static int hook = 0;
static regex_mode mode = REGEX_MODE_NORMAL;


int regex(char* string1, char* string2)
{
    switch (*string2)
    {
        case '.':
            if (strlen(string1)>1 && strlen(string2)>1)
                return regex((string1+1),(string2+1));
            else
                return 1;
            break;
        case '(':; 
            char *ptr_to_first_word = string2+1;
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
            char* ptr_to_last_word = ptr_to_first_word;
            nb_w = 0;
            while (*string2 != ')')
            {
                if (*string2 == '|')
                {
                    *(possible_words + nb_w) = strdup(ptr_to_last_word);
                    char* word_end = *(possible_words + nb_w)+ (string2 - ptr_to_last_word);
                    *word_end = '\0';
                    nb_w++;
                    string2++; // stepping the |
                    ptr_to_last_word = string2;

                }
                string2++;
            }
            *(possible_words + nb_w) = strdup(ptr_to_last_word);
            char* word_end = *(possible_words + nb_w)+ (string2 - ptr_to_last_word);
            *word_end = '\0';
            nb_w++;
            ptr_to_last_word = string2;
            int word_index;
            for (word_index = 0; word_index < nb_w; word_index++) 
            {
                // We now have the different word possibilities. 
                // We now need to regex(string1, each word) to know if its okay
                mode = REGEX_MODE_BEGIN; // we shouldnt check if the word exist somewhere in string1, but we have to be strict
                if (regex(string1, *(possible_words+word_index))) {
                    // So its okay, we found a matching word
                    // ISSUE : it will find it on all the string, we have to ensure it find it at the good spot
                    // Maybe add "^"
                    if (strlen(string2) == 1) return 1; // if string2 = ')' it means the regex is over and we're done
                    // Now we have to simply step after the word to continue the regex
                    if (regex(string1+strlen(*(possible_words+word_index)), string2+1)) 
                        return 1;
                    // NOTE : we're not returning directly regex, before if it is 0, we still have to check the other words.
                }
            }
            return 0;mode = REGEX_MODE_BEGIN; // we shouldnt check if the word exist somewhere in string1, but we have to be strict

            break;
        case '[':;
            char *possible_chars = string2+1;
            int nb = 0;
            // we need to find the other brace to deduce the possible chars
            while (*string2 != ']')
            {
                nb++;
                string2++;
            }
            // So now for one char it can be anything from possible_chars
            int i;
            for (i = 0; i < nb; i++) {
                if (*string1 == *(possible_chars+i) || // Same char
                            (*(possible_chars+i) == '-'  // Interval
                            && *string1 > *(possible_chars+i-1) 
                            && *string1 < *(possible_chars+i+1)))
                    {   
                        if (strlen(string2) == 1)
                            return 1;
                        return regex(string1+1, string2+1); // ok! continue
                    } 
                
            }
            // not ok, we stop
            hook=0;
            return 0;
            break;

        // MULTIPLIERS
        case '*':;
            if (strlen(string2-1) < 2) return -1; // ERROR : lonely multiplier
            // getting the char before the multiplier as a separate string
            char* to_compare = strdup(string2-1);
            // getting the char to be compared (will move) as a separate string
            char* compared_to = strdup(string1-1);
            to_compare[1] = '\0';
            compared_to[1] = '\0';
            while (strlen(string1)>1 && *(string2+1) != *(string1+1) &&  regex(compared_to, to_compare) == 1) // until we can exit the '*'. 
            // For now we will exit the multiplier as soon as the regex can continue or if the string is over
            {
                /*if (regex(compared_to, to_compare) != 1) 
                {
                    hook=0;
                    return 0;// check the regex between the char before * and the string1
                }*/
                string1++; 
                compared_to = strdup(string1);
                compared_to[1] = '\0';
            }
            hook=0; // reset hook
            if (strlen(string2)>1)
                return regex(string1-1, string2+1);
            return 1;
            break;
    }

    if (mode == REGEX_MODE_BEGIN)
    {
        if (*string1 == *string2)
            if (strlen(string1)>1 && strlen(string2)>1)
                return regex((string1+1),(string2+1));
            else
                return 1;
        return 0;
    }

    if (strlen(string1)>1 && strlen(string2)>1 && *string1==*string2) {
            hook=1;
            return regex((string1+1),(string2+1));
        }
        else if (*string1==*string2 && strlen(string2) == 1)
            return 1;
        else if (strlen(string1) == 1){
            hook=0;
            return 0;
        }
        else if (hook == 0)
            return regex((string1+1),(string2));
    hook=0;
    return 0;
}

int regex_wrap(char* string1, char* string2)
{
    int i;

    // begin & end char
    if (*string2 == '^') 
    {
        mode = REGEX_MODE_BEGIN;
        string2++;
    } else if (*(string2+strlen(string2)-1) == '$') // getting last char
    {
        mode = REGEX_MODE_END;
        char* word_end = (string2+strlen(string2)-1);
        // To make it work, we should maybe turn the regex upside down : we should check every char from the end..
        // the issue is that the multipliers wont work
        // example : ooour ->  o*ur$
        // We should check the r then the u then the o, multiplied.
        // So we need to transform the strings to
        //           ruooo -> ^ruo*
        // string1 gets inversed easily
        // string2 gets inversed, then inverse the char '*' with the char after it, then change the * with ^
        // And the {} shouldnt get inversed
        char* string1_i = malloc(sizeof(string1));
        char* string2_i = malloc(sizeof(string2));
        
        for ( i = strlen(string1) - 1; i >= 0 ; i--)
        {
            string1_i[i] = *string1;
            string1++;
        }

        for ( i = strlen(string2) - 1; i >= 0 ; i--)
        {
            string2_i[i] = *string2;
            string2++;
        }
        *string2_i = '^';
        // now we need to take care of multipliers
        // TODO Take care of '{}' multiplier
        for ( i = 0; i < strlen(string2_i) ; i++)
        {
            if (string2_i[i] == '*')
            {
                //if (strlen(string2_i+i) == 1) // syntax error : multiplier without arg
                  //  return -1;

                // INVERSING
                string2_i[i] = string2_i[i+1];
                string2_i[i+1] = '*';
                break;
            }
        }
        return regex_wrap(string1_i, string2_i);

    } // Doesnt support both modes at the same time
    
    int result = regex(string1, string2);
    hook = 0;
    return result;
}

int main(int  argc, char* argv[])
{
    char* tested_string = "abcdefghijk";
    printf("\n\nBEGIN TEST\n\n");
    printf("NO SPECIAL CHAR TESTS\n");

    assert(regex_wrap(tested_string,"abcdef") == 1);
    assert(regex_wrap(tested_string,"def") == 1);
    assert(regex_wrap(tested_string,"ghij") == 1);
    assert(regex_wrap(tested_string,"k") == 1);

    assert(regex_wrap(tested_string,"r") == 0);
    assert(regex_wrap(tested_string,"abcef") == 0);
    assert(regex_wrap(tested_string,"NOP") == 0);
    assert(regex_wrap(tested_string,"deff") == 0);
    
    printf("TESTS PASSED  \n");

    printf("SPECIAL CHAR  : .\n");

    assert(regex_wrap(tested_string,"ab.def") == 1);
    assert(regex_wrap(tested_string,"de.") == 1);
    assert(regex_wrap(tested_string,".hij") == 1);
    assert(regex_wrap(tested_string,".") == 1);

    assert(regex_wrap(tested_string,"ne.u") == 0);
    assert(regex_wrap(tested_string,"a.ef") == 0);
    assert(regex_wrap(tested_string,"..aaa") == 0);
    assert(regex_wrap(tested_string,"ab.c") == 0);

    printf("TESTS PASSED  \n");

    printf("SPECIAL CHAR  : *\n");

    assert(regex_wrap(tested_string,"ab.*f") == 1);
    assert(regex_wrap(tested_string,"de*") == 1);
    assert(regex_wrap(tested_string,".*hij") == 1);
    assert(regex_wrap(tested_string,"abc*def") == 1);

    assert(regex_wrap(tested_string,"*abc") == -1);

    assert(regex_wrap(tested_string,".*abc") == 0);
    assert(regex_wrap(tested_string,"a*aa") == 0);
    assert(regex_wrap(tested_string,"ijkkk*") == 0);

    printf("TESTS PASSED  \n");

    printf("SPECIAL CHARs : []\n");

    assert(regex_wrap(tested_string, "ab[ca]defghijk") == 1);
    assert(regex_wrap(tested_string, "[aA]bcdef") == 1);
    assert(regex_wrap(tested_string, "abcde[fFaor]") == 1);
    assert(regex_wrap(tested_string, "ab[cC]de[fFaor]") == 1);


    assert(regex_wrap(tested_string, "ab[ab]defghijk") == 0);
    assert(regex_wrap(tested_string, "[Bb]bcdef") == 0);
    assert(regex_wrap(tested_string, "abcde[aor]") == 0);
    assert(regex_wrap(tested_string, "ab[OO]de[fFaor]") == 0);


    printf("SPECIAL CHARs : [ - ]\n");

    assert(regex_wrap(tested_string, "ab[a-c]defghijk") == 1);
    assert(regex_wrap(tested_string, "[a-Z]bcdef") == 1);
    assert(regex_wrap(tested_string, "abcde[a-z]") == 1);
    assert(regex_wrap(tested_string, "ab[a-e]de[d-g]") == 1);


    assert(regex_wrap(tested_string, "ab[A-Z]defghijk") == 0);
    assert(regex_wrap(tested_string, "[1-9]bcdef") == 0);
    assert(regex_wrap(tested_string, "abcde[o-o]") == 0);
    assert(regex_wrap(tested_string, "ab[A-Z]de[a-f]") == 0);

    printf("SPECIAL CHARs : ( | )\n");

    assert(regex_wrap(tested_string, "ab(alors|cde)fghijk") == 1);
    assert(regex_wrap(tested_string, "(abc|def|ijk)def") == 1);
    assert(regex_wrap(tested_string, "abcde(fgh|ll)") == 1);
    assert(regex_wrap(tested_string, "ab(cde|c)de(fg|h)") == 1);


    assert(regex_wrap(tested_string, "ab(123|hey)defghijk") == 0);
    assert(regex_wrap(tested_string, "(abc|aaa)deffuie") == 0);
    assert(regex_wrap(tested_string, "abcde(fge|o|lol)") == 0);

    printf("BEGIN CHARs : ^\n");

    assert(regex_wrap(tested_string, "^abc") == 1);
    assert(regex_wrap(tested_string, "^(abc|def|ijk)def[a-z]hi.k") == 1);

    assert(regex_wrap(tested_string, "^defhg)") == 0);
    assert(regex_wrap(tested_string, "^[A-C](def|hij))") == 0);


    printf("END CHARs : $\n");

    assert(regex_wrap(tested_string, "hi*jk$") == 1);
    assert(regex_wrap(tested_string, "hijk$") == 1);

    assert(regex_wrap(tested_string, "abc$") == 0);
    assert(regex_wrap(tested_string, "ij[abc]$") == 0);


    printf("TESTS PASSED  \n");
}