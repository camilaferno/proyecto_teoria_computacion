#ifndef NODE_H
#define NODE_H

template <typename A>
struct State {
typedef typename A::S S;
typedef typename A::T T;
typedef typename A::state state;
typedef typename A::transition transition;
typedef typename A::TransitionSeq TransitionSeq;

TransitionSeq transitions;
S data;

State() {}

State(S value): data(value) {}

void addTransition(T transitionChar, state* begin, state* end)
{
  transition* newTransition = new transition(transitionChar, begin, end);
  transitions.push_back(newTransition);
}

};

#endif
