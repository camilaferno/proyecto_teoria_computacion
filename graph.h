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

#include "state.h"
#include "transition.h"

using namespace std;

class Traits {
public:
    typedef string T;
    typedef string S;
};

template <typename Tr>
struct Automata {
	typedef Automata<Tr> self;
  typedef State<self> state;
  typedef Transition<self> transition;
  typedef vector<state*> StateSeq;
  typedef vector<transition*> TransitionSeq;
  typedef typename StateSeq::iterator StateIte;
  typedef typename TransitionSeq::iterator TransitionIte;
  typedef typename Tr::S S;
  typedef typename Tr::T T;

	int numberOfStates, numberOfFinalStates;
	state* initState;
	StateSeq states, initStates, finalStates;

  StateIte si;
  TransitionIte ti;

	Automata() {}

	vector<string> split(string phrase, string delimiter){
    vector<string> list;
    string s = string(phrase);
    size_t pos = 0;
    string token;
    while ((pos = s.find(delimiter)) != string::npos)
    {
        token = s.substr(0, pos);
        list.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    return list;
	}

	Automata(string file){
    ifstream automataFile (file);
    string line;

    if (automataFile.is_open()){
	  	string firstLine;
	    getline(automataFile, firstLine);

	    vector<string> vectorFirstLine = split(firstLine + " ", " ");

	    numberOfStates = stoi(vectorFirstLine[0]);
	    S fileInitState = vectorFirstLine[1];
	    numberOfFinalStates = stoi(vectorFirstLine[2]);

	    initState = new state(fileInitState);
			initStates.push_back(initState);
			states.push_back(initState);

	    for(size_t i = 3; i < vectorFirstLine.size(); i++){
        state* newState = new state(vectorFirstLine[i]);
        states.push_back(newState);
        finalStates.push_back(newState);
	    }

	    string line;
	    while (getline(automataFile, line)){
      	vector<string> vectorLine = split(line + " ", " ");
    		T transitionChar = vectorLine[1];

				if(!isState(vectorLine[0])){
					addState(vectorLine[0]);
				}

				if(!isState(vectorLine[2])){
					addState(vectorLine[2]);
				}

				state* sb = findState(vectorLine[0]);
				state* se = findState(vectorLine[2]);

				sb->addTransition(transitionChar, sb, se);
	  }

	    automataFile.close();
    }
    else cout << "Error" << endl;
  };

	void addState(S data)
	{
			state* newState = new state(data);
			states.push_back(newState);
	}

	bool isState(S data){
		for(si = states.begin(); si!=states.end(); ++si){
			if((*si)->data == data){return true;}
		}
		return false;
	}

  bool isInitState(S data){
    for(si = initStates.begin(); si!=initStates.end(); ++si){
			if((*si)->data == data){return true;}
		}
		return false;
  }

	state* findState(S data)
	{
			bool found=false;
			if (states.size() == 0) return nullptr;
			for(si = states.begin(); si!=states.end(); ++si)
			{
					if((*si)->data == data){
						found=true;
						return (*si);
					}
			}
			if(!found){return nullptr;};
			return nullptr;
	}

	state* findState(state s)
	{
			if (states.size() == 0) return nullptr;
			bool found = false;
			for(si = states.begin(); si!=states.end(); ++si)
			{
					if((*si)->data == s->data)
					{
							found = true;
							break;
					}
			}

			if (!found) return nullptr;
			else return (*si);
	}

	state* findState(state* s, StateSeq where)
	{
			if (where.size() == 0) return nullptr;
			bool found = false;
			for(si = where.begin(); si!=where.end(); ++si)
			{
					if((*si)->data == s->data)
					{
							found = true;
							break;
					}
			}

			if (!found) return nullptr;
			else return (*si);
	}

	Automata* getTranspose(){
		Automata* transpose = new Automata();

    for(si=states.begin();si!=states.end();++si){
			transpose->numberOfStates+=1;
			transpose->addState((*si)->data);
		}

		for(auto it=states.begin();it!=states.end();++it){
			for(ti=(*it)->transitions.begin(); ti!=(*it)->transitions.end(); ++ti){
				transpose->findState((*ti)->stateEnd->data)->addTransition((*ti)->transitionChar, transpose->findState((*ti)->stateEnd->data), transpose->findState((*ti)->stateBegin->data));
			}
		}

		for(auto it=initStates.begin(); it!=initStates.end(); ++it){
      transpose->finalStates.push_back(transpose->findState((*it)->data));
		}
		for(auto it=finalStates.begin(); it!=finalStates.end(); ++it){
			transpose->initStates.push_back(transpose->findState((*it)->data));
		}
		transpose->numberOfFinalStates=finalStates.size();
		transpose->initState=finalStates.front();

		transpose->printNFA();
		cout << endl;
		return transpose;
	}

	// void unionOfStates(string &newNode1, string &newNode2, string current){
	//   for(int i=0; i<current.size(); i++){
	//     string character = current.substr(i, 1);
	//     for(auto stateit=states.begin(); stateit!=states.end(); ++stateit){
	//     	if(character==(*stateit)->getNdata()){
	//     		for(ti=(*stateit)->transitions.begin(); ti!=(*stateit)->transitions.end(); ti++){
	//     			if((*ei)->getEdata()==0){
	//     				newNode1+=(*ei)->nodes[1]->getNdata();
	//     			}
	//     			if((*ei)->getEdata()==1){
	//     				newNode2+=(*ei)->nodes[1]->getNdata();
	//     			}
	//     		}
	//     	}
	//     }
	//   }
	// }

	Automata* NFAtoDFA(){
		Automata* convertedToDFA = new Automata();
		queue<S> myqueue;
		string newNode1="?";
		string newNode2="?";

		//Agregar stados inciales
		for(auto it=initStates.begin(); it!=initStates.end(); ++it){
      convertedToDFA->addState((*it)->data);
			convertedToDFA->initStates.push_back(convertedToDFA->findState((*it)->data));
			convertedToDFA->numberOfStates+=1;
		}

		for(auto it=states.begin(); it!=states.end(); it++){
			if(isInitState((*it)->data)){
				for(ti=(*it)->transitions.begin(); ti!=(*it)->transitions.end(); ti++){ //Suma los nombres de los estados
					if((*ti)->transitionChar=="0"){
						newNode1+=(*ti)->stateEnd->data;
					}
					else if((*ti)->transitionChar=="1"){
						newNode2+=(*ti)->stateEnd->data;
					}
				}

				if(newNode2.find("?") != string::npos && newNode2.size()>1){ //borra "?"
					newNode2.erase(remove(newNode2.begin(), newNode2.end(), '?'), newNode2.end());
				}
				if(newNode1.find("?") != string::npos && newNode1.size()>1){
					newNode1.erase(remove(newNode1.begin(), newNode1.end(), '?'), newNode1.end());
				}

				sort(newNode1.begin(), newNode1.end());
				sort(newNode2.begin(), newNode2.end());

				bool isSalida1=false;
				for(auto& salida: finalStates){
			    if(newNode1.find((salida)->data) != string::npos){ //si dentro de newNode1 hay un elemento de salida
			      convertedToDFA->addState(newNode1);
						convertedToDFA->finalStates.push_back(convertedToDFA->findState(newNode1));
						convertedToDFA->numberOfFinalStates+=1;
						convertedToDFA->numberOfStates+=1;
						isSalida1=true;
			      break;
			    }
			  }
				if(!isSalida1){convertedToDFA->numberOfStates+=1; convertedToDFA->addState(newNode1);}

				bool isSalida2=false;
				for(auto& salida: finalStates){
			    if(newNode2.find((salida)->data) != string::npos){ //si dentro de newNode1 hay un elemento de salida
						convertedToDFA->addState(newNode2);
						convertedToDFA->finalStates.push_back(convertedToDFA->findState(newNode2));
						convertedToDFA->numberOfFinalStates+=1;
						convertedToDFA->numberOfStates+=1;
						isSalida1=true;
			      break;
			    }
			  }
				if(!isSalida2){convertedToDFA->numberOfStates+=1; convertedToDFA->addState(newNode2);}

				(convertedToDFA->findState((*it)->data))->addTransition("0", convertedToDFA->findState((*it)->data), convertedToDFA->findState(newNode1));
				(convertedToDFA->findState((*it)->data))->addTransition("1", convertedToDFA->findState((*it)->data), convertedToDFA->findState(newNode2));

			}

		}

		// myqueue.push(newNode1);
		// newNode1.clear(); //maybe we dont need this
		// myqueue.push(newNode2);
		// newNode2.clear();

		// while(!myqueue.empty()){
		// 	newNode1="?";
		// 	newNode2="?";
		// 	if(myqueue.front()=="?" && !myqueue.empty()){
		// 		convertedToDFA.insertEdge(0, "?", "?");
		// 		convertedToDFA.insertEdge(1, "?", "?");
		// 		myqueue.pop();
		// 	}
		// 	bool a=false;
		// 	for(ni=nodes.begin();ni!=nodes.end();++ni){
		// 		string current;
		// 		if(!myqueue.empty()){current=myqueue.front();}
		// 		string firstNode=(*ni)->getNdata();
		// 		if(((*ni)->getNdata()==myqueue.front() && !myqueue.empty()) || (current.find(firstNode) != string::npos && !myqueue.empty())){
		// 			a=true;
		//
		// 			if(current.size()>1){ //Si se necesita union
		// 				unionOfStates(newNode1, newNode2, current);
		// 			}
		// 			else{
		// 				for(ei=(*ni)->edges.begin(); ei!=(*ni)->edges.end(); ei++){
		// 					if((*ei)->getEdata()==0){
		// 						newNode1+=(*ei)->nodes[1]->getNdata();
		// 					}
		// 					if((*ei)->getEdata()==1){
		// 						newNode2+=(*ei)->nodes[1]->getNdata();
		// 					}
		// 				}
		// 			}
		//
		// 			if(newNode2.find("?") != string::npos && newNode2.size()>1){
		// 				newNode2.erase(remove(newNode2.begin(), newNode2.end(), '?'), newNode2.end());
		// 			}
		// 			if(newNode1.find("?") != string::npos && newNode1.size()>1){
		// 				newNode1.erase(remove(newNode1.begin(), newNode1.end(), '?'), newNode1.end());
		// 			}
		//
		// 			sort(newNode1.begin(), newNode1.end());
		// 			sort(newNode2.begin(), newNode2.end());
		//
		// 			if(find(convertedToDFA.nodes.begin(), convertedToDFA.nodes.end(), convertedToDFA.findNode(newNode1)) == convertedToDFA.nodes.end()){ //if newNode1 is not in the nodes vector
		// 				bool isSalida1=false;
		// 				for(auto& salidas: setSalida){
		// 			    if(newNode1.find(salidas) != string::npos){ //si dentro de newNode1 hay un elemento de salida
		// 			      convertedToDFA.insertNode(newNode1, false, true);
		// 						isSalida1=true;
		// 			      break;
		// 			    }
		// 			  }
		// 				if(!isSalida1){ convertedToDFA.insertNode(newNode1, false, false);}
		// 				myqueue.push(newNode1);
		// 			}
		//
		// 			if(find(convertedToDFA.nodes.begin(), convertedToDFA.nodes.end(), convertedToDFA.findNode(newNode2)) == convertedToDFA.nodes.end()){//if newNode2 is not in the nodes vector
		// 				bool isSalida2=false;
		// 				for(auto& salidas: setSalida){
		// 			    if(newNode2.find(salidas) != string::npos){ //si dentro de newNode1 hay un elemento de salida
		// 			      convertedToDFA.insertNode(newNode2, false, true);
		// 						isSalida2=true;
		// 			      break;
		// 			    }
		// 			  }
		// 				if(!isSalida2){ convertedToDFA.insertNode(newNode2, false, false);}
		// 				myqueue.push(newNode2);
		// 			}
		//
		// 			convertedToDFA.insertEdge(0, myqueue.front(), newNode1);
		// 			convertedToDFA.insertEdge(1, myqueue.front(), newNode2);
		// 			newNode1="?";
		// 			newNode2="?";
		// 			myqueue.pop();
		// 		}
		//
		// 	}
		// }

		convertedToDFA->printNFA();
		return convertedToDFA;
	}

	void Brzozowski(){
		//Automata* brzozowski = new Automata();

    getTranspose()->NFAtoDFA();

		//return brzozowski;
	}

	void printDFA()
  {
      cout << numberOfStates << " " << initState->data << " " << numberOfFinalStates << " ";

      for (si = finalStates.begin(); si != finalStates.end(); ++si)
      {
          cout << (*si)->data;
          if (si != finalStates.end()) cout << " ";
          cout << endl;
      }

      for(size_t i = 0; i < states.size(); i++)
      {
          for(size_t j = 0; j < states[i]->transitions.size(); j++)
          {
              cout << states[i]->transitions[j]->stateBegin->data << " ";
              cout << states[i]->transitions[j]->transitionChar << " ";
              cout << states[i]->transitions[j]->stateEnd->data << " ";
              cout << endl;
          }
      }

  }

	void printNFA()
	{
			cout << numberOfStates << " ";

			for(si=initStates.begin(); si!=initStates.end(); ++si){
				cout << (*si)->data << " ";
			}

			cout << numberOfFinalStates << " ";

			for(si=finalStates.begin(); si!=finalStates.end(); ++si){
				cout << (*si)->data << " ";
			}

			cout << endl;

      for(si=states.begin(); si!=states.end(); si++){
        for(ti=(*si)->transitions.begin(); ti!=(*si)->transitions.end(); ti++){
          cout << (*ti)->stateBegin->data << " ";
          cout << (*ti)->transitionChar << " ";
          cout << (*ti)->stateEnd->data << " ";
          cout << endl;
        }
      }


	}

				//
				// self getTranspose(){
				//  	self transpose;
				//
				//  	for(ni=nodes.begin();ni!=nodes.end();++ni){
				// 		if((*ni)->getEntrada()){ //si el nodo es entrada se vuelve salida
				// 			transpose.insertNode((*ni)->getNdata(), false, true);
				// 		}
				// 		else if((*ni)->getSalida()){ //si el nodo es salida se vuelve entrada
				// 			transpose.insertNode((*ni)->getNdata(), true, false);
				// 		}
				//  		else{
				// 			transpose.insertNode((*ni)->getNdata(),false, false);
				// 		}
				//  	}
				//
				//  	for(ni=nodes.begin();ni!=nodes.end();++ni){
				//  		for(ei=(*ni)->edges.begin(); ei!=(*ni)->edges.end(); ++ei){
				//  			transpose.insertEdge((*ei)->getEdata(), (*ei)->nodes[1]->getNdata(), (*ei)->nodes[0]->getNdata());
				//  		}
				//  	}
				//
				// 	transpose.print();
				//  	return transpose;
				// }
				//
				// void unionOfStates(string &newNode1, string &newNode2, string current){
				//   for(int i=0; i<current.size(); i++){
				//     string character = current.substr(i, 1);
				//     for(auto nodeit=nodes.begin();nodeit!=nodes.end();++nodeit){
				//     	if(character==(*nodeit)->getNdata()){
				//     		for(ei=(*nodeit)->edges.begin(); ei!=(*nodeit)->edges.end(); ei++){
				//     			if((*ei)->getEdata()==0){
				//     				newNode1+=(*ei)->nodes[1]->getNdata();
				//     			}
				//     			if((*ei)->getEdata()==1){
				//     				newNode2+=(*ei)->nodes[1]->getNdata();
				//     			}
				//     		}
				//     	}
				//     }
				//   }
				// }
				//
				// void mockfunct(){
				// 	string newNode1="?";
				// 	string newNode2="?";
				// 	string current="ABCDE";
				// 	unionOfStates(newNode1, newNode2, current);
				// 	cout << endl << "newnode1: " << newNode1 << "newnode2: " << newNode2;
				// }


};

typedef Automata<Traits> automata;

#endif
