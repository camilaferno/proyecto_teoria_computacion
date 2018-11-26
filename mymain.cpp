#include <iostream>

#include "graph.h"

using namespace std;

main(int argc, char const *argv[])
{
    automata* aut = new automata("e2_aut_1.txt");
    aut->print();
    cout << endl;
    
    automata* brz = aut->Brzozowski();
    cout << "BRO" << endl;
    brz->printNFA();

    vector<vector<bool>> m = aut->Equivalencia();
    cout << "EQ1" << endl;
    for (int i = 0; i < aut->numberOfStates; i++)
    {
        for (int j = 0; j < aut->numberOfStates; j++)
        {
            cout << m[i][j] << " ";
        }   
        cout << endl;
    }

    cout << endl;
    
    vector<vector<bool>> m_2 = aut->equivalence_improved();
    cout << "EQUIPLAZ" << endl;
    for (int i = 0; i < aut->numberOfStates; i++)
    {
        cout << aut->states[i]->data << " ";
        for (int j = 0; j < aut->numberOfStates; j++)
        {
            cout << m_2[i][j] << " ";
        }   
        cout << endl;
    }
    cout << "  ";
    for (int i = 0; i < aut->numberOfStates; i++)
    {
        cout << aut->states[i]->data << " ";
    }
    cout << endl;

    cout << endl;
    cout << "MUR" << endl;
    automata* mur = aut->moore();
    mur->print();

    cout << endl;
    cout << "JOP" << endl;
    automata* hopboi = aut->Hopcroft();
    hopboi->print();


    return 0;
}
