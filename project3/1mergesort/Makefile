# All: mts.o
# 	gcc -pthread -o  mts mts.o 
# mts.o: mts.c
# 	gcc -c mts.c 
# clean:
# 	rm *.o
# 	rm mts

obj-m += mergesort.o  
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
