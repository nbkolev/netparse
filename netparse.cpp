
#include <config.h>
#include <iostream>
#include "server.h"


int main(int argc, char* argv[])
{
    // Perform smoke tests
    if(!test_tokenizer() ||
    !test_reverse_polish_sort() || !test_evaluate()){
        cout << "Tests fail. Exiting." << endl;
        exit(1);
    }

    //Infinite server listen loop
    serverLoop();
    return 0;
}
