dx: ./obj/dataCorruptor.o ./obj/logger.o
	cc ./obj/dataCorruptor.o ./obj/logger.o -o ./bin/dx

./obj/dataCorruptor.o: ./src/dataCorruptor.c
	cc -Wall -Iinc -c ./src/dataCorruptor.c -o ./obj/dataCorruptor.o

./obj/logger.o: ./inc/logger.h ./src/logger.c
	cc -Wall -Iinc -c ./src/logger.c -o ./obj/logger.o

clean:
	rm -f dx
	rm -f ./obj/dataCorruptor.o
	rm -f ./obj/logger.o

rebuild: clean
	make