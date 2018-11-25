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

            for(size_t i = 3; i < vectorFirstLine.size(); i++)
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
                    cout << " state0: " << vectorLine[0];
                    addState(vectorLine[0]);
                }
                if(!isState(vectorLine[2]))
                {
                    cout << " state2: " << vectorLine[2];
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
            cout << (*si)->data;
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

        cout << "Transpuesto: " << endl;
        transpose->printNFA();
        cout << endl;
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

    void Equivalencia()
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

        // cout << "Equivalencia: " << endl;
        for (int i = 0; i < numberOfStates; i++)
        {
            if (i > 0)
                cout << states[i]->data << " ";
            for (int j = 0; j < i; j++)
                cout << matrix[i][j] << " ";
            cout << endl;
        }
        cout << "  ";
        for (int i = 0; i < numberOfStates; i++)
            cout << states[i]->data << " ";
        cout << endl;
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
        cout << "Init: " << init;
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

                    // cout << "Current: " << current << " NewNode1: " << newNode1 << " NewNode2: " << newNode2 << endl;

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

        /* while (!renamedTo.empty())
        {
            std::cout << renamedTo.begin()->first << " => " << renamedTo.begin()->second << '\n';
            renamedTo.erase(renamedTo.begin());
        }
        */
        cout << convertedToDFA->numberOfFinalStates << " ";
        cout << "NFA to DFA: " << endl;
        convertedToDFA->printNFA();
        cout << endl;
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

        for (std::map<string,int>::iterator ti=renamedTo.begin(); ti!=renamedTo.end(); ++ti)
        std::cout << ti->first << " => " << ti->second << '\n';
        cout << endl;

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
        cout << endl;

    }

    Automata* Brzozowski()
    {
        Automata* brzozowski = new Automata();
        Automata* renamed = new Automata;
        Automata* lastRenamed = new Automata;

        renamed = (getTranspose()->NFAtoDFA());
        renombramiento(renamed);
        cout << "Renamed NFAtoDFA" << endl;
        renamed->printNFA();

        lastRenamed = (renamed->getTranspose())->NFAtoDFA();
        renombramiento(lastRenamed);

        brzozowski= lastRenamed;

        brzozowski->printNFA();

        return brzozowski;
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
        for(size_t i = 0; i < where.size(); i++)
            if (where[i]->first == first && where[i]->second == second) return where[i];

        return nullptr;
    }

    void equivalence_improved()
    {
        vector<vector<bool>> distinctMatrix(numberOfStates, vector<bool>(numberOfStates, 1));
        vector<PairOfStates*> pairs;
        queue<PairOfStates*> pairQueue;
        map<state*, int> stateLocation;

        for(size_t i = 0; i < numberOfStates; i++)
        {
            stateLocation[states[i]] = i;
            for(size_t j = 0; j < numberOfStates; j++)
            {
                PairOfStates* newPair = new PairOfStates(states[i], states[j]);
                pairs.push_back(newPair);
            }
        }

        for(size_t i = 0; i < pairs.size(); i++)
        {
            PairOfStates* &pair = pairs[i];
            PairOfStates* dependency0 = findPairOfStates(pairs[i]->first->transitions[0]->stateEnd,
                                                        pairs[i]->second->transitions[0]->stateEnd,
                                                        pairs);

            PairOfStates* dependency1 = findPairOfStates    (pairs[i]->first->transitions[1]->stateEnd,
                                                            pairs[i]->second->transitions[1]->stateEnd,
                                                            pairs);

            if (dependency0)
                pair->dependencies.push_back(dependency0);
            else
                pair->addDependency(pairs[i]->first->transitions[0]->stateEnd,
                                    pairs[i]->second->transitions[0]->stateEnd);

            if (dependency1)
                pair->dependencies.push_back(dependency1);
            else
                pair->addDependency(pairs[i]->first->transitions[1]->stateEnd,
                                    pairs[i]->second->transitions[1]->stateEnd);

        }

        // Inicializa con 1 los pares (p,q) donde p es estado final y q no
        for(size_t i = 0; i < numberOfStates; i++)
        {
            if (findState(states[i], finalStates))
            {
                for(size_t j = 0; j < numberOfStates; j++)
                {
                    if (states[j] != states[i] || findState(states[j], finalStates) == 0)
                    {
                        distinctMatrix[i][j] = 0;
                        distinctMatrix[j][i] = 0;
                    }
                }
            }
        }

        for (size_t i = 0; i < pairs.size(); i++)
        {
            PairOfStates* &pair = pairs[i];
            int pairFirstLocation = stateLocation[pair->first];
            int pairSecondLocation = stateLocation[pair->second];

            if (distinctMatrix[pairFirstLocation][pairSecondLocation] == 0)
            {
                vector<PairOfStates*> &listOfDependencies = pair->dependencies;
                for (size_t j = 0; j < listOfDependencies.size(); j++)
                {
                    PairOfStates* &currentDependency = listOfDependencies[j];
                    int depFirstLocation = stateLocation[currentDependency->first];
                    int depSecondLocation = stateLocation[currentDependency->second];

                    if (distinctMatrix[depFirstLocation][depSecondLocation] == 1 && depFirstLocation != depSecondLocation)
                    {
                        distinctMatrix[depFirstLocation][depSecondLocation] = distinctMatrix[depSecondLocation][depFirstLocation] = 0;
                        pairQueue.push(currentDependency);
                    }
                }
                break;
            }
        }
        while (!pairQueue.empty())
        {
            PairOfStates* pair = pairQueue.front();
            int pairFirstLocation = stateLocation[pair->first];
            int pairSecondLocation = stateLocation[pair->second];

            if (distinctMatrix[pairFirstLocation][pairSecondLocation] == 0)
            {
                vector<PairOfStates*> &listOfDependencies = pair->dependencies;
                for (size_t j = 0; j < listOfDependencies.size(); j++)
                {
                    PairOfStates* &currentDependency = listOfDependencies[j];
                    int depFirstLocation = stateLocation[currentDependency->first];
                    int depSecondLocation = stateLocation[currentDependency->second];

                    if (distinctMatrix[depFirstLocation][depSecondLocation] == 1 && depFirstLocation != depSecondLocation)
                    {
                        distinctMatrix[depFirstLocation][depSecondLocation] = distinctMatrix[depSecondLocation][depFirstLocation] = 0;
                        pairQueue.push(currentDependency);
                    }else
                    {
                        if (!pairQueue.empty())pairQueue.pop();
                    }
                }
            }

        }

        for(size_t i = 0; i < numberOfStates; i++)
        {
            cout << states[i]->data << " ";
            for(size_t j = 0; j <= i; j++)
                cout << distinctMatrix[i][j] << " ";
            cout << endl;
        }

        cout << "  ";
        for(size_t i = 0; i < numberOfStates; i++)
            cout << states[i]->data << " ";
        cout << endl;

    }


    void print()
    {
        for(si = states.begin(); si != states.end(); si++)
        {
            cout << (*si)->data << " ";
        }
        cout << endl;

        cout << numberOfStates << " " << initState->data << " " << numberOfFinalStates << " ";

        for (si = finalStates.begin(); si != finalStates.end(); ++si)
        {
            cout << (*si)->data;
            if (si != finalStates.end())
                cout << " ";
        }
        cout << endl;

        for(size_t i = 0; i < states.size(); i++)
        {
            for(size_t j = 0; j < states[i]->transitions.size(); j++)
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
