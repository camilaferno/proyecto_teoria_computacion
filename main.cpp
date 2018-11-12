#include <iostream>

#include "graph.h"

using namespace std;

int main(int argc, char *argv[]) {
	automata* A = new automata("graph_construct.txt");
  A->printDFA();
	cout << endl;
	//A->getTranspose();
	//cout << endl;
	A->Brzozowski();

	return 0;
}
