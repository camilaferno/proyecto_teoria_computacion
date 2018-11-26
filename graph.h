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
    map<string, string> renamedTo;

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
            states.push_back(initState);

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

                if(!isState(vectorLine[0])) addState(vectorLine[0]);
                if(!isState(vectorLine[2])) addState(vectorLine[2]);

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
        
        transpose->numberOfFinalStates = finalStates.size();
        transpose->initState = finalStates.front();

        /* cout << "Transpuesto: " << endl;
        transpose->printNFA();
        cout << endl; */
        return transpose;
    }

    void unionOfStates(string &newNode1, string &newNode2, string current)
    {
        for(int i = 0; i < current.size(); i++)
        {
            string character = current.substr(i, 1);

            for(auto stateit = states.begin(); stateit != states.end(); ++stateit)
            {
                if(character == (*stateit)->data)
                {
                    for(ti = (*stateit)->transitions.begin(); ti != (*stateit)->transitions.end(); ti++)
                    {
                        if((*ti)->transitionChar == "0")
                            newNode1 += (*ti)->stateEnd->data;
                        if((*ti)->transitionChar == "1")
                            newNode2 += (*ti)->stateEnd->data;
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
    
    void Hopcroft()
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

        set<set<state*>> newF, newq0;
        for (typename set<set<state*>>::iterator it = P.begin(); it != P.end();){
            bool go = true;
            if ((*it).count(initState) != 0) {
                typename set<set<state*>>::iterator tmp = it;
                if (F.count(*((*it).begin())) != 0) {
                    typename set<set<state*>>::iterator tmp = it;
                    newF.insert(*tmp);
                }
                ++it;
                P.erase(*tmp);
                newq0.insert(*tmp);
                go = false;
            }
            else if (F.count(*((*it).begin())) != 0) {
                typename set<set<state*>>::iterator tmp = it;
                ++it;
                P.erase(*tmp);
                newF.insert(*tmp);
                go = false;
            }

            if (go)
                ++it;
        }

        /* for (typename set<set<state*>>::iterator it = P.begin(); it != P.end();){
            bool go = true;
            
            if (F.count(*((*it).begin())) != 0) {
                typename set<set<state*>>::iterator tmp = it;
                //--it;
                ++it;
                P.erase(*tmp);
                newF.insert(*tmp);
                go = false;
            }
            if (go)
                ++it;
        } */
        
        cout << P.size() + newq0.size() + newF.size() << " ";
        for (typename set<state*>::iterator it = (*(newq0.begin())).begin(); it != (*(newq0.begin())).end(); ++it)
            cout << (*it)->data;
        cout << " " << newF.size();
        for (typename set<set<state*>>::iterator it = newF.begin(); it != newF.end(); ++it){
            cout << " ";
            for (typename set<state*>::iterator it2 = (*it).begin(); it2 != (*it).end(); ++it2)
                cout << (*it2)->data;
        }
        cout << endl;
        /* for (typename set<set<state*>>::iterator it = newq0.begin(); it != newq0.end(); ++it){
            //cout << "X" << endl;
            for (int d = 0; d < 2; d++){
                for (typename set<state*>::iterator it2 = (*it).begin(); it2 != (*it).end(); ++it2)
                    cout << "w" << endl;
                    //cout <<(*it2)->data;

                //cout << " " << d << " " << (*((*it).begin()))->transitions[d]->stateEnd->data << endl;
                cout << " " << d << " " << (*((*it).begin())) << endl;
            }
        } */
        for (typename set<set<state*>>::iterator it = P.begin(); it != P.end(); ++it){
            for (int d = 0; d < 2; d++){
                for (typename set<state*>::iterator it2 = (*it).begin(); it2 != (*it).end(); ++it2)
                    cout << (*it2)->data;
                cout << " " << d << " " << (*((*it).begin()))->transitions[d]->stateEnd->data << endl;
            }
        }
        for (typename set<set<state*>>::iterator it = newF.begin(); it != newF.end(); ++it){
            for (int d = 0; d < 2; d++){
                for (typename set<state*>::iterator it2 = (*it).begin(); it2 != (*it).end(); ++it2)
                    cout << (*it2)->data;
                cout << " " << d << " " << (*((*it).begin()))->transitions[d]->stateEnd->data << endl;
            }
        }

    };

    Automata* NFAtoDFA()
    {
        Automata* convertedToDFA = new Automata();
        queue<S> myqueue;
        set<S> AddedToQueue;

        string newNode1 = "?";
        string newNode2 = "?";

        //Agregar stados inciales
        for(auto it = initStates.begin(); it != initStates.end(); ++it)
        {
            convertedToDFA->addState((*it)->data);
            convertedToDFA->initStates.push_back(convertedToDFA->findState((*it)->data));
            convertedToDFA->numberOfStates += 1;
        }

        for(auto it = states.begin(); it != states.end(); it++)
        {
            bool found = false;
            if(isInitState((*it)->data))
            {
                found = true;
                // Suma los nombres de los estados
                for(ti = (*it)->transitions.begin(); ti != (*it)->transitions.end(); ti++)
                {
                    if((*ti)->transitionChar == "0")
                        newNode1 += (*ti)->stateEnd->data;

                    else if((*ti)->transitionChar == "1")
                        newNode2 += (*ti)->stateEnd->data;
                }

                // Borra "?"
                if(newNode2.find("?") != string::npos && newNode2.size()>1)
                    newNode2.erase(remove(newNode2.begin(), newNode2.end(), '?'), newNode2.end());

                if(newNode1.find("?") != string::npos && newNode1.size()>1)
                    newNode1.erase(remove(newNode1.begin(), newNode1.end(), '?'), newNode1.end());

                sort(newNode1.begin(), newNode1.end());
                sort(newNode2.begin(), newNode2.end());

                if(!convertedToDFA->isState(newNode1))
                {
                    bool isSalida1 = false;
                    for(auto& salida: finalStates)
                    {
                        // Si dentro de newNode1 hay un elemento de salida
                        if(newNode1.find((salida)->data) != string::npos)
                        {
                            convertedToDFA->addState(newNode1);
                            convertedToDFA->finalStates.push_back(convertedToDFA->findState(newNode1));
                            convertedToDFA->numberOfFinalStates += 1;
                            convertedToDFA->numberOfStates += 1;
                            isSalida1=true;
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
                    for(auto& salida: finalStates)
                    {
                        // Si dentro de newNode1 hay un elemento de salida
                        if(newNode2.find((salida)->data) != string::npos)
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

                (convertedToDFA->findState((*it)->data))->addTransition ("0",
                                                                        convertedToDFA->findState((*it)->data),
                                                                        convertedToDFA->findState(newNode1));

                (convertedToDFA->findState((*it)->data))->addTransition ("1",
                                                                        convertedToDFA->findState((*it)->data),
                                                                        convertedToDFA->findState(newNode2));
            }

            if (found)
            {
                if (AddedToQueue.find(newNode1) == AddedToQueue.end() && !isInitState(newNode1))
                {
                    myqueue.push(newNode1);
                    AddedToQueue.insert(newNode1);
                }

                if (AddedToQueue.find(newNode2) == AddedToQueue.end() && !isInitState(newNode2))
                {
                    myqueue.push(newNode2);
                    AddedToQueue.insert(newNode2);
                }
                newNode1 = "?";
                newNode2 = "?";
            }
        }

        // Maybe we dont need this
        newNode1.clear();
        newNode2.clear();

        while (!myqueue.empty())
        {
            newNode1 = "?";
            newNode2 = "?";

            if (myqueue.front() == "?" && !myqueue.empty())
            {
                state* vacio = convertedToDFA->findState("?");
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
                    if(current.size() > 1)
                        unionOfStates(newNode1, newNode2, current);
                    else
                    {
                        for(ti = (*it)->transitions.begin(); ti != (*it)->transitions.end(); ti++)
                        {
                            if((*ti)->transitionChar == "0")
                                newNode1 += (*ti)->stateEnd->data;

                            if((*ti)->transitionChar == "1")
                                newNode2 += (*ti)->stateEnd->data;
                        }
                    }

                    if(newNode2.find("?") != string::npos && newNode2.size() > 1)
                        newNode2.erase(remove(newNode2.begin(), newNode2.end(), '?'), newNode2.end());
                    
                    if(newNode1.find("?") != string::npos && newNode1.size()>1)
                        newNode1.erase(remove(newNode1.begin(), newNode1.end(), '?'), newNode1.end());
                    

                    sort(newNode1.begin(), newNode1.end());
                    sort(newNode2.begin(), newNode2.end());

                    // cout << "Current: " << current << " NewNode1: " << newNode1 << " NewNode2: " << newNode2 << endl;

                    // If newNode1 is not in the nodes vector
                    if(!convertedToDFA->isState(newNode1))
                    {
                        bool isSalida1 = false;

                        for (auto& salida: finalStates)
                        {
                            //Si dentro de newNode1 hay un elemento de salida
                            if (newNode1.find((salida)->data) != string::npos)
                            {
                                convertedToDFA->addState(newNode1);
                                convertedToDFA->finalStates.push_back(convertedToDFA->findState(newNode1));
                                convertedToDFA->numberOfFinalStates += 1;
                                convertedToDFA->numberOfStates += 1;
                                isSalida1 = true;
                                break;
                            }
                        }
                        if (!isSalida1)
                        {
                            convertedToDFA->numberOfStates += 1;
                            convertedToDFA->addState(newNode1);
                        }
                        myqueue.push(newNode1);
                    }

                    // If newNode2 is not in the nodes vector
                    if(!convertedToDFA->isState(newNode2))
                    {
                        bool isSalida1 = false;
                        for(auto& salida: finalStates)
                        {
                            // Si dentro de newNode1 hay un elemento de salida
                            if(newNode2.find((salida)->data) != string::npos)
                            {
                                convertedToDFA->addState(newNode2);
                                convertedToDFA->finalStates.push_back(convertedToDFA->findState(newNode2));
                                convertedToDFA->numberOfFinalStates += 1;
                                convertedToDFA->numberOfStates += 1;
                                isSalida1 = true;
                                break;
                            }
                        }

                        if(!isSalida1)
                        {
                            convertedToDFA->numberOfStates += 1;
                            convertedToDFA->addState(newNode2);
                        }
                        
                        myqueue.push(newNode2);
                    }

                    (convertedToDFA->findState(myqueue.front()))->addTransition ("0",
                                                                                convertedToDFA->findState(myqueue.front()),
                                                                                convertedToDFA->findState(newNode1));

                    (convertedToDFA->findState(myqueue.front()))->addTransition ("1",
                                                                                convertedToDFA->findState(myqueue.front()),
                                                                                convertedToDFA->findState(newNode2));

                    newNode1 = "?";
                    newNode2 = "?";
                    myqueue.pop();
                }

            }
        }

        /* while (!renamedTo.empty())
        {
            std::cout << renamedTo.begin()->first << " => " << renamedTo.begin()->second << '\n';
            renamedTo.erase(renamedTo.begin());
        }
        cout << "NFA to DFA: " << endl;
        convertedToDFA->printNFA();
        cout << endl; */
        return convertedToDFA;
    }

    void renombramiento(Automata* &rename)
    {
        string tmp = "0";
        int count = 0;

        for(si = rename->states.begin(); si != rename->states.end(); ++si)
        {
            if(((*si)->data).size() == 1)
            {
                if(tmp < (*si)->data)
                    tmp = (*si)->data;
            }
        }

        string nextLetter(1, static_cast<char>(tmp[0] + 1));

        for(auto it = rename->states.begin(); it != rename->states.end(); ++it)
        {
            if (((*it)->data).size() > 1)
            {
                if (count == 0)
                {
                    rename->renamedTo.insert({(*it)->data, nextLetter});

                    if(isInitState((*it)->data))
                        (*it)->data = nextLetter;
                        
                    if(isFinalState((*it)->data))
                        (*it)->data = nextLetter;

                    (*it)->data = nextLetter;
                    count = 1;
                }
                else if (count > 0)
                {
                    string nextnextLetter(1, static_cast<char>(nextLetter[0] + count));
                    rename->renamedTo.insert({(*it)->data, nextnextLetter});

                    if(isInitState((*it)->data))
                        (*it)->data = nextLetter;

                    if(isFinalState((*it)->data))
                        (*it)->data = nextLetter;

                    (*it)->data = nextnextLetter;
                    count += 1;
                }
            }
            else if (((*it)->data) == "?")
                (*it)->data = "N";  
        }
    }

    Automata* Brzozowski()
    {
        Automata* brzozowski = new Automata();
        Automata* renamed = new Automata;

        renamed = (getTranspose()->NFAtoDFA())->getTranspose();
        renombramiento(renamed);
        /* cout << endl;
        cout << "Renamed: " << endl; */
        renamed->printNFA();
        // cout << endl;
        brzozowski = renamed->NFAtoDFA();

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
