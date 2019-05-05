# Rules for building .o
cs2123p6Driver.o: cs2123p6Driver.c cs2123p6.h
	gcc -g -c cs2123p6Driver.c -o cs2123p6Driver.o

p6FlightData.o: p6FlightData.c cs2123p6.h
	gcc -g -c p6FlightData.c -o p6FlightData.o

p6graph.o: p6graph.c cs2123p6.h
	gcc -g -c p6graph.c -o p6graph.o

p6PathFunctions.o: p6PathFunctions.c cs2123p6.h
	gcc -g -c p6PathFunctions.c -o p6PathFunctions.o

p6PrintFunctions.o: p6PrintFunctions.c cs2123p6.h
	gcc -g -c p6PrintFunctions.c -o p6PrintFunctions.o

# Rules for building executable
p6: p6FlightData.o p6graph.o p6PathFunctions.o p6PrintFunctions.o
	gcc -g -o p6 p6FlightData.o p6graph.o p6PathFunctions.o p6PrintFunctions.o

clean:
	rm -f cs2123p6Driver.o p6FlightData.o p6graph.o p6PathFunctions.o p6PrintFunctions.o
