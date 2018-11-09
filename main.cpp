#include <iostream>

#include "graph.h"

using namespace std;

int main(int argc, char *argv[]) {
	graph G("graph_construct2.txt");
  G.print();
	cout << endl;

	G.NFAtoDFA();
	return 0;
}
