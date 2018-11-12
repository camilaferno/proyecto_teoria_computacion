#include <iostream>

#include "graph.h"

using namespace std;

int main(int argc, char *argv[]) {
	automata* A = new automata("graph_construct3.txt");
  A->printDFA();
	cout << endl;
	//A->getTranspose();
	//cout << endl;
	A->Brzozowski();
	//A->Equivalencia();
	//A->renombramiento();

	return 0;
}
