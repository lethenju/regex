
#include <assert.h>
#include <stdio.h>
#include "regex.h"


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
    
    printf("TESTS PASSED  \n");

    printf("SPECIAL CHAR  : .\n");

    assert(regex(tested_string,"ab.def") == 1);
    assert(regex(tested_string,"de.") == 1);
    assert(regex(tested_string,".hij") == 1);
    assert(regex(tested_string,".") == 1);

    assert(regex(tested_string,"ne.u") == 0);
    assert(regex(tested_string,"a.ef") == 0);
    assert(regex(tested_string,"..aaa") == 0);
    assert(regex(tested_string,"ab.c") == 0);

    printf("TESTS PASSED  \n");

    printf("SPECIAL CHAR  : *\n");

    assert(regex(tested_string,"ab.*cdef") == 1);
    assert(regex(tested_string,"de*") == 1);
    assert(regex(tested_string,".*hij") == 1);
    assert(regex(tested_string,"abc*def") == 1);
    assert(regex(tested_string,".*abc") == 1);

    assert(regex(tested_string,"*abc") == -1);

    assert(regex(tested_string,"a*aa") == 0);
    assert(regex(tested_string,"ijkkk*") == 0);


    printf("SPECIAL CHAR  : +\n");

    assert(regex(tested_string,"ab.+f") == 1);
    assert(regex(tested_string,"de+") == 1);
    assert(regex(tested_string,".+hij") == 1);
    assert(regex(tested_string,"abc+def") == 1);

    assert(regex(tested_string,"+abc") == -1);

    assert(regex(tested_string,".+abc") == 0);
    assert(regex(tested_string,"a+aa") == 0);
    assert(regex(tested_string,"ijkkk+") == 0);


    printf("SPECIAL CHARs : {}  \n");
    
    char* testing_string_multiplier =  "abbcccdddd"; 

    assert(regex(testing_string_multiplier, "ab{2}") == 1);
    assert(regex(testing_string_multiplier, "ab{2}c{3}d") == 1);

    assert(regex(testing_string_multiplier, "ab{3}") == 0);
    assert(regex(testing_string_multiplier, "ab{2}c{3}d{5}") == 0);


    printf("SPECIAL CHAR  : ?  \n");

    assert(regex(testing_string_multiplier, "abb?c") == 1);
    assert(regex(testing_string_multiplier, "abbb?c") == 1);

    assert(regex(testing_string_multiplier, "ab?c") == 0);
    assert(regex(testing_string_multiplier, "abbbb?c") == 0);

    printf("SPECIAL CHARs : []\n");

    assert(regex(tested_string, "ab[ca]defghijk") == 1);
    assert(regex(tested_string, "[aA]bcdef") == 1);
    assert(regex(tested_string, "abcde[fFaor]") == 1);
    assert(regex(tested_string, "ab[cC]de[fFaor]") == 1);


    assert(regex(tested_string, "ab[ab]defghijk") == 0);
    assert(regex(tested_string, "[Bb]bcdef") == 0);
    assert(regex(tested_string, "abcde[aor]") == 0);
    assert(regex(tested_string, "ab[OO]de[fFaor]") == 0);


    printf("SPECIAL CHARs : [ - ]\n");

    assert(regex(tested_string, "ab[a-c]defghijk") == 1);
    assert(regex(tested_string, "[a-Z]bcdef") == 1);
    assert(regex(tested_string, "abcde[a-z]") == 1);
    assert(regex(tested_string, "ab[a-e]de[d-g]") == 1);

    assert(regex(tested_string, "ab[0-9a-c]defghijk") == 1);
    assert(regex(tested_string, "[a-Z0-5]bcdef") == 1);
    assert(regex(tested_string, "abcde[a-jj-z]") == 1);
    assert(regex(tested_string, "ab[a-cd-e]de[f-ga-z]") == 1);



    assert(regex(tested_string, "ab[A-Z0-9]defghijk") == 0);
    assert(regex(tested_string, "[1-9A-Z]bcdef") == 0);

    printf("SPECIAL CHARs : ( | )\n");

    assert(regex(tested_string, "ab(alors|cde)fghijk") == 1);
    assert(regex(tested_string, "(abc|def|ijk)def") == 1);
    assert(regex(tested_string, "abcde(fgh|ll)") == 1);
    assert(regex(tested_string, "ab(cde|c)de(fg|h)") == 1);


    assert(regex(tested_string, "ab(123|hey)defghijk") == 0);
    assert(regex(tested_string, "(abc|aaa)deffuie") == 0);
    assert(regex(tested_string, "abcde(fge|o|lol)") == 0);

    printf("BEGIN CHARs : ^\n");

    assert(regex(tested_string, "^abc") == 1);
    assert(regex(tested_string, "^(abc|def|ijk)def[a-z]hi.k") == 1);

    assert(regex(tested_string, "^defhg)") == 0);
    assert(regex(tested_string, "^[A-C](def|hij))") == 0);


    printf("END CHARs : $\n");

    assert(regex(tested_string, "hijk$") == 1);
    assert(regex(tested_string, "fghi*jk$") == 1);

    assert(regex(tested_string, "abc$") == 0);
    assert(regex(tested_string, "ij[abc]$") == 0);

    printf("BOTH BEGIN AND END CHARs : ^$\n");

    assert(regex(tested_string, "^ab.*jk$") == 1);
    assert(regex(tested_string, "^a.*k$") == 1);

    assert(regex(tested_string, "^abc$") == 0);
    assert(regex(tested_string, "^ij[abc]$") == 0);


    printf("CHARACTER CLASSES : \\d\n");
    
    char* character_test_string = "ab1c2d34:!/";
    // need to escape it for now
    assert(regex(character_test_string, "^ab\\dc") == 1);
    assert(regex(character_test_string, "d\\d\\d") == 1);

    assert(regex(character_test_string, "ab\\d\\d") == 0);
    assert(regex(character_test_string, "\\d\\d\\d") == 0);

    printf("CHARACTER CLASSES : \\D\n");

    assert(regex(character_test_string, "^\\D\\D1\\D\\d") == 1);
    assert(regex(character_test_string, "\\D34") == 1);

    assert(regex(character_test_string, "ab\\D") == 0);
    assert(regex(character_test_string, "\\D\\D\\D") == 0);


    printf("CHARACTER CLASSES : \\w\n");


    assert(regex(character_test_string, "ab1c2\\w\\w\\w:!/") == 1);
    assert(regex(character_test_string, "\\w\\w1c2") == 1);

    assert(regex(character_test_string, "ab1c2d34\\w\\w") == 0);
    assert(regex(character_test_string, "\\wab") == 0);

    printf("CHARACTER CLASSES : \\W\n");

    assert(regex(character_test_string, "d34\\W!") == 1);

    assert(regex(character_test_string, "ab\\W") == 0);
    assert(regex(character_test_string, "\\W\\W\\W\\W") == 0);



    printf("TESTS PASSED  \n");
}
