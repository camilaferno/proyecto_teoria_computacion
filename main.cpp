#include <iostream>

#include "graph.h"

using namespace std;

int main(int argc, char *argv[]) {
	graph G(0,"graph_construct.txt");
  G.print();
	return 0;
}
