# esinmath-expression-calculator
[UPC PROJECT] A small symbolic computing program that calculates and avaluates complex mathematical expressions using an adapted binary tree and the Shunting Yard Algorithm.

1. Once the expression is read, esinmath-expression-calculator decomposes the expression into a sequence
of tokens; each token is a literal value (an integer, rational, or comma constant
float), an identifier (of variable or function symbol), an operator (+, *, . . . ),
an opening or closing parenthesis or a comma.

2. An expression tree is then built, at the same time checking the syntactic correctness of the read expression. For example, if the sequence of characters entered by the user were:

<p align="center">
x+3*sqrt((yy*8)-7.3E-1)
</p>

this would be decomposed into the next sequence of tokens (after the phase
which is called lexical analysis):

x | + | 3 | * | sqrt | ( | ( | yy | * | 8 | ) | - | 7.3E-1 | )
--- | --- | --- | --- |--- |--- |--- |--- |--- |--- |--- |--- |--- |---

The expression will be represented by the following expression tree:

<p align="center">
  <img src="https://github.com/artHub-j/esinmath-expression-calculator/assets/92806890/9314287d-4fac-4e84-af3c-02c97946d640"/>
</p>

<p align="center">
https://en.wikipedia.org/wiki/Shunting_yard_algorithm
</p>

As mentioned, the algorithm used to build the expression tree is very similar to the one used to convert an expression in (the usual) infix notation to postfix or Polish notation. It is the so-called Shunting Yard Algorithm due to its similarity to the railway operation of moving freight train wagons on a beach of tracks

<p align="center">
  <img src="https://github.com/artHub-j/esinmath-expression-calculator/assets/92806890/dbebb620-8af7-4661-aab9-f02293860ba3"/>
</p>

It is implemented using a stack to hold operators and opening parentheses and an expression stack to hold reconstructed expression fragments. During the reading, the priority and associativity of the operators is taken into account.
