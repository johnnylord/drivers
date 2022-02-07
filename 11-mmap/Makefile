obj-m += mmap.o


all: user
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm user

user: user.c
	gcc -o $@ $<
