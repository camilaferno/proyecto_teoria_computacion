#ifndef EDGE_H
#define EDGE_H

#include "state.h"
#include "tuple"

using namespace std;

template <typename A>
struct Transition {
typedef typename A::T T;
typedef typename A::state state;

T transitionChar;
state* stateBegin;
state* stateEnd;

Transition() {};

Transition  (T transitionChar,
            state* begin,
            state* end):

            transitionChar(transitionChar),
            stateBegin(begin),
            stateEnd(end) {};

~Transition() {};

};

#endif
