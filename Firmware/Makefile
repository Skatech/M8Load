PROJECT = M8Load
MCU = atmega8
CLOCK = 4000000UL
SRCDIR = 
OUTDIR = ../.out/
CC = avr-gcc
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(CLOCK) -Wall -gdwarf-2 -std=gnu99 -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
DFLAGS = -MMD -MP -MF $(OUTDIR)obj/dep/$(@F).d
LFLAGS = -mmcu=$(MCU)
#LFLAGS += -Wl,-u,vfprintf -Wl,-lm -Wl,-lscanf_flt -Wl,-lprintf_flt
HFLAGS = -R .eeprom -R .fuse -R .lock -R .signature
EFLAGS = -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 --no-change-warnings

TARGET = $(OUTDIR)$(PROJECT).elf
SOURCES := $(wildcard *.c)
OBJECTS := $(patsubst %,$(OUTDIR)obj/%, $(SOURCES:.c=.o))

all: setup build size

setup:
	$(shell mkdir $(subst /,\,$(OUTDIR)obj\dep) 2>NUL)

build: $(OBJECTS)
	$(CC) $(LFLAGS) -Wl,-Map=$(OUTDIR)$(PROJECT).map $(OBJECTS) -o $(OUTDIR)$(PROJECT).elf
	avr-objcopy -O ihex $(HFLAGS) $(OUTDIR)$(PROJECT).elf $(OUTDIR)$(PROJECT).hex
	avr-objcopy $(EFLAGS) -O ihex $(OUTDIR)$(PROJECT).elf $(OUTDIR)$(PROJECT).eep || exit 0
	avr-objdump -h -S $(OUTDIR)$(PROJECT).elf > $(OUTDIR)$(PROJECT).lss

size:
	@avr-size -C --mcu=${MCU} ${TARGET}

$(OUTDIR)obj/%.o: $(SRCDIR)%.c
	$(CC) $(CFLAGS) $(DFLAGS) -o $@ -c $< 

.PHONY: clean
clean:
	-rm -rf $(OUTDIR)

trace:
	$(info SOURCES:$(SOURCES))
	$(info OBJECTS:$(OBJECTS))

upload:
	avrdude -c usbtiny -p m8 -U flash:w:"$(OUTDIR)$(PROJECT).hex":i

fuse:
	avrdude -c usbtiny -p m8 -U lfuse:w:0xE3:m -U hfuse:w:0xD9:m
	
# -U lfuse:w:0x__:m -U hfuse:w:0x__:m
# Int.RC 8 MHZ: H:D9, L:E4
# Int.RC 4 MHZ: H:D9, L:E3
# Int.RC 4 MHZ: H:D9, L:E1
# Ext.QQ 8 MHz(Low Power): H:D9, L:EF
# Ext.QQ 8 MHz(Noisy Env): H:C9, L:EF
