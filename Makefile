obj-m += process_info.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	
clean:
	make -C /lib/mobules/$(shell uname -r)/build M=$(PWD) clean
