all: lex.yy2.o toT.tab2.o recycle2.o translation2.o funcdef2.o exp2.o register2.o

lex.yy2.o: lex.yy2.cpp utils2.hpp toT.tab2.hpp
	g++ -Wno-register -O2 -lm -std=c++17 lex.yy2.cpp -c -Idirs

toT.tab2.o: toT.tab2.cpp utils2.hpp
	g++ -Wno-register -O2 -lm -std=c++17 toT.tab2.cpp -c -Idirs

recycle2.o:
	g++ -Wno-register -O2 -lm -std=c++17 recycle2.cpp -c -Idirs

translation2.o:
	g++ -Wno-register -O2 -lm -std=c++17 translation2.cpp -c -Idirs

funcdef2.o:
	g++ -Wno-register -O2 -lm -std=c++17 funcdef2.cpp -c -Idirs

exp2.o:
	g++ -Wno-register -O2 -lm -std=c++17 exp2.cpp -c -Idirs

register2.o:
	g++ -Wno-register -O2 -lm -std=c++17 register2.cpp -c -Idirs

clean:
	-rm *.o *.c *.h
	find . -type f -executable -delete
