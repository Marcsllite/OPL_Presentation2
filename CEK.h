#ifndef CEK_H
#define CEK_H

#include "CEKInterface.h"
#include <iostream>  // cout, endl
#include <stdlib.h>  // exit();
#include <exception>

using std::cout;
using std::endl;
using std::to_string;
using std::invalid_argument;

//###### Expressions ######//
// Lambda Calc = X | LambdaX. M | M N
class Variable : public EXPRESSION {
public:
	string var;
	
	Variable(string var) { this->var = var; }
	string toString() { return var; }
};

class Abstraction : public EXPRESSION {
public:
	string var;
	EXPRESSION *body;

	Abstraction(string var, EXPRESSION *body) { this->var = var; this->body = body; }
	string toString() { return "(Lam" + var + ". " + body->toString() + ")"; }
};

class Application : public EXPRESSION {
public:
	EXPRESSION *appliedTo;
	EXPRESSION *applier;

	Application(EXPRESSION *appliedTo, EXPRESSION *applier) { this->appliedTo = appliedTo; this->applier = applier; }
	string toString() {
		return "(" + appliedTo->toString() + " " + applier->toString() + ")";
	}
};

// ISWIM = Lambda Calc - beta rule + (O, <b...>) + b + delta rule
class PrimitiveOp : public EXPRESSION {
public:
	string operation;
	LIST *variables;

	PrimitiveOp(string operation, LIST *variables) { this->operation = operation; this->variables = variables; }
	string toString() {
		return "(" + operation + " " + this->variables->toString() + ")";
	}

	// delta rule
	EXPRESSION *deltaRule() {
		// declaring variables
		int op = -1;

		// figuring out which operation we want to do
		(_strcmpi(operation.c_str(), "add1")) ? op = 0 : op = -1;
		(_strcmpi(operation.c_str(), "add")) ? op = 1 : op = -1;
		(_strcmpi(operation.c_str(), "sub")) ? op = 2 : op = -1;
		(_strcmpi(operation.c_str(), "div")) ? op = 3 : op = -1;
		(_strcmpi(operation.c_str(), "mult")) ? op = 4 : op = -1;

		switch (op) {
			case 0:  // add1
				// must have exactly 1 argument
				// argument must be number
				break;
			case 1:  // add
				// must have 2 arguments
				// arguments must be numbers
				break;
			case 2:  // sub
				// must have 2 arguments
				// arguments must be numbers
				break;
			case 3:  // div
				// must have 2 arguments
				// arguments must be numbers
				break;
			case 4:  // mult
				// must have 2 arguments
				// arguments must be numbers
				break;
			default:  // if op == -1
				throw invalid_argument("PrimitiveOp: Operation is invalid.");
		}
	}
};

// Values in ISWIM
class Closure : public EXPRESSION {
public:
	Abstraction *lam;
	ENVIRONMENT *env;

	Closure(Abstraction *lam, ENVIRONMENT *env) { this->lam = lam; this->env = env; }
	string toString() {
		return "clo(" + this->lam->toString() + ", " + this->env->toString() + ")";
	}
};

class Number : public EXPRESSION {
public:
	int num;
	Number(int num) { this->num = num; }
	string toString() { return to_string(num); }
};

//###### Expression Lists ######//
/*
	Describing the two kinds of lists that could occur
	Either the list is empty
			OR
	The list contains the constant argments (linked List) for the primitive operation
*/
class NullList : public LIST {
public:
	string toString() { return ""; }  // returning nothing for the null List
};

class ConstantList : public LIST {
public:
	EXPRESSION *firstEx;
	LIST *remaining;

	ConstantList(EXPRESSION *firstEx, LIST *remaining) { this->firstEx = firstEx; this->remaining = remaining; }

	string toString() {
		if (remaining->toString() != "") {
			return firstEx->toString() + " " + remaining->toString();
		} else {
			return firstEx->toString();
		}
	}
};

