#ifndef CEK_H
#define CEK_H

#include "CEKInterface.h"
#include <iostream>  // cout, endl
#include <stdlib.h>  // exit();
#include <exception>
#include <string>
#include <sstream>

using std::cout;
using std::endl;
//using std::to_string;
using std::invalid_argument;
using std::logic_error;
using std::stringstream;

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

	Abstraction(string var, EXPRESSION *body) { this->var = var; this->body = body;}
	~Abstraction() { delete &body; }
	string toString() { return "(Lambda" + var + ". " + body->toString() + ")"; }
};

class Application : public EXPRESSION {
 public:
	EXPRESSION *appliedTo;
	EXPRESSION *applier;

	Application(EXPRESSION *appliedTo, EXPRESSION *applier) { this->appliedTo = appliedTo; this->applier = applier;}
	~Application() { delete &appliedTo; delete &applier; }
	string toString() {
		return "(" + appliedTo->toString() + " " + applier->toString() + ")";
	}
};

// ISWIM = Lambda Calc - beta rule + (O, <b...>) + b + delta rule
class PrimitiveOp : public EXPRESSION {
 public:
	string operation;
	LIST *arguments;

	PrimitiveOp(string operation, LIST *arguments) { this->operation = operation; this->arguments = arguments;}
	~PrimitiveOp() { delete &arguments; }
	string toString() {
		return "(" + operation + " " + this->arguments->toString() + ")";
	}

	// delta rule
	//EXPRESSION *deltaRule() {
		// declaring variables
		//int op = -1;

		// figuring out which operation we want to do
		// (strcmpi(operation.c_str(), "add1")) ? op = 0 : op = -1;
		// (strcmpi(operation.c_str(), "add")) ? op = 1 : op = -1;
		// (strcmpi(operation.c_str(), "sub")) ? op = 2 : op = -1;
		// (strcmpi(operation.c_str(), "div")) ? op = 3 : op = -1;
		// (strcmpi(operation.c_str(), "mult")) ? op = 4 : op = -1;

		// switch (op) {
		// 	case 0:  // add1
		// 		// must have exactly 1 argument
		// 		// argument must be number
		// 		break;
		// 	case 1:  // add
		// 		// must have 2 arguments
		// 		// arguments must be numbers
		// 		break;
		// 	case 2:  // sub
		// 		// must have 2 arguments
		// 		// arguments must be numbers
		// 		break;
		// 	case 3:  // div
		// 		// must have 2 arguments
		// 		// arguments must be numbers
		// 		break;
		// 	case 4:  // mult
		// 		// must have 2 arguments
		// 		// arguments must be numbers
		// 		break;
		// 	default:  // if op == -1
		// 		throw invalid_argument("PrimitiveOp: Operation is invalid.");
		// }
	//}
};

// Values in ISWIM
class Closure : public EXPRESSION {
 public:
	Abstraction *lam;
	ENVIRONMENT *env;

	Closure(Abstraction *lam, ENVIRONMENT *env) { this->lam = lam; this->env = env;}
	~Closure() { delete &lam; delete &env; }
	string toString() {
		return "clo(" + this->lam->toString() + ", " + this->env->toString() + ")";
	}
};

class Number : public EXPRESSION {
 public:
	int num;
	Number(int num) { this->num = num; }
	string toString() {
		stringstream ss;
		ss << num;
		return ss.str(); }
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

	ConstantList(EXPRESSION *firstEx, LIST *remaining) { this->firstEx = firstEx; this->remaining = remaining;}
	~ConstantList() { delete &firstEx; delete &remaining; }

	string toString() {
		if (remaining->toString() != "") {
			return firstEx->toString() + " " + remaining->toString();
		} else {
			return firstEx->toString();
		}
	}
};

// CESK Machine = CEK Machine + pointer deref(pointer) + set(pointer, value)
class Pointer : public EXPRESSION {
 EXPRESSION *pointer;

 public:
	Pointer(EXPRESSION *pointer) : pointer(pointer) {};
	~Pointer() {delete &pointer; }

	string toString(){
		return  pointer->toString();
	}
};

class Deref : public EXPRESSION {
 public:
	EXPRESSION *pointer;

	Deref(EXPRESSION *pointer) : pointer(pointer) {};
	~Deref(){ delete &pointer; }

	string toString(){
		return "(deref " + pointer->toString() + ")";
	}
};

class Set : public EXPRESSION {
 public:
	Pointer *pName;  // name of pointer
	EXPRESSION *value;  // value you want to give pointer

	Set(Pointer *pName, EXPRESSION *value) : pName(pName), value(value) {};
	~Set(){ delete &pName; delete&value;};

	string toString(){
		return "set! (" + pName->toString() + " " + value->toString() + ")";
	}
};

//###### Environments ######//
/*
	Describing the two kinds of environments that could occur
	Either the environment is empty
				OR
	There is an environment that has a variable with a value
*/
class NullEnvironment : public ENVIRONMENT {
 public:
	string toString() { return "*"; }  // returning '*' to signify an empty environment
	EXPRESSION *search(string toFind) {
		throw logic_error("NullEnvironment: Unbound variable " + toFind);
	}
};

