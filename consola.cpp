#include <stdlib.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <esin/gen_driver>
#include "math_sessio.hpp"

int main()
{
  // When the tab key is hit a tab is inserted rather to auto-complete files.
  //rl_bind_key('\t', rl_complete);
  rl_bind_key('\t', rl_insert);

  // Enable history
  using_history();

  // Load file with error messages
  gen_driver d("math.err", false);
  
  // Create a ESIN math session
  math_sessio S;

	std::cout << "Welcome to ESIN Math. Type help or ? to list commands.\n";

  while (true) {
    // Display prompt and read input
    char* input = readline("> ");

    // Add input to readline history.
    add_history(input);

    // Check for EOF.
    if (not input) {
    	std::cout << "Thank you for using ESIN math.\n";
      break;
    }

    if (string(input) == "?" or string(input) == "help") {
    	std::cout << "ESIN Math provides these commands:\n";
    	std::cout << "  E: Evaluates the expression E\n";
      std::cout << "  v :=  E. Evaluates expression E and assigns the result to variable v\n";
      std::cout << "  unassign v: Unassigns the variable v\n";
      std::cout << "  list: List of the current variables with their expressions\n";
      std::cout << "  history: History of the previous commands. Can be retrieved with the arrow keys\n";
      std::cout << "  byebye: End of session\n";
      std::cout << "  help or ?: This help\n\n";
      std::cout << "The expression E can contain:\n";
      std::cout << "  Constants: integer, rational, real or e\n";
      std::cout << "  Operators: + - * / ^ -(sign change) +(positive sign) sqrt() exp() log() evalf()\n";
      std::cout << "  Variables: Names containing lowercase or uppercase letters or the _ char\n";
      std::cout << "  %: Special variable refering last expression\n";
      std::cout << "  Parenthesis: ( )\n";
      continue;
    }

    if (string(input) == "history") {
    	HIST_ENTRY** hl = history_list();
    	for(int i = 0; i < history_length; i++) {
    		std::cout << (**hl).line << std::endl;
    		hl++;
    	}
    	continue;
    }

    if (string(input) == "list") {
    	list<string> l;
    	S.dump(l);
    	l.sort();
    	for(list<string>::const_iterator it = l.begin(); it != l.end(); it++) {
    		std::cout << *it << std::endl;
    	}
    	continue;
    }

    list<token> Lin, Lout;
    try {
    	math_io::scan(input, Lin);
    	S.execute(Lin, Lout);
    	std::cout << math_io::tostring(Lout) << std::endl;
    } catch(error e) {
    	std::cout << "Error " << e.codigo() << ": " << e.mensaje() << std::endl;
    	continue;
    }

    // Check for byebye.
   	if (Lin.size() == 1 and Lin.front() == token(token::BYEBYE)) {
    	std::cout << "Thank you for using ESIN math.\n";
      break;
    }

    // Free buffer that was allocated by readline
    free(input);
  }
  return 0;
}