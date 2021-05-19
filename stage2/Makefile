all: stage2.o

stage2.o:
	g++ -Wno-register -O2 -lm -std=c++17 recycle2.cpp translation2.cpp \
funcdef2.cpp exp2.cpp register2.cpp -c -Idirs


clean:
	-rm *.o *.c *.h
	find . -type f -executable -delete
