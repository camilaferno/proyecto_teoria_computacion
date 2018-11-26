#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <algorithm>
#include <list>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <limits.h>
#include <set>
#include <queue>
#include <map>
#include <algorithm>

#include "state.h"
#include "transition.h"

using namespace std;

class Traits {
public:
    typedef string T;
    typedef string S;
};

template <typename Tr>
struct Automata
{
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
    map<string, int> renamedTo;

    StateIte si;
    TransitionIte ti;

    Automata() {}

    vector<string> split(string phrase, string delimiter)
    {
        vector<string> list;
        string s = string(phrase);
        int pos = 0;
        string token;
        while ((pos = s.find(delimiter)) != string::npos)
        {
            token = s.substr(0, pos);
            list.push_back(token);
            s.erase(0, pos + delimiter.length());
        }
        return list;
    }

    Automata(string file)
    {
        ifstream automataFile (file);
        string line;

        if (automataFile.is_open())
        {
            string firstLine;
            getline(automataFile, firstLine);

            vector<string> vectorFirstLine = split(firstLine + " ", " ");

            numberOfStates = stoi(vectorFirstLine[0]);
            S fileInitState = vectorFirstLine[1];
            numberOfFinalStates = stoi(vectorFirstLine[2]);

            initState = new state(fileInitState);
            initStates.push_back(initState);

            for(int i = 3; i < vectorFirstLine.size(); i++)
            {
                state* newState = new state(vectorFirstLine[i]);
                states.push_back(newState);
                finalStates.push_back(newState);
            }

            string line;

            while (getline(automataFile, line))
            {
                vector<string> vectorLine = split(line + " ", " ");
                T transitionChar = vectorLine[1];

                if(!isState(vectorLine[0]))
                {
                    addState(vectorLine[0]);
                }
                if(!isState(vectorLine[2]))
                {
                    addState(vectorLine[2]);
                }

                state* sb = findState(vectorLine[0]);
                state* se = findState(vectorLine[2]);

                sb->addTransition(transitionChar, sb, se);
            }

            automataFile.close();
            sortAllStates();
        }
        else cout << "Error" << endl;
    };

    void addState(S data)
    {
        state* newState = new state(data);
        states.push_back(newState);
    }

    // Comparador de estados para hacer sort
    // La función sort es la que viene con la STL y se ejecuta con:
    // states.sort(stateCmp());
    struct StateCmp
    {
        bool operator()(const state* a, const state* b)
        {
            try
            {
                return (stoi(a->data) < stoi(b->data));
            }
            catch(exception& e)
            {
                return (a->data < b->data);
            }
        }
    };

    void sortAllStates()
    {
        sort(states.begin(), states.end(), StateCmp());
        sort(finalStates.begin(), finalStates.end(), StateCmp());
    }

    bool isState(S data)
    {
        for (si = states.begin(); si != states.end(); ++si)
            if((*si)->data == data) return true;
        return false;
    }

    bool isInitState(S data)
    {
        for (si = initStates.begin(); si != initStates.end(); ++si)
            if((*si)->data == data) return true;
        return false;
    }

    bool isFinalState(S data)
    {
        for(si = finalStates.begin(); si != finalStates.end(); ++si)
            if((*si)->data == data) return true;
        return false;
    }

    state* findState(S data)
    {
        bool found = false;
        if (states.size() == 0) return nullptr;
        for(si = states.begin(); si != states.end(); ++si)
        {
            if((*si)->data == data)
            {
                found = true;
                return (*si);
            }
        }
        if(!found) return nullptr;
        return nullptr;
    }

