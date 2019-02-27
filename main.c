#include <stdio.h>
#include <string.h>
#include <assert.h>

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
        case '[':;
            char *possible_chars = string2+1;
            int nb = 0;
            // we need to find the other brace to deduce the meaning
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
                    return regex(string1+1, string2+1); // ok! continue
                
            }
            // not ok, we stop
            return 0;
            break;

        // MULTIPLIERS
        case '*':;
            if (strlen(string2-1) < 2) return -1; // ERROR : lonely multiplier
            // getting the char before the multiplier as a separate string
            char* to_compare = strdup(string2-1);
            // getting the char to be compared (will move) as a separate string
            char* compared_to = strdup(string1);
            to_compare[1] = '\0';
            compared_to[1] = '\0';
            while (strlen(string1)>1 && *(string2+1) != *(string1+1)) // until we can exit the '*'. 
            // For now we will exit the multiplier as soon as the regex can continue or if the string is over
            {
                if (regex(compared_to, to_compare) != 1) return 0;// check the regex between the char before * and the string1
                string1++; 
                compared_to = strdup(string1);
                compared_to[1] = '\0';
            }
            return regex(string1, string2+1);
            break;
    }
    if (strlen(string1)>1 && strlen(string2)>1 && *string1==*string2)
            return regex((string1+1),(string2+1));
        else if (*string1==*string2)
            return 1;
        else if (strlen(string1) == 1)
            return 0;
        else
            return regex((string1+1),(string2));

    return 0;
}

int main(int  argc, char* argv[])
{
    char* tested_string = "abcdefghijk";
    printf("\n\nBEGIN TEST\n\n");
    printf("NO SPECIAL CHAR TESTS\n");
    
    assert(regex(tested_string,"abcdef") == 1);
    assert(regex(tested_string,"def") == 1);
    assert(regex(tested_string,"ghij") == 1);
    assert(regex(tested_string,"k") == 1);

    assert(regex(tested_string,"r") == 0);
    assert(regex(tested_string,"abcef") == 0);
    assert(regex(tested_string,"NOP") == 0);
    assert(regex(tested_string,"deff") == 0);
    
    printf("SPECIAL CHAR  : .\n");

    assert(regex(tested_string,"ab.def") == 1);
    assert(regex(tested_string,"de.") == 1);
    assert(regex(tested_string,".hij") == 1);
    assert(regex(tested_string,".") == 1);

    assert(regex(tested_string,"ne.u") == 0);
    assert(regex(tested_string,"a.ef") == 0);
    assert(regex(tested_string,"..aaa") == 0);
    assert(regex(tested_string,"ab.c") == 0);

    printf("SPECIAL CHAR  : *\n");

    assert(regex(tested_string,"ab.*f") == 1);
    assert(regex(tested_string,"de*") == 1);
    assert(regex(tested_string,".*hij") == 1);
    assert(regex(tested_string,"abc*def") == 1);

    assert(regex(tested_string,"*abc") == -1);
    assert(regex(tested_string,".*abc") == 0);
    assert(regex(tested_string,"a*aa") == 0);
    assert(regex(tested_string,"ijk*") == 0);

    printf("SPECIAL CHARs : []\n");

    assert(regex("bonjour", "b[oau]njour") == 1);
}
