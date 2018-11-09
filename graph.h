#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <algorithm>
#include <list>
#include <iostream>
#include <string>
#include <fstream>
#include <limits.h>
#include <set>
#include <queue>

#include "node.h"
#include "edge.h"

using namespace std;

class Traits {
	public:
		typedef string N; //Node es de tipo string
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
        Graph():nodes(0){}
        Graph(string file):nodes(0){
					ifstream infile(file);
					int numNodos,peso;
					N nombre,nodo1,nodo2;
          bool entrada,salida;
					infile >> numNodos;
					for(int i = 0;i < numNodos ; i++){
						infile >> nombre >> entrada >> salida;
						insertNode(nombre,entrada,salida);
					}
					while(infile >> peso >> nodo1 >> nodo2){
            insertEdge(peso,nodo1,nodo2);
					}
				}

        void insertNode(N value, bool entrada, bool salida){
          for(int i=0; i < nodes.size(); i++){
              if(value == nodes[i]->getNdata()){
                  return;
              }
          }
          node* NewNodo = new node(value,entrada,salida);
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

				self Brzozowski(){
					self minimizedDFA;

					getTranspose();

				}

				self NFAtoDFA(){
					self convertedToDFA;
					set<string> setSalida;
					queue<N> myqueue;
					string newNode1="?";
					string newNode2="?";

					for(ni=nodes.begin(); ni!=nodes.end(); ni++){ //crea un set de los estados de salida
						if((*ni)->getSalida()){
							setSalida.insert((*ni)->getNdata());
						}
					}

					for(ni=nodes.begin(); ni!=nodes.end(); ni++){
						if((*ni)->getEntrada()){
							if((*ni)->getSalida()){ //Agrega al nuevo grafo el primer nodo(el de entrada del anterior)
								convertedToDFA.insertNode((*ni)->getNdata(), true, true);
							}
							else{
								convertedToDFA.insertNode((*ni)->getNdata(), true, false);
							}

							for(ei=(*ni)->edges.begin(); ei!=(*ni)->edges.end(); ei++){ //Suma los nombres de los estados
								if((*ei)->getEdata()==0){
									newNode1+=(*ei)->nodes[1]->getNdata();
								}
								else if((*ei)->getEdata()==1){
									newNode2+=(*ei)->nodes[1]->getNdata();
								}
							}

							if(newNode2.find("?") != string::npos && newNode2.size()>1){ //borra ?
								newNode2.erase(remove(newNode2.begin(), newNode2.end(), '?'), newNode2.end());
							}
							if(newNode1.find("?") != string::npos && newNode1.size()>1){
								newNode1.erase(remove(newNode1.begin(), newNode1.end(), '?'), newNode1.end());
							}

							bool isSalida1=false;
							for(auto& salidas: setSalida){
						    if(newNode1.find(salidas) != string::npos){ //si dentro de newNode1 hay un elemento de salida
						      convertedToDFA.insertNode(newNode1, false, true);
									isSalida1=true;
						      break;
						    }
						  }
							if(!isSalida1){ convertedToDFA.insertNode(newNode1, false, false);}

							bool isSalida2=false;
							for(auto& salidas: setSalida){
						    if(newNode2.find(salidas) != string::npos){ //si dentro de newNode1 hay un elemento de salida
						      convertedToDFA.insertNode(newNode2, false, true);
									isSalida2=true;
						      break;
						    }
						  }
							if(!isSalida2){ convertedToDFA.insertNode(newNode2, false, false);}

							convertedToDFA.insertEdge(0, (*ni)->getNdata(), newNode1);
							convertedToDFA.insertEdge(1, (*ni)->getNdata(), newNode2);

						}

					}
					myqueue.push(newNode1);
					newNode1.clear(); //maybe we dont need this
					myqueue.push(newNode2);
					newNode2.clear();

					while(!myqueue.empty()){
						newNode1="?";
						newNode2="?";
						if(myqueue.front()=="?" && !myqueue.empty()){
							convertedToDFA.insertEdge(0, "?", "?");
							convertedToDFA.insertEdge(1, "?", "?");
							myqueue.pop();
						}
						bool a=false;
						for(ni=nodes.begin();ni!=nodes.end();++ni){

							if((*ni)->getNdata()==myqueue.front() && !myqueue.empty()){
								a=true;
								for(ei=(*ni)->edges.begin(); ei!=(*ni)->edges.end(); ei++){
									if((*ei)->getEdata()==0){
										newNode1+=(*ei)->nodes[1]->getNdata();
									}
									if((*ei)->getEdata()==1){
										newNode2+=(*ei)->nodes[1]->getNdata();
									}
								}

								if(newNode2.find("?") != string::npos && newNode2.size()>1){
									newNode2.erase(remove(newNode2.begin(), newNode2.end(), '?'), newNode2.end());
								}
								if(newNode1.find("?") != string::npos && newNode1.size()>1){
									newNode1.erase(remove(newNode1.begin(), newNode1.end(), '?'), newNode1.end());
								}

								if(find(convertedToDFA.nodes.begin(), convertedToDFA.nodes.end(), convertedToDFA.findNode(newNode1)) == convertedToDFA.nodes.end()){ //if newNode1 is not in the nodes vector
									bool isSalida1=false;
									for(auto& salidas: setSalida){
								    if(newNode1.find(salidas) != string::npos){ //si dentro de newNode1 hay un elemento de salida
								      convertedToDFA.insertNode(newNode1, false, true);
											isSalida1=true;
								      break;
								    }
								  }
									if(!isSalida1){ convertedToDFA.insertNode(newNode1, false, false);}
									myqueue.push(newNode1);
								}

								if(find(convertedToDFA.nodes.begin(), convertedToDFA.nodes.end(), convertedToDFA.findNode(newNode2)) == convertedToDFA.nodes.end()){//if newNode2 is not in the nodes vector
									bool isSalida2=false;
									for(auto& salidas: setSalida){
								    if(newNode2.find(salidas) != string::npos){ //si dentro de newNode1 hay un elemento de salida
								      convertedToDFA.insertNode(newNode2, false, true);
											isSalida2=true;
								      break;
								    }
								  }
									if(!isSalida2){ convertedToDFA.insertNode(newNode2, false, false);}
									myqueue.push(newNode2);
								}

								convertedToDFA.insertEdge(0, myqueue.front(), newNode1);
								convertedToDFA.insertEdge(1, myqueue.front(), newNode2);
								cout << myqueue.front() << " ";
								myqueue.pop();
							}

						}
						if(a==false){cout << myqueue.front(); myqueue.pop();};
					}

					convertedToDFA.print();
					return convertedToDFA;
				}

				self getTranspose(){
				 	self transpose;

				 	for(ni=nodes.begin();ni!=nodes.end();++ni){
						if((*ni)->getEntrada()){ //si el nodo es entrada se vuelve salida
							transpose.insertNode((*ni)->getNdata(), false, true);
						}
						else if((*ni)->getSalida()){ //si el nodo es salida se vuelve entrada
							transpose.insertNode((*ni)->getNdata(), true, false);
						}
				 		else{
							transpose.insertNode((*ni)->getNdata(),false, false);
						}
				 	}

				 	for(ni=nodes.begin();ni!=nodes.end();++ni){
				 		for(ei=(*ni)->edges.begin(); ei!=(*ni)->edges.end(); ++ei){
				 			transpose.insertEdge((*ei)->getEdata(), (*ei)->nodes[1]->getNdata(), (*ei)->nodes[0]->getNdata());
				 		}
				 	}

					transpose.print();
				 	return transpose;
				}

        void print(){
    				if (!nodes.size()) {cout<<"No hay nodos\n"; return;}
            for(auto ni: nodes){
                if(ni->getEntrada()){cout << "->";}
                if(ni->getSalida()){cout << " *";}
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
