dx: ./obj/dataCorruptor.o ./obj/logger.o ./obj/shared_memory.o ./obj/message_queue.o
	cc ./obj/dataCorruptor.o ./obj/logger.o ./obj/shared_memory.o ./obj/message_queue.o -o ./bin/dx

./obj/dataCorruptor.o: ./src/dataCorruptor.c
	cc -Wall -Iinc -c ./src/dataCorruptor.c -o ./obj/dataCorruptor.o

./obj/logger.o: ./inc/logger.h ./src/logger.c
	cc -Wall -Iinc -c ./src/logger.c -o ./obj/logger.o

./obj/shared_memory.o: ./inc/shared_memory.h ./src/shared_memory.c
	cc -Wall -Iinc -c ./src/shared_memory.c -o ./obj/shared_memory.o

./obj/message_queue.o: ./inc/message_queue.h ./src/message_queue.c
	cc -Wall -Iinc -c ./src/message_queue.c -o ./obj/message_queue.o

clean:
	rm -f dx
	rm -f ./obj/dataCorruptor.o
	rm -f ./obj/logger.o
	rm -f ./obj/shared_memory.o
	rm -f ./obj/message_queue.o

rebuild: clean
	make
