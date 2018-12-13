#include "CEK.h"

EXPRESSION * CEK::eval() {
	int laps = 0;
	// if(this->done()){
	// 	cout << "State " << laps << ": " << this->print() << endl;
	// }
	while (!this->done() && laps < 10) {  // checking if we're at the done state
		cout << "State " << laps << ": " << this->print() << endl;
		cout << "Environment: " << this->env->toString() << endl;
		this->step();  // stepping CEK machine
		laps++;
	}
	return this->context;  // returning the context at the done state
}

void CEK::step() {
	// <Variable, E, K> --> <value, E, K>
	if (Variable *var = dynamic_cast<Variable*>(this->context)) {  // if context is a variable
		context = this->env->search(var->var);  // looking in the environment searching for variable's value
	}

	// <Application, E, K> --> <appliedTo, E, fn(applier, E, K)>
	else if (Application *app = dynamic_cast<Application*>(this->context)) {  // if context is an application
		/*
			focus on first part of application and note in the continuation
		 	that you're looking at the function part of the application
		 	and still need to go back to the argument part
		 	also must save current environment and kontinuation because
		 	the first part of the application could be something that has different environment/kontinuation
		*/
		if(dynamic_cast<Number*>(app->appliedTo)){  // if the first part of the application is a number
			context = new Variable("ERROR: First part of application cannot be a number.");
			env = new NullEnvironment();
			kont = new returnKontinuation();
		} else {
			context = app->appliedTo;
			kont = new functionKontinuation(app->applier, this->env, this->kont);
		}
	}

	// <Abstraction, E, K> --> <closure(Abstraction, E), E, K>
	else if (Abstraction *abs = dynamic_cast<Abstraction*>(this->context)) {  // if context is an abstraction
		context = new Closure(abs, this->env);
	}

	// <Value, E, fn(applier, E', K)> --> <applier, E', ar(Value, K)>
	else if (functionKontinuation *fn = dynamic_cast<functionKontinuation*>(this->kont)) {  // if kontinuation is a function kontinuation
		// new state is whatever we were doing before we finished parsing the function
		if(Closure *clo = dynamic_cast<Closure*>(this->context)) {  // if context is Closure
			context = fn->next;
			env = clo->env;
			kont = new argumentKontinuation(clo, fn->kont);
		}
		else if(dynamic_cast<Number*>(this->context)){  // if context is Number
			context = new Variable("ERROR: First part of application cannot be a number.");
			env = new NullEnvironment();
			kont = new returnKontinuation();
		}
	}

	// <PrimitiveOp, E, K> --> <first, E, primitiveKontinuation(operation, null list, E, remaining, K)>
	else if (PrimitiveOp *prim = dynamic_cast<PrimitiveOp*>(this->context)) {  // if context is a primitive operation
		if (ConstantList *primList = dynamic_cast<ConstantList*>(prim->arguments)) {  // if arguments are a list of constants
			if (dynamic_cast<Number*>(primList->firstEx)) {  // if the first argument in the list is a number
				context = primList->firstEx;
				kont = new primitiveKontinuation(prim->operation, new NullList, this->env, primList->remaining, this->kont);
			} else {  // else we cannot complete the operation (currently all operations are only number based)
				context = new Variable("ERROR: argument must be a number.");
				env = new NullEnvironment();
				kont = new returnKontinuation();
			}
		} else {  // arguments are not list of constants
			context = new Variable("ERROR: arguments of primitive operation is either null or not a list.");
			env = new NullEnvironment();
			kont = new returnKontinuation();
		}
	}

	// <Value, E, ar(closure(Abstraction, Ep), K)> --> <functBody, Ep[X -> Value], K>
	else if (argumentKontinuation *ar = dynamic_cast<argumentKontinuation*>(this->kont)) {  // if kontinuation is a function kontinuation
		if(dynamic_cast<Number*>(this->context) || dynamic_cast<Closure*>(this->context)){  // if a value
			if (Closure *clo = dynamic_cast<Closure*>(ar->value)) {  // if the value of the argument is a closure
				env = new ProperEnvironment(clo->lam->var, this->context, clo->env);
				context = clo->lam->body;
				kont = ar->kont;
			} else {
				context = new Variable("ERROR: value of argument is not a closure");
				env = new NullEnvironment();
				kont = new returnKontinuation();
			}
		}
	}
	// at returnKontinuation
	else if (dynamic_cast<returnKontinuation*>(this->kont)){
		return;
	}

	else {
		context = new Variable("ERROR: Bad CEK arguments");
		env = new NullEnvironment();
		kont = new returnKontinuation();
	}
}

