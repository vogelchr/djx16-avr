PROJECT=djx16

DEVICE_CC = atmega162
DEVICE_DUDE = m162
PROGRAMMER_DUDE = -Pusb -c dragon_isp

F_CPU=8000000  # internal calibrated R/C oscillator

CROSS?=avr-
AVRDUDE=avrdude
ifeq ($(V),1)
	SILENT=
	ECHO=@true
else
	SILENT=@
	ECHO=@echo
endif
OBJCOPY=$(SILENT)$(CROSS)objcopy
OBJDUMP=$(SILENT)$(CROSS)objdump
CC=$(SILENT)$(CROSS)gcc
LD=$(SILENT)$(CROSS)gcc
SIZE=$(SILENT)$(CROSS)size

LDFLAGS=-Wall -g -mmcu=$(DEVICE_CC)
CPPFLAGS=-I. -DF_CPU=$(F_CPU)
CFLAGS=-mmcu=$(DEVICE_CC) -Os -Wall -Wextra -g -fshort-enums
ASFLAGS=$(CFLAGS)

OBJS = djx16.o djx16_hw.o djx16_led.o djx16_key.o djx16_adc.o \
	djx16_output.o djx16_ui.o

all : $(PROJECT).hex $(PROJECT).lst

$(PROJECT).bin : $(OBJS)

%.o : %.c
	$(ECHO) "creating $@"
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $<

%.bin : %.o
	$(ECHO) "creating $@"
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

%.hex : %.bin
	$(ECHO) "creating $@"
	$(SIZE) --mcu=$(DEVICE_CC) -C $^
	$(OBJCOPY) -j .text -j .data -O ihex $^ $@ || (rm -f $@ ; false )

%.raw : %.bin
	$(ECHO) "creating $@"
	$(SIZE) --mcu=$(DEVICE_CC) -C $^
	$(OBJCOPY) -j .text -j .data -O binary $^ $@ || (rm -f $@ ; false )

%.lst : %.bin
	$(ECHO) "creating $@"
	$(OBJDUMP) -S $^ >$@ || (rm -f $@ ; false )

ifneq ($(MAKECMDGOALS),clean)
include $(OBJS:.o=.d)
endif

%.d : %.c
	$(ECHO) "creating $@"
	$(CC) $(CPPFLAGS) -o $@ -MM $^

%.d : %.S
	$(ECHO) "creating $@"
	$(CC) $(CPPFLAGS) -o $@ -MM $^

.PHONY : clean burn
burn : $(PROJECT).hex
	$(AVRDUDE) $(PROGRAMMER_DUDE) -p $(DEVICE_DUDE) -U flash:w:$^
clean :
	$(ECHO) "creating $@"
	@rm -f *.bak *~ *.bin *.hex *.lst *.o *.d *.raw
