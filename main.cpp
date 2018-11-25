#include <iostream>

#include "graph.h"

using namespace std;

int main(int argc, char *argv[]) {
	// automata* A = new automata("graph_construct.txt");
	// automata* B = new automata("graph_construct4.txt");
	automata* C = new automata("graph_construct5.txt");

  // A->print();
	// cout << endl;
	// A->Brzozowski();
	// cout << endl;
	// A->Equivalencia();
	// cout << endl;
	// A->equivalence_improved();
	// cout << endl;
	// cout << endl;

	// B->print();
	// cout << endl;
	// //B->Brzozowski();
	// cout << endl;
	// B->Equivalencia();
	// cout << endl;
	// B->equivalence_improved();
	// cout << endl;
	// cout << endl;
	//
	C->print();
	cout << endl;
	C->Brzozowski();
	//C->getTranspose();
	// cout << endl;
	// C->Equivalencia();
	// cout << endl;
	// C->equivalence_improved();
	// cout << endl;
	// cout << endl;

	return 0;
}
