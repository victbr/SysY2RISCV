all: lex yacc toE

toT: 
	cd stage2
	make

toE: lex.yy.o toE.tab.o recycle.o translation.o funcdef.o exp.o constdecl.o block.o library.o
	g++ -Wno-register -O2 -lm -std=c++17 toE.tab.o lex.yy.o recycle.o translation.o exp.o \
	funcdef.o constdecl.o block.o library.o stage2/stage2.o -o compiler -Idirs

lex: toE.l utils.hpp
	flex -o lex.yy.cpp toE.l

yacc: toE.y utils.hpp
	bison -d -o toE.tab.cpp toE.y

lex.yy.o: lex.yy.cpp utils.hpp toE.tab.hpp
	g++ -Wno-register -O2 -lm -std=c++17 lex.yy.cpp -c -Idirs

toE.tab.o: toE.tab.cpp utils.hpp
	g++ -Wno-register -O2 -lm -std=c++17 toE.tab.cpp -c -Idirs

recycle.o: recycle.cpp utils.hpp
	g++ -Wno-register -O2 -lm -std=c++17 recycle.cpp -c -Idirs

translation.o: translation.cpp utils.hpp
	g++ -Wno-register -O2 -lm -std=c++17 translation.cpp -c -Idirs

funcdef.o: funcdef.cpp utils.hpp
	g++ -Wno-register -O2 -lm -std=c++17 funcdef.cpp -c -Idirs

exp.o: exp.cpp utils.hpp
	g++ -Wno-register -O2 -lm -std=c++17 exp.cpp -c -Idirs

constdecl.o: constdecl.cpp utils.hpp
	g++ -Wno-register -O2 -lm -std=c++17 constdecl.cpp -c -Idirs

block.o: block.cpp utils.hpp
	g++ -Wno-register -O2 -lm -std=c++17 block.cpp -c -Idirs

library.o: library.cpp utils.hpp
	g++ -Wno-register -O2 -lm -std=c++17 library.cpp -c -Idirs

clean:
	-rm *.o toE.tab.cpp *.h *.c toE.tab.hpp lex.yy.cpp
	find . -type f -executable -delete
