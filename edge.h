#ifndef EDGE_H
#define EDGE_H

#include "node.h"

template <typename G>
class Edge {
    public:
        typedef typename G::E E;
        typedef typename G::node node;

        node* nodes[2]; //Los dos nodos que conecta mirarlo asi: Node* start, Node* end

        Edge(E peso, node* node1, node* node2):data(peso){
            nodes[0]=node1;
            nodes[1]=node2;
    	  };

        E getEdata(){
          return data;
        }

    private:
        E data; //Peso numerico
        bool dir; //Si es direccionado o no

};

#endif