//###### Envirionments ######//
/*
	Describing the two kinds of environments that could occur
	Either the envirionment is empty
				OR
	There is an environment that has a variable with a value
*/
class NullEnvironment : public ENVIRONMENT {
public:
	string toString() { return "*"; }  // returning '*' to signify an empty environment
	EXPRESSION *search(string toFind) { 
		cout << "NullEnvironment: Unbound variable " << toFind << endl;  // tell user variable is unbound
		exit(1); // exit with error code 1 
	}
};

class ProperEnvironment : ENVIRONMENT {
private:
	ENVIRONMENT *env;  // keeping track of the locations of the many different environments (linked list)
	string variable;  // variable in current environment
	EXPRESSION *value;  // value of varibale in current environment
public:
	ProperEnvironment(string variable, EXPRESSION *value, ENVIRONMENT *env) {
		this->variable = variable;
		this->value = value;
		this->env = env;
	}

	string toString() { return this->env->toString() + "[" + variable + " -> " + this->value->toString() + "]"; }
	// seeing if variable we need is bound in this environment
	// if not looking through the next environment until we find 
	// the value needed or reach a NullEnvironment and end the program
	EXPRESSION *search(string toFind) {
		// if the variable we're looking for is bound in this environment
		if (variable == toFind) {
			return value;
		}
		else {  // else check other environments in linked list
			return env->search(toFind);
		}
	}
};

//###### KONTINUATIONS ######//
/*
	Describing the kinds of kontinuations that could occur
	Either we've finished parsing and at the returnKontinuation
				OR
	We're parsing the argument of an application
				OR
	We're parsing the function of an application
				OR
	We're parsing a primitive operation
*/
class returnKontinuation : public KONTINUATION {
public:
	string toString() { return "ret"; }  // simply returns 'ret' for return kontinuation
};

class argumentKontinuation : public KONTINUATION {
public:
	EXPRESSION *value;
	KONTINUATION *kont;

	argumentKontinuation(EXPRESSION *value, KONTINUATION *kont) { this->value = value; this->kont = kont; }
	string toString() {
		return "ar(" + this->value->toString() + " , " + this->kont->toString() + ")";
	}
};

class functionKontinuation : public KONTINUATION {
public:
	EXPRESSION *next;
	ENVIRONMENT *env;
	KONTINUATION *kont;

	functionKontinuation(EXPRESSION *next, ENVIRONMENT *env, KONTINUATION *kont) : next(next), env(env), kont(kont) {};
	string toString() {
		return "fn" + this->next->toString() + " , "
			        + this->env->toString() + " , "
			        + this->kont->toString() + ")";
	}
};

class primitiveKontinuation : public KONTINUATION {
public:
	string operation;
	LIST *variables;
	ENVIRONMENT *env;
	LIST *ms;
	KONTINUATION *kont;

	primitiveKontinuation(string operation, LIST *variables, ENVIRONMENT *env, LIST *ms, KONTINUATION *kont) :
		operation(operation), variables(variables), env(env), ms(ms), kont(kont) {};
	string toString() {
		return "pr(" + operation + ", <" + this->variables->toString()
			         + ">, <" + this->ms->toString() + ">, " 
			         + this->kont->toString() + ")";
	}
};

//###### CEK Machine ######
class CEK {
public:
	EXPRESSION *context;
	ENVIRONMENT *env;
	KONTINUATION *kont;

	CEK(EXPRESSION *context) { this->context = context; env = new NullEnvironment(); kont = new returnKontinuation(); }
	CEK(EXPRESSION *context, ENVIRONMENT *env, KONTINUATION *kont) :
		context(context), env(env), kont(kont) {};

	/**
		evaluates start state to end state of CEK machine

		@return the end state's context
	*/
	EXPRESSION *eval();

	/**
		moves to next state of machine given CEK rules

		@return next state of the CEK machine
				(each state of the CEK machine is a CEK)
	*/
	CEK *step();

	/**
		are we at the end state of the CEK machine (no more work left to be done)

		@return true if at end state else false
	*/
	bool done();

	string toString() { return this->context->toString();}
};
#endif // !CEK_H

