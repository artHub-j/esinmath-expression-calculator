
all: racional.o variables.o math_io.o token.o math_sessio.o expressio.o driver_esinmath.o consola.o 
	g++ -o Driver.exe racional.o variables.o math_io.o token.o math_sessio.o expressio.o driver_esinmath.o -Wno-deprecated -lesin
	g++ -o Consola.exe racional.o variables.o math_io.o token.o math_sessio.o expressio.o consola.o -Wno-deprecated -lesin -lreadline

Driver.exe: racional.o variables.o math_io.o token.o math_sessio.o expressio.o driver_esinmath.o
	g++ -o Driver.exe racional.o variables.o math_io.o token.o math_sessio.o expressio.o driver_esinmath.o -Wno-deprecated -lesin

Consola.exe: racional.o variables.o math_io.o token.o math_sessio.o expressio.o consola.o 
	g++ -o Consola.exe racional.o variables.o math_io.o token.o math_sessio.o expressio.o consola.o -Wno-deprecated -lesin -lreadline

racional.o: racional.cpp 
	g++ -c -g -O0 -Wall -Wno-deprecated -std=c++14 racional.cpp
	
variables.o: variables.cpp
	g++ -c -g -O0 -Wall -Wno-deprecated -std=c++14 variables.cpp
	
token.o: token.cpp 
	g++ -c -g -O0 -Wall -Wno-deprecated -std=c++14 token.cpp
	
math_io.o: math_io.cpp 
	g++ -c -g -O0 -Wall -Wno-deprecated -std=c++14 math_io.cpp
	
math_sessio.o: math_sessio.cpp 
	g++ -c -g -O0 -Wall -Wno-deprecated -std=c++14 math_sessio.cpp
	
expressio.o: expressio.cpp
	g++ -c -g -O0 -Wall -Wno-deprecated -std=c++14 expressio.cpp
	
driver_esinmath.o: driver_esinmath.cpp 
	g++ -c -g -O0 -Wall -Wno-deprecated -std=c++14 driver_esinmath.cpp

consola.o: consola.cpp
	g++ -c -g -O0 -Wall -Wno-deprecated -std=c++14 consola.cpp 

clean:
	rm *.o rm *.exe

#test racional:
# ./Driver.exe < jp_public_racional.in > jp1.res
# ./Driver.exe < jp_public_token.in > jp2.res
# ./Driver.exe < jp_public_expressio.in > jp3.res
# ./Driver.exe < jp_public_variables.in > jp4.res

# diff jp1.res jp_public_racional.res
# diff jp2.res jp_public_token.res
# diff jp3.res jp_public_expressio.res
# diff jp4.res jp_public_variables.res
