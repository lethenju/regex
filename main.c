#include <stdio.h>
#include <string.h>

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
        case '*':
            if (strlen(string2) == 1) return 1;
            while (strlen(string1)>1 && *(string2+1) != *(string1+1))
                string1++;
            return regex(string1+1, string2+1);
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
    }
    if (*string1 == *string2)
    {
        if (strlen(string1)>1 && strlen(string2)>1)
                return regex((string1+1),(string2+1));
            else
                return 1;
    }
    return 0;

}

int main(int  argc, char* argv[])
{
    printf("1 Test with bonjour and bonjour : %d\n", regex("bonjour", "bonjour"));
    printf("1 Test with bonjour and bon : %d\n", regex("bonjour", "bon"));
    printf("1 Test with bonjour and bon.our : %d\n", regex("bonjour", "bon.our"));

    printf("1 Test with bonjour and bon*r : %d\n", regex("bonjour", "bon*r"));
    printf("1 Test with bonjour and bon* : %d\n", regex("bonjour", "bon*"));
    printf("1 Test with bonjour and * : %d\n", regex("bonjour", "*"));
    printf("1 Test with bonjour and *nj*r : %d\n", regex("bonjour", "*nj*r"));

    printf("1 Test with bonjour and b[oau]njour : %d\n", regex("bonjour", "b[oau]njour"));
    printf("1 Test with bonjour and b[oau]njou[abcder] : %d\n", regex("bonjour", "b[oau]njou[abcder]"));
    
    printf("1 Test with bonjour and b[a-z]njour : %d\n", regex("bonjour", "b[a-z]njour"));



    printf("0 Test with bonjour and b[A-Z]njour : %d\n", regex("bonjour", "b[A-Z]njour"));
    printf("0 Test with bonjour and b[au]njour : %d\n", regex("bonjour", "b[au]njour"));
    printf("0 Test with bonjour and b[oau]njou[abcde] : %d\n", regex("bonjour", "b[oau]njou[abcde]"));
    printf("0 Test with bonjour and salut : %d\n", regex("bonjour", "salut"));
    printf("0 Test with bonjour and bonour : %d\n", regex("bonjour", "bonour"));    
    printf("0 Test with bonjour and *nk*r : %d\n", regex("bonjour", "*nk*r"));
}