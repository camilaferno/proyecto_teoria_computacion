#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <algorithm>
#include <list>
#include <iostream>
#include <string>
#include <fstream>
#include <limits.h>
#include <map>
#include <queue>
#include <stack>

#include "node.h"
#include "edge.h"

using namespace std;

class Traits {
	public:
		typedef char N; //Node es de tipo char
		typedef int E; //Edge es de tipo int
};

template <typename Tr>
class Graph {
    public:
        typedef Graph<Tr> self; //Para definir tipo de grafo
        typedef Node<self> node; //Para definir todos los nodos mas rapido
        typedef Edge<self> edge; //Para definir todos los edges mas rapido
        typedef vector<node*> NodeSeq; //Alias
        typedef list<edge*> EdgeSeq;
        typedef typename Tr::N N; //Alias
        typedef typename Tr::E E; //Alias
        typedef typename NodeSeq::iterator NodeIte;
        typedef typename EdgeSeq::iterator EdgeIte;

    private:
        NodeSeq nodes;
        NodeIte ni;
        EdgeIte ei;


    public:
        Graph(string file):nodes(0){
					ifstream infile(file);
					int numNodos,peso;
					N nombre,nodo1,nodo2;
					double x,y;
					infile >> numNodos;
					for(int i = 0;i < numNodos ; i++){
						infile >> nombre >> x >> y;
						insertNode(nombre,x,y);
					}
					while(infile >> peso >> nodo1 >> nodo2){
            insertEdge(peso,nodo1,nodo2);
					}
				}

        void insertNode(N value, double x,double y){
          for(int i=0; i < nodes.size(); i++){
              if(value == nodes[i]->getNdata()){
                  return;
              }
          }
          node* NewNodo = new node(value,x,y);
          nodes.push_back(NewNodo);
        }


        void insertEdge(E peso, N node1, N node2){
					node* nodoinsert1 = findNode(node1);
					node* nodoinsert2 = findNode(node2);

          edge* NewEdge1 = new edge(peso, nodoinsert1, nodoinsert2);
          nodoinsert1 -> edges.push_back(NewEdge1);
        }

        node* findNode(N node){
					for(ni = nodes.begin(); ni != nodes.end(); ni++){
						if((*ni)->getNdata() == node){
							return (*ni);
							break;
						}
					}
					return nullptr;
				}

        void removeEdge(N node1, N node2){
          for(ni = nodes.begin(); ni != nodes.end(); ni++){
              if((*ni)->getNdata() == node1){
                  for(ei = (*ni)->edges.begin(); ei != (*ni)->edges.end(); ei++){
                      if(((*ei)->nodes[1])->getNdata() == node2){
                          (*ni) -> edges.remove(*ei);
                          break;
                      }
                  }
              }
          }
        }

        void removeNode(N value){
	        for(ni = nodes.begin(); ni != nodes.end(); ni++){
            if((*ni)->getNdata()==value){
                (*ni)->edges.clear(); //borra toda la lista adjacente
                nodes.erase(ni); //borra nodo del vector nodes
								break;
            }
            for(ei = (*ni)->edges.begin(); ei != (*ni)->edges.end(); ei++){
                if((*ei)->nodes[1]->getNdata()==value){
                    (*ni)->edges.remove(*ei);
										break;
                }
            }
	        }
        }

        void print(){
    				if (!nodes.size()) {cout<<"No hay nodos\n"; return;}
            for(auto ni: nodes){
                cout << ni->getNdata() << "| ";
                for(auto item: ni->edges){
                    cout << item->nodes[1]->getNdata() <<":"<<item->getEdata()<<' ';
                }
                    cout << endl;
            }
        }


};

typedef Graph<Traits> graph;

#endif