    state* findState(state s)
    {
        if (states.size() == 0) return nullptr;
        bool found = false;
        for(si = states.begin(); si != states.end(); ++si)
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
        for(si = where.begin(); si != where.end(); ++si)
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

    state* findInitState(S data)
    {
      for (si = initStates.begin(); si != initStates.end(); ++si)
          if((*si)->data == data) return (*si);
    }

    state* findFinalState(S data)
    {
      for (si = finalStates.begin(); si != finalStates.end(); ++si)
          if((*si)->data == data) return (*si);
    }

    void sort_string(string &newNode)
    {
        vector<int> myvector;
        string tmp;
        stringstream stream(newNode);
        while(getline(stream, tmp, ','))
        {
            myvector.push_back(stoi(tmp));
        }

        sort(myvector.begin(), myvector.end());

        newNode.clear();
        newNode = to_string(*myvector.begin());

        for (std::vector<int>::iterator it=myvector.begin()+1; it!=myvector.end(); ++it)
        {
            newNode += ',' + to_string(*it);
        }
    }

    Automata* getTranspose()
    {
        Automata* transpose = new Automata();

        for(si = states.begin(); si != states.end(); ++si)
        {
             
            transpose->numberOfStates += 1;
            transpose->addState((*si)->data);
        }

        for(auto it = states.begin(); it != states.end(); ++it)
        {
            for(ti = (*it)->transitions.begin(); ti != (*it)->transitions.end(); ++ti)
            {
                transpose->findState((*ti)->stateEnd->data)->addTransition  ((*ti)->transitionChar,
                                                                            transpose->findState((*ti)->stateEnd->data),
                                                                            transpose->findState((*ti)->stateBegin->data));
            }
        }

        for(auto it = initStates.begin(); it != initStates.end(); ++it)
            transpose->finalStates.push_back(transpose->findState((*it)->data));

        for(auto it = finalStates.begin(); it != finalStates.end(); ++it)
            transpose->initStates.push_back(transpose->findState((*it)->data));

        transpose->numberOfFinalStates = transpose->finalStates.size();
        transpose->initState = finalStates.front();
         
        return transpose;
    }

    void unionOfStates(string &newNode1, string &newNode2, string current)
    {
        vector<string> states_in_current;
        string tmp;
        stringstream stream(current);
        while(getline(stream, tmp, ','))
        {
            states_in_current.push_back(tmp);
        }

        for(auto currentit = states_in_current.begin(); currentit != states_in_current.end(); ++currentit)
        {
            for(auto stateit = states.begin(); stateit != states.end(); ++stateit)
            {
                if(*currentit == (*stateit)->data)
                {
                    for(ti = (*stateit)->transitions.begin(); ti != (*stateit)->transitions.end(); ti++)
                    {
                        if((*ti)->transitionChar == "0")
                            newNode1 += ',' + (*ti)->stateEnd->data;
                        if((*ti)->transitionChar == "1")
                            newNode2 += ',' + (*ti)->stateEnd->data;
                    }
                }
            }
        }
    }

    vector<vector<bool>> Equivalencia()
    {
        vector<vector<bool>> matrix(numberOfStates, vector<bool>(numberOfStates, 1));

        for (int i = 0; i < numberOfStates; i++)
        {
            if (find(finalStates.begin(), finalStates.end(), states[i]) != finalStates.end())
            {
                for (int j = 0; j < numberOfStates; j++)
                {
                    if (find(finalStates.begin(), finalStates.end(), states[j]) == finalStates.end())
                        matrix[i][j] = matrix[j][i] = 0;
                }
            }
        }

        bool change = 1;
        while(change)
        {
            change = 0;
            for (int i = 0; i < numberOfStates; i++)
            {
                for (int j = 0; j < numberOfStates; j++)
                {
                    if (matrix[i][j] == 1)
                    {
                        int in, jn;
                        for (in = 0; in < numberOfStates; in++)
                        {
                            if (states[i] -> transitions[0] -> stateEnd == states[in])
                                break;
                        }
                        for (jn = 0; jn < numberOfStates; jn++)
                        {
                            if (states[j] -> transitions[0] -> stateEnd == states[jn])
                                break;
                        }
                        if (matrix[in][jn] == 0)
                        {
                            matrix[i][j] = 0;
                            change = 1;
                        }
                        else
                        {
                            for (in = 0; in < numberOfStates; in++)
                            {
                                if (states[i] -> transitions[1] -> stateEnd == states[in])
                                    break;
                            }

                            for (jn = 0; jn < numberOfStates; jn++)
                            {
                                if (states[j] -> transitions[1] -> stateEnd == states[jn])
                                    break;
                            }

                            if (matrix[in][jn] == 0)
                            {
                                matrix[i][j] = 0;
                                change = 1;
                            }
                        }
                    }
                }
            }
        }

        return matrix;
    };
    
    self* Hopcroft()
    {
        set<set<state*>> P, W;
        set<state*> F (finalStates.begin(),finalStates.end()), noF (states.begin(),states.end());
        for (si = finalStates.begin(); si != finalStates.end(); ++si)
            noF.erase(*si);
        P.insert(F);
        P.insert(noF);
        W.insert(F);
        while (!W.empty()){
            set<state*> A (*(W.begin()));
            W.erase(A);                     //choose and remove a set A from W
            for (int L = 0; L < 2; L++){     //lenguaje es {0,1}
                set<state*> X;
                for (si = states.begin(); si != states.end(); ++si){
                    if (A.count((*si)->transitions[L]->stateEnd)!=0)    //let X be the set of states for which a transition on c leads to a state in A
                        X.insert(*si);
                }
                for (typename set<set<state*>>::iterator it = P.begin(); it != P.end(); ++it){
                    set<state*> Y = *it, intersect, minus;
                    set_intersection(X.begin(),X.end(),Y.begin(),Y.end(),inserter(intersect,intersect.begin()));
                    set_difference(Y.begin(),Y.end(),X.begin(),X.end(),inserter(minus,minus.begin()));
                    if (!intersect.empty() && !minus.empty()){
                        P.erase(Y);
                        P.insert(intersect);
                        P.insert(minus);
                        if (W.find(Y) != W.end()){
                            W.erase(Y);
                            W.insert(intersect);
                            W.insert(minus);
                        }
                        else if (intersect.size() <= minus.size())
                            W.insert(intersect);
                        else
                            W.insert(minus);
                    }
                }
            }
        }
        //OUTPUT

        self* reduced_automata = new self();
        reduced_automata->numberOfStates = P.size();

        for (typename set<set<state*>>::iterator it = P.begin(); it != P.end(); ++it)
        {
            state* newState = new state();
            string newData = "";
            int i = 0;
            for (typename set<state*>::iterator it2 = (*it).begin(); it2 != (*it).end(); ++it2)
            {
                newData += (*it2)->data;
                if (i != (*it).size() - 1)
                    newData += ",";
                i++;
            }

            newState->data = "[" + newData + "]";
            reduced_automata->states.push_back(newState);
        }
        int i = 0;
        for (typename set<set<state*>>::iterator it = P.begin(); it != P.end(); ++it)
        {
            state* stateA = *((*it).begin());
            state* &stateB_0 = stateA->transitions[0]->stateEnd;

            int stateB_0_Index = -1;

            bool found = false;
            int j = 0;
            for (typename set<set<state*>>::iterator it2 = P.begin(); it2 != P.end() && !found; ++it2)
            {
                if ((*it2).find(stateB_0) != (*it2).end())
                {
                    found = true;
                    stateB_0_Index = j;
                }
                j++;
            }

            state* &stateB_1 = stateA->transitions[1]->stateEnd;

            int stateB_1_Index = -1;

            found = false;
            j = 0;
            for (typename set<set<state*>>::iterator it2 = P.begin(); it2 != P.end() && !found; ++it2)
            {
                if ((*it2).find(stateB_1) != (*it2).end())
                {
                    found = true;
                    stateB_1_Index = j;
                }
                j++;
            }

            reduced_automata->states[i]->addTransition("0", reduced_automata->states[i], reduced_automata->states[stateB_0_Index]);
            reduced_automata->states[i]->addTransition("1", reduced_automata->states[i], reduced_automata->states[stateB_1_Index]);
            i++;
        }

        bool found = false;
        int indexOfInitState = -1;
        i = 0;
        for (typename set<set<state*>>::iterator it = P.begin(); it != P.end()  && !found; ++it)
        {
            for (typename set<state*>::iterator it2 = (*it).begin(); it2 != (*it).end() && !found; ++it2)
            {
                if ((*it2)->data == initState->data)
                {
                    found = true;
                    indexOfInitState = i;
                }
                
            }
            i++;
        }
        cout << endl;

        reduced_automata->initState = reduced_automata->states[indexOfInitState];

        for (int i = 0; i < finalStates.size(); i++)
        {
            bool found = false;
            int indexOfFinalState = -1, j = 0;
            for (typename set<set<state*>>::iterator it = P.begin(); it != P.end() && !found; ++it)
            {
                if ((*it).find(finalStates[i]) != (*it).end())
                {
                    found = true;
                    indexOfFinalState = j;
                }
                j++;
            }

            state* &newFinalState = reduced_automata->states[indexOfFinalState];

            if(findState(newFinalState, reduced_automata->finalStates) == nullptr)
                reduced_automata->finalStates.push_back(newFinalState);
        }

        reduced_automata->numberOfFinalStates = reduced_automata->finalStates.size();


        return reduced_automata;
    };
    
    Automata* NFAtoDFA()
    {
        Automata* convertedToDFA = new Automata();
        queue<S> myqueue;

        string init;

        string newNode1 = "-";
        string newNode2 = "-";

        for(auto it = initStates.begin(); it != initStates.end(); ++it)
        {
            init += ',' + (*it)->data;
        }

        // Borra ","
        if(init.find(",") != string::npos && init.size()>1)
        {
            init.erase(0,1);
        }

        convertedToDFA->addState(init);
        convertedToDFA->initStates.push_back(convertedToDFA->findState(init));
        if(init.find(",") !=  string::npos)
        {
            string tmpInit;
            stringstream stream(init);
            while(getline(stream, tmpInit, ','))
            {
                if(isFinalState(tmpInit))
                {
                    convertedToDFA->finalStates.push_back(convertedToDFA->findState(init));
                    convertedToDFA->numberOfFinalStates += 1;
                    break;
                }
            }
        }
        else if(isFinalState(init))
        {
            convertedToDFA->finalStates.push_back(convertedToDFA->findState(init));
            convertedToDFA->numberOfFinalStates += 1;
        }

        convertedToDFA->numberOfStates += 1;

        if(init.find(",") !=  string::npos)
            unionOfStates(newNode1, newNode2, init);
        else
        {
            for(auto it = initStates.begin(); it != initStates.end(); ++it)
            {
                if((*it)->data == init)
                {
                    for(ti = (*it)->transitions.begin(); ti != (*it)->transitions.end(); ti++)
                    {
                        if((*ti)->transitionChar == "0")
                            newNode1 += ',' + (*ti)->stateEnd->data;

                        if((*ti)->transitionChar == "1")
                            newNode2 += ',' + (*ti)->stateEnd->data;
                    }
                    break;
                }
            }
        }

        if(newNode2.find("-") != string::npos && newNode2.size()>1)
        {
          newNode2.erase(0,2); //there where 2 identical erase here idk why
        }
        if(newNode1.find("-") != string::npos && newNode1.size()>1)
        {
          newNode1.erase(0,2);
        }

        if(newNode1 != "-")
        {
            sort_string(newNode1);
        }
        if(newNode2 != "-")
        {
            sort_string(newNode2);
        }

        if(!convertedToDFA->isState(newNode1))
        {
            bool isSalida1 = false;

            // Si dentro de newNode1 hay un elemento de salida
            string tmpNewNode1;
            stringstream stream(newNode1);
            while(getline(stream, tmpNewNode1, ','))
            {
              if(isFinalState(tmpNewNode1))
              {
                convertedToDFA->addState(newNode1);
                convertedToDFA->finalStates.push_back(convertedToDFA->findState(newNode1));
                convertedToDFA->numberOfFinalStates += 1;
                convertedToDFA->numberOfStates += 1;
                isSalida1 = true;
                break;
              }
          }

            if(!isSalida1)
            {
                convertedToDFA->numberOfStates += 1;
                convertedToDFA->addState(newNode1);
            }
        }

        if(!convertedToDFA->isState(newNode2))
        {
            bool isSalida2 = false;

            // Si dentro de newNode2 hay un elemento de salida
            string tmpNewNode2;
            stringstream stream(newNode2);
            while(getline(stream, tmpNewNode2, ','))
            {
              if(isFinalState(tmpNewNode2))
              {
                convertedToDFA->addState(newNode2);
                convertedToDFA->finalStates.push_back(convertedToDFA->findState(newNode2));
                convertedToDFA->numberOfFinalStates += 1;
                convertedToDFA->numberOfStates += 1;
                isSalida2 = true;
                break;
              }
            }

            if(!isSalida2)
            {
                convertedToDFA->numberOfStates += 1;
                convertedToDFA->addState(newNode2);
            }
        }

        (convertedToDFA->findState(init))->addTransition ("0",
                                                                convertedToDFA->findState(init),
                                                                convertedToDFA->findState(newNode1));

        (convertedToDFA->findState(init))->addTransition ("1",
                                                                convertedToDFA->findState(init),
                                                                convertedToDFA->findState(newNode2));


        if (!isInitState(newNode1))
        {
            myqueue.push(newNode1);
        }

        if (!isInitState(newNode2) && newNode1!=newNode2)
        {
            myqueue.push(newNode2);
        }

        while (!myqueue.empty())
        {
            newNode1 = "-";
            newNode2 = "-";

            if (myqueue.front() == "-" && !myqueue.empty())
            {
                state* vacio = convertedToDFA->findState("-");
                vacio->addTransition("0", vacio, vacio);
                vacio->addTransition("1", vacio, vacio);
                myqueue.pop();
            }

            for(auto it = states.begin(); it!= states.end(); it++)
            {
                string current;

                if (!myqueue.empty())
                    current = myqueue.front();

                string firstState = (*it)->data;

                if
                (
                    ((*it)->data == myqueue.front() && !myqueue.empty())
                    ||
                    (current.find(firstState) != string::npos && !myqueue.empty())
                )
                {
                    // Si se necesita union
                    if(current.find(",") !=  string::npos)
                        unionOfStates(newNode1, newNode2, current);
                    else
                    {
                        for(ti = (*it)->transitions.begin(); ti != (*it)->transitions.end(); ti++)
                        {
                            if((*ti)->transitionChar == "0")
                                newNode1 += ',' + (*ti)->stateEnd->data;

                            if((*ti)->transitionChar == "1")
                                newNode2 += ',' + (*ti)->stateEnd->data;
                        }
                    }

                    if(newNode2.find("-") != string::npos && newNode2.size()>1)
                    {
                      newNode2.erase(0,2); //there where 2 identical erase here idk why
                    }
                    if(newNode1.find("-") != string::npos && newNode1.size()>1)
                    {
                      newNode1.erase(0,2);
                    }

                    if(newNode1 != "-")
                    {
                        sort_string(newNode1);
                    }
                    if(newNode2 != "-")
                    {
                        sort_string(newNode2);
                    }

                    if(!convertedToDFA->isState(newNode1))
                    {
                        bool isSalida1 = false;

                        // Si dentro de newNode1 hay un elemento de salida
                        string tmpNewNode1;
                        stringstream stream(newNode1);
                        while(getline(stream, tmpNewNode1, ','))
                        {
                            if(isFinalState(tmpNewNode1))
                            {
                                convertedToDFA->addState(newNode1);
                                convertedToDFA->finalStates.push_back(convertedToDFA->findState(newNode1));
                                convertedToDFA->numberOfFinalStates += 1;
                                convertedToDFA->numberOfStates += 1;
                                isSalida1 = true;
                                break;
                            }
                        }

                        if(!isSalida1)
                        {
                            convertedToDFA->numberOfStates += 1;
                            convertedToDFA->addState(newNode1);
                        }
                        myqueue.push(newNode1);
                    }

                    if(!convertedToDFA->isState(newNode2))
                    {
                        bool isSalida2 = false;

                        // Si dentro de newNode2 hay un elemento de salida
                        string tmpNewNode2;
                        stringstream stream(newNode2);
                        while(getline(stream, tmpNewNode2, ','))
                        {
                          if(isFinalState(tmpNewNode2))
                          {
                            convertedToDFA->addState(newNode2);
                            convertedToDFA->finalStates.push_back(convertedToDFA->findState(newNode2));
                            convertedToDFA->numberOfFinalStates += 1;
                            convertedToDFA->numberOfStates += 1;
                            isSalida2 = true;
                            break;
                          }
                        }

                        if(!isSalida2)
                        {
                            convertedToDFA->numberOfStates += 1;
                            convertedToDFA->addState(newNode2);
                        }
                        if(newNode1 != newNode2)
                            myqueue.push(newNode2);
                    }

                    (convertedToDFA->findState(myqueue.front()))->addTransition ("0",
                                                                                convertedToDFA->findState(myqueue.front()),
                                                                                convertedToDFA->findState(newNode1));

                    (convertedToDFA->findState(myqueue.front()))->addTransition ("1",
                                                                                convertedToDFA->findState(myqueue.front()),
                                                                                convertedToDFA->findState(newNode2));

                    newNode1 = "-";
                    newNode2 = "-";
                    myqueue.pop();
                }

            }
        }
        
        return convertedToDFA;
    }

    void renombramiento(Automata* &rename)
    {
        int count=0;

        for(auto it = rename->states.begin(); it != rename->states.end(); ++it)
        {
          renamedTo.insert(pair<string, int>((*it)->data, count));
          count +=1;
        }

        ostringstream int_to_string;

        for(auto it = rename->states.begin(); it != rename->states.end(); ++it)
        {
          ostringstream int_to_string;
          int_to_string << renamedTo.find((*it)->data)->second;
          (*it)->data = int_to_string.str();

          if(isInitState((*it)->data))
          {
            findInitState((*it)->data)->data = int_to_string.str();
          }

          if(isFinalState((*it)->data))
          {
            findFinalState((*it)->data)->data = int_to_string.str();
          }

        }
    }

    Automata* Brzozowski()
    {
        Automata* renamed = new Automata;
        Automata* lastRenamed = new Automata;
        renamed = (getTranspose()->NFAtoDFA());
        renombramiento(renamed);

        lastRenamed = (renamed->getTranspose())->NFAtoDFA();
        renombramiento(lastRenamed);

        return lastRenamed;
    }

    struct PairOfStates
    {
        state* first;
        state* second;
        vector<PairOfStates*> dependencies;
        string data;

        PairOfStates(state* newFirst, state* newSecond): first(newFirst), second(newSecond)
        {
            data = newFirst->data + newSecond->data;
        };

        void addDependency(state* first, state* second)
        {
            PairOfStates* newPair = new PairOfStates(first, second);
            dependencies.push_back(newPair);
        }
    };

    PairOfStates* findPairOfStates(state* first, state* second, vector<PairOfStates*> &where)
    {
        for(int i = 0; i < where.size(); i++)
            if (where[i]->first == first && where[i]->second == second) return where[i];

        return nullptr;
    }

    vector<vector<bool>> equivalence_improved()
    {
        const bool dist = 0;
        const bool no_dist = 1;

        vector<vector<bool>> distinctMatrix(numberOfStates, vector<bool>(numberOfStates, no_dist));
        vector<PairOfStates*> pairs;
        queue<PairOfStates*> pairQueue;
        map<state*, int> stateLocation;

        // Producto cartesiano entre el vector de estados para crear cada par posible
        for(int i = 0; i < numberOfStates; i++)
        {
            stateLocation[states[i]] = i;
            for(int j = 0; j < numberOfStates; j++)
            {
                PairOfStates* newPair = new PairOfStates(states[i], states[j]);
                pairs.push_back(newPair);
            }
        }

        // Asignación de dependencias para cada par
        for(int i = 0; i < pairs.size(); i++)
        {
            PairOfStates* &pair = pairs[i];
            PairOfStates* transitionPair0 = findPairOfStates(pairs[i]->first->transitions[0]->stateEnd,
                                                        pairs[i]->second->transitions[0]->stateEnd,
                                                        pairs);

            PairOfStates* transitionPair1 = findPairOfStates(pairs[i]->first->transitions[1]->stateEnd,
                                                        pairs[i]->second->transitions[1]->stateEnd,
                                                        pairs);

            transitionPair0->dependencies.push_back(pair);
            transitionPair1->dependencies.push_back(pair);
        }

        // Inicializa como distinguibles los pares (p,q) donde p es estado final y q no
        for(int i = 0; i < numberOfStates; i++)
        {
            if (findState(states[i], finalStates))
            {
                for(int j = 0; j < numberOfStates; j++)
                {
                    if (states[j] != states[i] && findState(states[j], finalStates) == nullptr)
                    {
                        distinctMatrix[i][j] = dist;
                        distinctMatrix[j][i] = dist;
                        pairQueue.push(findPairOfStates(states[i], states[j], pairs)); 
                    }
                }
            }
        }

        while (!pairQueue.empty())
        {
            PairOfStates* pair = pairQueue.front();

            int pairFirstLocation = stateLocation[pair->first];
            int pairSecondLocation = stateLocation[pair->second];

            if (distinctMatrix[pairFirstLocation][pairSecondLocation] == dist)
            {
                vector<PairOfStates*> &listOfDependencies = pair->dependencies;
                for (int j = 0; j < listOfDependencies.size(); j++)
                {
                    PairOfStates* &currentDependency = listOfDependencies[j];
                    int depFirstLocation = stateLocation[currentDependency->first];
                    int depSecondLocation = stateLocation[currentDependency->second];

                    if (distinctMatrix[depFirstLocation][depSecondLocation] != dist && depFirstLocation != depSecondLocation)
                    {
                        distinctMatrix[depFirstLocation][depSecondLocation] = distinctMatrix[depSecondLocation][depFirstLocation] = dist;
                        pairQueue.push(currentDependency);
                    }
                }
            }
            pairQueue.pop();
        }

        return distinctMatrix;
    }

    self* moore()
    {
        vector<vector<bool>> distinctMatrix = equivalence_improved();
        const bool dist = 0;
        const bool no_dist = 1;
        
        vector<vector<state*>> sets;

        // Armando sets
        for (int i = 0; i < states.size(); i++)
        {
            for (int j = 0; j < i; j++)
            {
                if (distinctMatrix[i][j] == no_dist)
                {   
                    if (sets.size() == 0)
                    {
                        vector<state*> newSet;
                        newSet.push_back(states[j]);
                        newSet.push_back(states[i]);
                        sets.push_back(newSet);
                    }
                    else
                    {
                        bool found = false;
                        for (int k = 0; k < sets.size() && !found; k++)
                        {
                            state* isStateOneInSet = findState(states[i], sets[k]);
                            state* isStateTwoInSet = findState(states[j], sets[k]);
                            if (isStateOneInSet != nullptr || isStateTwoInSet != nullptr)
                            {
                                if (isStateOneInSet == nullptr)
                                    sets[k].push_back(states[i]);
                                if (isStateTwoInSet == nullptr)
                                    sets[k].push_back(states[j]);
                                found = true;
                            }
                        }
                        if (!found)
                        {
                            vector<state*> newSet;
                            newSet.push_back(states[j]);
                            newSet.push_back(states[i]);
                            sets.push_back(newSet);
                        }
                    }
                }
            }
        }

        for (int i = 0; i < states.size(); i++)
        {
            bool found = false;
            for (int j = 0; j < sets.size() && !found; j++)
            {
                if (findState(states[i], sets[j]) != nullptr)
                    found = true;
            }
            if (!found)
            {
                vector<state*> newSet;
                newSet.push_back(states[i]);
                sets.push_back(newSet);
            }
        }

        self* reduced_automata = new self();
        reduced_automata->numberOfStates = sets.size();
        
        // Añadiendo estados
        for (int i = 0; i < sets.size(); i++)
        {
            state* newState = new state();
            string newData = "";

            for (int j = 0; j < sets[i].size(); j++)
            {
                newData += sets[i][j]->data;
                if (j != sets[i].size() - 1)
                    newData += ",";
            }

            newState->data = "[" + newData + "]";
            reduced_automata->states.push_back(newState);
        }

        // Añadiendo transiciones
        for (int i = 0; i < sets.size(); i++)
        {
            state* &stateA = sets[i][0];
            state* &stateB_0 = stateA->transitions[0]->stateEnd;

            int stateB_0_Index = -1;

            bool found = false;
            for (int j = 0; j < sets.size() && !found; j++)
            {
                if (findState(stateB_0, sets[j]) != nullptr)
                {
                    found = true;
                    stateB_0_Index = j;
                }
            }

            state* &stateB_1 = stateA->transitions[1]->stateEnd;

            int stateB_1_Index = -1;

            found = false;
            for (int j = 0; j < sets.size() && !found; j++)
            {
                if (findState(stateB_1, sets[j]) != nullptr)
                {
                    found = true;
                    stateB_1_Index = j;
                }
            }

            reduced_automata->states[i]->addTransition("0", reduced_automata->states[i], reduced_automata->states[stateB_0_Index]);
            reduced_automata->states[i]->addTransition("1", reduced_automata->states[i], reduced_automata->states[stateB_1_Index]);
        }

        // Añadiendo estado inicial
        bool found = false;
        int indexOfInitState = -1;
        for (int i = 0; i < sets.size() && !found; i++)
        {
            if (findState(initState, sets[i]))
            {
                found = true;
                indexOfInitState = i;
            }
        }

        reduced_automata->initState = reduced_automata->states[indexOfInitState];

        // Añadiendo estados finales
        for (int i = 0; i < finalStates.size(); i++)
        {
            bool found = false;
            int indexOfFinalState = -1;
            for (int j = 0; j < sets.size() && !found; j++)
            {
                if (findState(finalStates[i], sets[j]))
                {
                    found = true;
                    indexOfFinalState = j;
                }
            }

            state* &newFinalState = reduced_automata->states[indexOfFinalState];

            if(findState(newFinalState, reduced_automata->finalStates) == nullptr)
                reduced_automata->finalStates.push_back(newFinalState);
        }

        reduced_automata->numberOfFinalStates = reduced_automata->finalStates.size();
        

        return reduced_automata;
    }

    void print()
    {
        cout << numberOfStates << " " << initState->data << " " << numberOfFinalStates << " ";

        for (si = finalStates.begin(); si != finalStates.end(); ++si)
        {
            cout << (*si)->data;
            if (si != finalStates.end())
                cout << " ";
        }
        cout << endl;

        for(int i = 0; i < states.size(); i++)
        {
            for(int j = 0; j < states[i]->transitions.size(); j++)
            {
                cout << states[i]->transitions[j]->stateBegin->data << " ";
                cout << states[i]->transitions[j]->transitionChar << " ";

                if (states[i]->transitions[j]->stateEnd)
                    cout << states[i]->transitions[j]->stateEnd->data << " ";

                cout << endl;
            }
        }
    }

    void printNFA()
    {
        cout << numberOfStates << " ";

        for(si = initStates.begin(); si != initStates.end(); ++si)
            cout << (*si)->data << " ";

        cout << numberOfFinalStates << " ";

        for(si = finalStates.begin(); si != finalStates.end(); ++si)
            cout << (*si)->data << " ";

        cout << endl;

        for(si = states.begin(); si != states.end(); si++)
        {
            for(ti = (*si)->transitions.begin(); ti != (*si)->transitions.end(); ti++)
            {
                cout << (*ti)->stateBegin->data << " ";
                cout << (*ti)->transitionChar << " ";
                cout << (*ti)->stateEnd->data << " ";
                cout << endl;
            }
        }
    }


};

typedef Automata<Traits> automata;

#endif