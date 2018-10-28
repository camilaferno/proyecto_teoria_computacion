#include <iostream>

#include "graph.h"

using namespace std;

int main(int argc, char *argv[]) {
	graph G("graph_construct.txt");
  G.print();
	return 0;
}
