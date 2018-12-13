#include "CEK.h"
#include <iostream>
#include <string>
#include <stdlib.h>
//#include <vld.h>
// memory leak detection
//
//#include <stdlib.h>
//#include <crtdbg.h>
//#define _CRTDBG_MAP_ALLOC



int main() {
	// Testing out CEK Machine
	// if context is a number
	//Number *num = new Number(21);
	CEK test1(new Number(21));  // <21, *, ret>

	// Regular example
	CEK test2(new Application(
				new Abstraction("x",
					new Application (
						new Abstraction("y", new Variable("x"))
					, new Number(8))
			    ),
			  new Number(7))
	);  // ((LambdaX. {(LambdaY. {X})8})7) where {} = body of an abstraction ; = 7

	//(from 9-2 in notes)
	CEK test40(new Application(
				new Application(
					new Abstraction("x",
						new Abstraction("y",
							new PrimitiveOp( "add",
								new ConstantList( new Number(8),
									new ConstantList(new Variable("y"), new NullList())
								)
							)
						)
				    ),
				    new Number(7)
				),
			  new Variable("x"))
	);
	// // if context is a variable
	CEK test3(new Variable("x")); // unbounded variable (ERROR: unbound variable)

	// if context is an application
	CEK test4(new Application(new Number(1), new Number(2)));  // (1 2) (ERROR: appliedTo cannot be a number)
	CEK test5(new Application(new Variable("x"), new Number(2)));  // (x 2) (ERROR: appliedTo cannot be a number)
	CEK test6(new Application(new Abstraction("x", new Variable("x")), new Number(2)));  // ((LambdaX. X) 2) (ERROR: appliedTo cannot be a number)
	CEK test7(new Application(
				new Application(
					new Abstraction("x", new Variable("x")), new Number(2)
				),
			  new Number(21)));  // (((LambdaX. X)2) 21) (ERROR: appliedTo cannot be a number)
	//CEK test8(new Application(new PrimitiveOp("add1", new ConstantList(new Number(20), new NullList())), new Number(2)));  // ((add1 20) 2) (ERROR: appliedTo cannot be a number)
	//CEK test9(new Application(new Closure(new Abstraction("x", new Variable("x")), ), new Number(2)));  // (closure(LambdaX. X, environment) 2)

	// if context is an abstraction
	CEK test10(new Abstraction("x", new Number(21)));  // LambdaX. 21
	CEK test11(new Abstraction("x", new Variable("y")));  // LambdaX. Y
	CEK test12(new Abstraction("x", new Abstraction("y", new Variable("y"))));  // LambdaX. (LambdaY. Y)
	CEK test13(new Abstraction("x", new Application(new Abstraction("y", new Variable("y")), new Number(13))));  // LambdaX. ((LambdaY. Y)13)
	// LambdaX. (mult 7 3)
	CEK test14(new Abstraction("x", new PrimitiveOp("mult", new ConstantList(new Number(7), new ConstantList( new Number(3), new NullList())))));
	// CEK test15(new Abstraction("x", new Closure(new Abstraction("x", new Variable("x")), )));

	// if context is a primitive operation
		//###### ADD1 ######//
		CEK test16(new PrimitiveOp("add1", new ConstantList(new Number(0), new NullList())));  // (add1 0) proper add1
		CEK test17(new PrimitiveOp("add1", new NullList()));  // (add1 ) ERROR: not enough arguments
		CEK test18(new PrimitiveOp("add1", new ConstantList(new Number(0),new ConstantList(new Number(1), new NullList()))));  // (add1 0, 1) ERROR: too many arguments
		CEK test19(new PrimitiveOp("add1", new ConstantList(new Variable("x"), new NullList())));  // (add1 x) ERROR: argument must be a number

		//###### ADD ######//
		CEK test20(new PrimitiveOp("add", new ConstantList(new Number(0), new ConstantList(new Number(1), new NullList()))));  // (add 0, 1) proper add
		CEK test21(new PrimitiveOp("add", new NullList()));  // (add) ERROR: not enough arguments
		CEK test22(new PrimitiveOp("add", new ConstantList(new Number(0), new NullList())));  // (add 0) ERROR: not enough arguments
		CEK test23(new PrimitiveOp("add", new ConstantList(new Variable("x"), new NullList())));  // (add x) ERROR: argument(s) must be a number
		CEK test24(new PrimitiveOp("add", new ConstantList(new Number(5), new ConstantList(new Variable("x"), new NullList()))));  // (add 5, x) ERROR: argument(s) must be a number

		//###### SUB ######//
		CEK test25(new PrimitiveOp("sub", new ConstantList(new Number(0), new ConstantList(new Number(1), new NullList()))));  // (sub 0, 1) proper sub
		CEK test26(new PrimitiveOp("sub", new NullList()));  // (sub) ERROR: not enough arguments
		CEK test27(new PrimitiveOp("sub", new ConstantList(new Number(0), new NullList())));  // (sub 0) ERROR: not enough arguments
		CEK test28(new PrimitiveOp("sub", new ConstantList(new Variable("x"), new NullList())));  // (sub x) ERROR: argument(s) must be a number
		CEK test29(new PrimitiveOp("sub", new ConstantList(new Number(5), new ConstantList(new Variable("x"), new NullList()))));  // (sub 5, x) ERROR: argument(s) must be a number

		//###### DIV ######//
		CEK test30(new PrimitiveOp("div", new ConstantList(new Number(0), new ConstantList(new Number(1), new NullList()))));  // (div 0, 1) proper div
		CEK test31(new PrimitiveOp("div", new NullList()));  // (div) ERROR: not enough arguments
		CEK test32(new PrimitiveOp("div", new ConstantList(new Number(0), new NullList())));  // (div 0) ERROR: not enough arguments
		CEK test33(new PrimitiveOp("div", new ConstantList(new Variable("x"), new NullList())));  // (div x) ERROR: argument(s) must be a number
		CEK test34(new PrimitiveOp("div", new ConstantList(new Number(5), new ConstantList(new Variable("x"), new NullList()))));  // (div 5, x) ERROR: argument(s) must be a number

		//###### MULT ######//
		CEK test35(new PrimitiveOp("mult", new ConstantList(new Number(0), new ConstantList(new Number(1), new NullList()))));  // (mult 0, 1) proper mult
		CEK test36(new PrimitiveOp("mult", new NullList()));  // (mult) ERROR: not enough arguments
		CEK test37(new PrimitiveOp("mult", new ConstantList(new Number(0), new NullList())));  // (mult 0) ERROR: not enough arguments
		CEK test38(new PrimitiveOp("mult", new ConstantList(new Variable("x"), new NullList())));  // (mult x) ERROR: argument(s) must be a number
		CEK test39(new PrimitiveOp("mult", new ConstantList(new Number(5), new ConstantList(new Variable("x"), new NullList()))));  // (mult 5, x) ERROR: argument(s) must be a number

	// if context is a closure

	//######################################//
	// evaluating all the tests and printing out final answers
	try {
		cout << "Test1 21: " << test1.eval()->toString() << endl;  // 21
		//cout << "Final CEK State Test1: " << test1.print() << endl;
		cout << "Test2 ((Lamx. ((Lamy. (x)) 8)) 7): " << test2.eval()->toString() << endl;  // 7
		//cout << "Final CEK State Test2: " << test2.print() << endl;
		//cout << "Test3 x: " << test3.eval()->toString() << endl;  // unbound variable x
		//cout << "Test4 (1 2): " << test4.eval()->toString() << endl;  // ERROR: first part of application can't be number
		//cout << "Test5 (x 2): " << test5.eval()->toString() << endl;  // unbound variable X
		cout << "Test6 ((Lambdax.x) 2): " << test6.eval()->toString() << endl;
		cout << "Test7 (((Lambdax.x) 2) 21): " << test7.eval()->toString() << endl;  // Error first part of application can't be number
		//###cout << "Test8 ((add1 20) 2): " << test8.eval()->toString() << endl;
		//###cout << "Test9: " << test9.eval()->toString() << endl;
		//cout << "Test10 (Lamx. 21): " << test10.eval()->toString() << endl;  // clo((Lambdax. 21), *)
		//cout << "Test11 (Lamx.y): " << test11.eval()->toString() << endl;
		//cout << "Test12 (Lamx. (Lamy.y)): " << test12.eval()->toString() << endl;
		//cout << "Test13 (Lamx. ((Lamy.y) 13)): " << test13.eval()->toString() << endl;
		//cout << "Test14 (Lamx. (mult 7 3)): " << test14.eval()->toString() << endl;
		// //####cout << "Test15: " << test15.eval()->toString() << endl;
		// cout << "Test16 (add1 0): " << test16.eval()->toString() << endl;
		// cout << "Test17 (add1): " << test17.eval()->toString() << endl;
		// cout << "Test18 (add1 0 1): " << test18.eval()->toString() << endl;
		// cout << "Test19 (add1 x): " << test19.eval()->toString() << endl;
		// cout << "Test20 (add 0 1): " << test20.eval()->toString() << endl;
		// cout << "Test21 (add): " << test21.eval()->toString() << endl;
		// cout << "Test22 (add 0): " << test22.eval()->toString() << endl;
		// cout << "Test23 (add x): " << test23.eval()->toString() << endl;
		// cout << "Test24 (add 5 x): " << test24.eval()->toString() << endl;
		// cout << "Test25 (sub 0 1): " << test25.eval()->toString() << endl;
		// cout << "Test26 (sub): " << test26.eval()->toString() << endl;
		// cout << "Test27 (sub 0): " << test27.eval()->toString() << endl;
		// cout << "Test28 (sub x): " << test28.eval()->toString() << endl;
		// cout << "Test29 (sub 5 x): " << test29.eval()->toString() << endl;
		// cout << "Test30 (div 0 1): " << test30.eval()->toString() << endl;
		// cout << "Test31 (div): " << test31.eval()->toString() << endl;
		// cout << "Test32 (div 0): " << test32.eval()->toString() << endl;
		// cout << "Test33 (div x): " << test33.eval()->toString() << endl;
		// cout << "Test34 (div 5 x): " << test34.eval()->toString() << endl;
		// cout << "Test35 (mult 0 1): " << test35.eval()->toString() << endl;
		// cout << "Test36 (mult): " << test36.eval()->toString() << endl;
		// cout << "Test37 (mult 0): " << test37.eval()->toString() << endl;
		// cout << "Test38 (mult x): " << test38.eval()->toString() << endl;
		// cout << "Test39 (mult 5 x): " << test39.eval()->toString() << endl;
		// cout << "Test 40 (((LambdaX. (LambdaY. 8 + Y)) 7)X): \n" << test40.eval()->toString() << endl;
	} catch (logic_error e){
		cout << e.what() << endl;
	}
	exit(0);
}
