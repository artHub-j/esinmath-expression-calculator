# esinmath-expression-calculator
A small symbolic computing program that calculates and avaluates complex mathematical expressions using an adapted binary tree and the Shunting Yard Algorithm.

As mentioned, the algorithm used to build the expression tree is very similar to the one used to convert an expression in (the usual) infix notation to postfix or Polish notation. It is the so-called Shunting Yard Algorithm due to its similarity to the railway operation of moving freight train wagons on a beach of tracks

![](https://upload.wikimedia.org/wikipedia/commons/2/24/Shunting_yard.svg)

It is implemented using a stack to hold operators and opening parentheses and an expression stack to hold reconstructed expression fragments. During the reading, the priority and associativity of the operators is taken into account.