string CEK::print(){
	return "<" + this->context->toString()
	     + " , " + this->env->toString()
	     + " , " + this->kont->toString() + ">";
}

bool CEK::done() {
	return ((dynamic_cast<Number*>(this->context) || dynamic_cast<Closure*>(this->context))  // checking if context is a closure or number
			&& dynamic_cast<returnKontinuation*>(this->kont));  // checking if at return continuation
}

EXPRESSION * CESK::eval() {
	int laps = 0;
	if(this->done()){
		cout << "State " << laps << ": " << this->print() << endl;
	}
	while (!this->done() && laps < 10) {  // checking if we're at the done state
		cout << "State " << laps << ": " << this->print() << endl;
		cout << "Environment: " << this->env->toString() << endl;
		cout << "Store: " << this->store->toString() << endl;
		this->step();  // stepping CEK machine
		laps++;
	}
	return this->context;  // returning the context at the done state
}

void CESK::step() {
	// ####################################### OLD CEK STEPS ###################################
	// <Variable, E, K> --> <value, E, K>
	if (Variable *var = dynamic_cast<Variable*>(this->context)) {  // if context is a variable
		context = this->env->search(var->var);  // looking in the environment searching for variable's value
	}

	// ## CEK <Application, E, K> --> <appliedTo, E, fn(applier, E, K)>
	// ## CESK <Application, E, S, K> --> <appliedTo, E, S, fn(applier, E, K)>
	else if (Application *app = dynamic_cast<Application*>(this->context)) {  // if context is an application
		/*
			focus on first part of application and note in the continuation
		 	that you're looking at the function part of the application
		 	and still need to go back to the argument part
		 	also must save current environment and kontinuation because
		 	the first part of the application could be something that has different environment/kontinuation
		*/
		if(dynamic_cast<Number*>(app->appliedTo)){  // if the first part of the application is a number
			context = new Variable("ERROR: First part of application cannot be a number.");
			env = new NullEnvironment();
			kont = new returnKontinuation();
		} else {
			context = app->appliedTo;
			kont = new functionKontinuation(app->applier, this->env, this->kont);
		}
	}

	// <Abstraction, E, K> --> <closure(Abstraction, E), E, K>
	else if (Abstraction *abs = dynamic_cast<Abstraction*>(this->context)) {  // if context is an abstraction
		context = new Closure(abs, this->env);
	}

	// ## CEK <Value, E, fn(applier, E', K)> --> <applier, E', ar(Value, K)>
	// ## CESK <Value, E, S, fn(applier, E', K)> --> <applier, E', S, ar(Value, K)>  // Store stays the same
	else if (functionKontinuation *fn = dynamic_cast<functionKontinuation*>(this->kont)) {  // if kontinuation is a function kontinuation
		// new state is whatever we were doing before we finished parsing the function
		if(Closure *clo = dynamic_cast<Closure*>(this->context)) {  // if context is Closure
			context = fn->next;
			env = clo->env;
			kont = new argumentKontinuation(clo, fn->kont);
		}
		else if(dynamic_cast<Number*>(this->context)){  // if context is Number
			context = new Variable("ERROR: First part of application cannot be a number.");
			env = new NullEnvironment();
			kont = new returnKontinuation();
		}
	}

	// <PrimitiveOp, E, K> --> <first, E, primitiveKontinuation(operation, null list, E, remaining, K)>
	else if (PrimitiveOp *prim = dynamic_cast<PrimitiveOp*>(this->context)) {  // if context is a primitive operation
		if (ConstantList *primList = dynamic_cast<ConstantList*>(prim->arguments)) {  // if arguments are a list of constants
			if (dynamic_cast<Number*>(primList->firstEx)) {  // if the first argument in the list is a number
				context = primList->firstEx;
				kont = new primitiveKontinuation(prim->operation, new NullList, this->env, primList->remaining, this->kont);
			} else {  // else we cannot complete the operation (currently all operations are only number based)
				context = new Variable("ERROR: argument must be a number.");
				env = new NullEnvironment();
				kont = new returnKontinuation();
			}
		} else {  // arguments are not list of constants
			context = new Variable("ERROR: arguments of primitive operation is either null or not a list.");
			env = new NullEnvironment();
			kont = new returnKontinuation();
		}
	}

	// ## CEK <Value, E, ar(closure(Abstraction, Ep), K)> --> <functBody, Ep[X -> Value], K>
	// ## CESK <Value, E, S ar(closure(Abstraction, Ep), K)> --> <functBody, Ep[X -> pointer], S[pointer -> Value], K>
	else if (argumentKontinuation *ar = dynamic_cast<argumentKontinuation*>(this->kont)) {  // if kontinuation is a function kontinuation
		if(dynamic_cast<Number*>(this->context) || dynamic_cast<Closure*>(this->context)){  // if a value
			if (Closure *clo = dynamic_cast<Closure*>(ar->value)) {  // if the value of the argument is a closure
				Pointer *point = new Pointer(new Variable(clo->lam->var));  // creating new pointer
				store = new ProperStore(point, this->context, this->store);  // store = S[pointer -> value]
				env = new ProperEnvironment(clo->lam->var, point, clo->env);  // environment = E[var -> pointer]
				context = clo->lam->body;
				kont = ar->kont;
			} else {
				context = new Variable("ERROR: value of argument is not a closure");
				env = new NullEnvironment();
				kont = new returnKontinuation();
			}
		}
	}

	// at returnKontinuation
	else if (dynamic_cast<returnKontinuation*>(this->kont)){
		return;
	}

	//############################################### NEW CESK STEPS ####################################
	// <pointer, E, S, K> --> <V, E, S, K>
	else if(dynamic_cast<Pointer*>(this->context)){  // if context is a pointer
		context = new Variable("POINTER");
		env = new NullEnvironment();
		kont = new returnKontinuation();
	}


	else if(dynamic_cast<Deref*>(this->context)){  // if context is a deref
		context = new Variable("DEREF");
		env = new NullEnvironment();
		kont = new returnKontinuation();
	}

	// <Set, E, S, K> --> <M, E, S, set()>
	else if(dynamic_cast<Set*>(this->context)){  // if context is a set expression
		context = new Variable("SET");
		env = new NullEnvironment();
		kont = new returnKontinuation();
	}

	//<V, E, S, set(pointer, K)> --> <V, E, S[pointer -> V], K>
	else if(dynamic_cast<setKontinuation*>(this->kont)){  // if kontinuation is set kontinuation
		context = new Variable("SETKON");
		env = new NullEnvironment();
		kont = new returnKontinuation();
	}

	else {
		context = new Variable("ERROR: Bad CESK arguments");
		env = new NullEnvironment();
		kont = new returnKontinuation();
	}
}

string CESK::print(){
	return "<" + this->context->toString()
	     + " , " + this->env->toString()
	     + " , " + this->store->toString()
	     + " , " + this->kont->toString() + ">";
}

bool CESK::done() {
	return ((dynamic_cast<Number*>(this->context) || dynamic_cast<Closure*>(this->context))  // checking if context is a closure or number
			&& dynamic_cast<returnKontinuation*>(this->kont));  // checking if at return continuation
}
