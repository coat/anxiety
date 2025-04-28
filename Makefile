ASM = ez80asm -l -b 00

NAME = anxiety
BINARY = $(NAME).bin
DEPS = mos_api.inc times12.uf2

SD_PATH = ~/.local/share/fab-agon-emulator/sdcard/bin

all: $(BINARY)

$(BINARY): $(NAME).asm $(DEPS)
	$(ASM) $< 

clean:
	rm -f $(BINARY) *.lst

install: $(BINARY)
	cp $(BINARY) $(SD_PATH)
