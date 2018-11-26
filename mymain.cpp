#include <iostream>

#include "graph.h"

using namespace std;

main(int argc, char const *argv[])
{
    automata* aut = new automata("fern2.txt");
    aut->print();
    cout << endl;
    //aut->equivalence_improved();
    
    aut->Hopcroft();
    

    return 0;
}
