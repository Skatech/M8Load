PROJECT = M8Load
MCU = atmega8
CLOCK = 4000000UL
SRCDIR = 
OUTDIR = 
CC = avr-gcc
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(CLOCK) -Wall -gdwarf-2 -std=gnu99 -Os
CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
LFLAGS = -mmcu=$(MCU)
#LFLAGS += -Wl,-u,vfprintf -Wl,-lm -Wl,-lscanf_flt -Wl,-lprintf_flt
HFLAGS = -R .eeprom -R .fuse -R .lock -R .signature
EFLAGS = -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 --no-change-warnings

TARGET = $(OUTDIR)$(PROJECT).elf
SOURCES := $(wildcard *.c)
OBJECTS := $(patsubst %,$(OUTDIR)%, $(SOURCES:.c=.o))
#OBJECTS := $(patsubst %.c,$(OUTDIR)/%.o, $(SOURCES))

all: setup build size

setup:
	$(shell mkdir $(OUTDIR)dep 2>NUL)

build: $(OBJECTS)
	$(CC) $(LFLAGS) -Wl,-Map=$(OUTDIR)$(PROJECT).map $(OBJECTS) -o $(OUTDIR)$(PROJECT).elf
	avr-objcopy -O ihex $(HFLAGS)  $(OUTDIR)$(PROJECT).elf $(OUTDIR)$(PROJECT).hex
	avr-objcopy $(EFLAGS) -O ihex $(OUTDIR)$(PROJECT).elf $(OUTDIR)$(PROJECT).eep || exit 0
	avr-objdump -h -S $(OUTDIR)$(PROJECT).elf > $(OUTDIR)$(PROJECT).lss

size:
	@avr-size -C --mcu=${MCU} ${TARGET}

$(OUTDIR)%.o: $(SRCDIR)%.c
	$(CC) $(CFLAGS) -MD -MP -MT $@ -MF dep/$(@F).d -c $< 

.PHONY: clean
clean:
	-rm -rf $(OBJECTS) $(OUTDIR)dep/ $(OUTDIR)$(PROJECT).elf $(OUTDIR)$(PROJECT).map $(OUTDIR)$(PROJECT).hex $(OUTDIR)$(PROJECT).eep $(OUTDIR)$(PROJECT).lss

trace:
	$(info SOURCES:$(SOURCES))
	$(info OBJECTS:$(OBJECTS))

upload:
	avrdude.exe -c usbtiny -p m8 -U flash:w:"$(OUTDIR)$(PROJECT).hex":i

fuse:
	avrdude.exe -c usbtiny -p m8 -U lfuse:w:0xef:m -U hfuse:w:0xd9:m

# -U lfuse:w:0x__:m -U hfuse:w:0x__:m
# Int.RC 8 MHZ: H:D9, L:E4
# Int.RC 4 MHZ: H:D9, L:E3
# Int.RC 4 MHZ: H:D9, L:E1
# Ext.QQ 8 MHz(Low Power): H:D9, L:EF
# Ext.QQ 8 MHz(Noisy Env): H:C9, L:EF
