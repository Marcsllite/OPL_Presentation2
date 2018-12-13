#ifndef CEKInterface_H
#define CEKInterface_H

#include <string>

using std::string;

class LIST {  // arguments to primitives (Expression Lists)
public:
	virtual string toString() = 0;  // to string function for printing out
};

class EXPRESSION {  // C = any ISWIM expression
public:
	virtual string toString() = 0;  // to string function for printing out
};

class STORE {  // S = store that holds all pointer mappings
 public:
    virtual string toString() = 0;  // to string function for printing out
    virtual EXPRESSION *search(EXPRESSION *toFind) = 0;  // search/lookup function to find pointer value in store
};

class ENVIRONMENT {  // E = * | E[x -> v]
public:
	virtual string toString() = 0;  // to string function for printing out
	virtual EXPRESSION *search(string toFind) = 0;  // search/lookup function to find current variable in environment
};

class KONTINUATION {  // K = ret | fn(N, E, K) | ar(V, K) | pr(O, <V...>, <M...>, K)
public:
	virtual string toString() = 0;  // to string function for printing out
};
#endif // !CEKInterface_H
