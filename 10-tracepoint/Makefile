PWD    := $(shell pwd)
KDIR   := /lib/modules/$(shell uname -r)/build

KBUILD_CFLAGS += -g

obj-m	:= sillymod-event.o

CFLAGS_sillymod-event.o = -I$(src)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
