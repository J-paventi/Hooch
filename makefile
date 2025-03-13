all: dataCreator dataReader dataCorruptor

.PHONY: all dataCreator dataReader dataCorruptor clean

dataCreator:
	$(MAKE) -C dataCreator

dataReader:
	$(MAKE) -C dataReader

dataCorruptor:
	$(MAKE) -C dataCorruptor

clean:
	$(MAKE) -C dataCreator clean
	$(MAKE) -C dataReader clean
	$(MAKE) -C dataCorruptor clean