class ProperEnvironment : public ENVIRONMENT {
 private:
	ENVIRONMENT *env;  // keeping track of the locations of the many different environments
	string variable;  // variable in current environment
	EXPRESSION *value;  // value of variable in current environment
 public:
	ProperEnvironment(string variable, EXPRESSION *value, ENVIRONMENT *env) {
		this->variable = variable;
		this->value = value;
		this->env = env;
	}
	~ProperEnvironment() { delete &env; delete &value; }

	string toString() { return this->env->toString() + "[" + variable + " -> " + this->value->toString() + "]"; }
	// seeing if variable we need is bound in this environment
	// if not looking through the next environment until we find
	// the value needed or reach a NullEnvironment and end the program
	EXPRESSION *search(string toFind) {
		// if the variable we're looking for is bound in this environment
		if (!variable.compare(toFind)) {
			// cout << "ProperEnvironment Search: toFind = " << toFind
			//      << " variable = " << variable << endl;
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

	argumentKontinuation(EXPRESSION *value, KONTINUATION *kont) { this->value = value; this->kont = kont;}
	~argumentKontinuation() { delete &value; delete &kont; }

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
	~functionKontinuation() { delete &next; delete &env; delete &kont; }

	string toString() {
		return "fn(" + this->next->toString() + " , "
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
	~primitiveKontinuation() { delete &variables; delete &ms; delete &kont; }

	string toString() {
		return "pr(" + operation + ", <" + this->variables->toString()
			         + ">, <" + this->ms->toString() + ">, "
			         + this->kont->toString() + ")";
	}
};

// State Kontinuation
class setKontinuation : public KONTINUATION {
 public:
};

//###### STORES ######//
/*
	Describing the two kinds of environments that could occur
	Either the store is empty
				OR
	There is an store that has a pointer with a value
*/
class NullStore : public STORE {
 public:
 	string toString() { return "*"; }
 	EXPRESSION *search(EXPRESSION *toFind) {
		throw logic_error("NullStore: " + toFind->toString() + " was not stored in store.");
	}
};

class ProperStore : public STORE {
 private:
	STORE *store;  // keeping track of the locations of the many different stores
	EXPRESSION *pointer;  // pointer in current store
	EXPRESSION *value;  // value of pointer in current store
 public:
	ProperStore(EXPRESSION *pointer, EXPRESSION *value, STORE *store) {
		this->pointer = pointer;
		this->value = value;
		this->store = store;
	}
	~ProperStore() { delete &store; delete &pointer; delete &value; }

	string toString() { return this->store->toString() + "[" + pointer->toString() + " -> " + this->value->toString() + "]"; }
	// seeing if pointer we need is bound in the store
	// if not looking through the next store in list until we find
	// the value needed or reach a NullStore and end the program
	EXPRESSION *search(EXPRESSION *toFind) {
		// if the pointer we're looking for is bound in this store
		if (!pointer->toString().compare(toFind->toString())) {
			return value;
		}
		else {  // else check remaining store in list
			return store->search(toFind);
		}
	}
};

//###### CEK Machine ######
class CEK {
 public:
	EXPRESSION *context;
	ENVIRONMENT *env;
	KONTINUATION *kont;

	CEK(EXPRESSION *context) { this->context = context; env = new NullEnvironment(); kont = new returnKontinuation();}
	CEK(EXPRESSION *context, ENVIRONMENT *env, KONTINUATION *kont) : context(context), env(env), kont(kont) {};
	~CEK() { delete[] &context; delete[] &env; delete[] &kont; }

	/**
		evaluates start state to end state of CEK machine

		@return the end state's context
	*/
	EXPRESSION *eval();

	/**
		moves to next state of machine given CEK rules
	*/
	void step();

	/**
		are we at the end state of the CEK machine (no more work left to be done)

		@return true if at end state else false
	*/
	bool done();

	string print();
	string toString() { return this->context->toString();}
};

//###### CESK Machine ######
class CESK {
 public:
	EXPRESSION *context;
	ENVIRONMENT *env;
	STORE *store;
	KONTINUATION *kont;

	CESK(EXPRESSION *context) {
		this->context = context; env = new NullEnvironment(); store = new NullStore(); kont = new returnKontinuation();}
	CESK(EXPRESSION *context, ENVIRONMENT *env, STORE *store, KONTINUATION *kont) :
	context(context), env(env), store(store), kont(kont) {};
	~CESK() { delete[] &context; delete[] &env; delete[] &store, delete[] &kont; }

	/**
		evaluates start state to end state of CESK machine

		@return the end state's context
	*/
	EXPRESSION *eval();

	/**
		moves to next state of machine given CESK rules
	*/
	void step();

	/**
		are we at the end state of the CESK machine (no more work left to be done)

		@return true if at end state else false
	*/
	bool done();

	string print();
	string toString() { return this->context->toString();}
};
#endif // !CEK_H

