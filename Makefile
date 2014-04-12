PROJECT=djx16

DEVICE_CC = atmega162
DEVICE_DUDE = m162
PROGRAMMER_DUDE = -Pusb -c dragon_isp

F_CPU=8000000  # internal calibrated R/C oscillator

CROSS?=avr-
AVRDUDE=avrdude
OBJCOPY=$(CROSS)objcopy
OBJDUMP=$(CROSS)objdump
CC=$(CROSS)gcc
LD=$(CROSS)gcc
SIZE=$(CROSS)size

LDFLAGS=-Wall -g -mmcu=$(DEVICE_CC)
CPPFLAGS=-I. -I$(VUSB) -DF_CPU=$(F_CPU)
CFLAGS=-mmcu=$(DEVICE_CC) -Os -Wall -Wextra -g
ASFLAGS=$(CFLAGS)

OBJS = djx16.o djx16_hw.o djx16_led.o djx16_key.o

VPATH = $(VUSB)

all : $(PROJECT).hex $(PROJECT).lst
$(PROJECT).bin : $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

%.hex : %.bin
	$(SIZE) --mcu=$(DEVICE_CC) -C $^
	$(OBJCOPY) -j .text -j .data -O ihex $^ $@ || (rm -f $@ ; false )

%.lst : %.bin
	$(OBJDUMP) -S $^ >$@ || (rm -f $@ ; false )

ifneq ($(MAKECMDGOALS),clean)
include $(OBJS:.o=.d)
endif

%.d : %.c
	$(CC) $(CPPFLAGS) -o $@ -MM $^

%.d : %.S
	$(CC) $(CPPFLAGS) -o $@ -MM $^

.PHONY : clean burn
burn : $(PROJECT).hex
	$(AVRDUDE) $(PROGRAMMER_DUDE) -p $(DEVICE_DUDE) -U flash:w:$^
clean :
	rm -f *.bak *~ *.bin *.hex *.lst *.o *.d
