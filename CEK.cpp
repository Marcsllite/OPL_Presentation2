#include "CEK.h"

EXPRESSION * CEK::eval() {
	CEK *stepped;
	int laps = 0;
	while (!this->done() && laps < 10) {  // checking if we're at the done state
		stepped = this->step();  // stepping CEK machine

		// updating current CEK state to be that of stepped machine
		this->context = stepped->context;
		this->env = stepped->env;
		this->kont = stepped->kont;

		laps++;
	}  

	return this->context;  // returning the context at the done state
}

CEK * CEK::step() {
	// <X, E, K> --> <V, E, K>
	return new CEK(new Variable("//TO-DO"));
}

bool CEK::done() {
	return (dynamic_cast<Number*>(this->context) || dynamic_cast<Closure*>(this->context)  // checking if context is a closure or number
			&& dynamic_cast<returnKontinuation*>(this->context));  // checking if at return continuation
}