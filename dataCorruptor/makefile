dx: ./obj/dataCorruptor.o ./obj/logger.o ./obj/sharedMemory.o ./obj/messageQueue.o
	cc ./obj/dataCorruptor.o ./obj/logger.o ./obj/sharedMemory.o ./obj/messageQueue.o -o ./bin/dx

./obj/dataCorruptor.o: ./inc/dataCorruptor.h ./src/dataCorruptor.c
	cc -Wall -Iinc -c ./src/dataCorruptor.c -o ./obj/dataCorruptor.o

./obj/logger.o: ./inc/logger.h ./src/logger.c
	cc -Wall -Iinc -c ./src/logger.c -o ./obj/logger.o

./obj/sharedMemory.o: ./inc/sharedMemory.h ./src/sharedMemory.c
	cc -Wall -Iinc -c ./src/sharedMemory.c -o ./obj/sharedMemory.o

./obj/messageQueue.o: ./inc/messageQueue.h ./src/messageQueue.c
	cc -Wall -Iinc -c ./src/messageQueue.c -o ./obj/messageQueue.o

clean:
	rm -f ./bin/dx
	rm -f ./obj/dataCorruptor.o
	rm -f ./obj/logger.o
	rm -f ./obj/sharedMemory.o
	rm -f ./obj/messageQueue.o

rebuild: clean
	make
