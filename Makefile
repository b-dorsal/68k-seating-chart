# path to RETRO68
RETRO68=../../../Retro68-build/toolchain

PREFIX=$(RETRO68)/m68k-apple-macos
CC=$(RETRO68)/bin/m68k-apple-macos-gcc
CXX=$(RETRO68)/bin/m68k-apple-macos-g++
REZ=$(RETRO68)/bin/Rez

LDFLAGS=-lRetroConsole
RINCLUDES=$(PREFIX)/RIncludes
REZFLAGS=-I$(RINCLUDES)

SeatingChart.bin SeatingChart.APPL SeatingChart.dsk: SeatingChart.code.bin
	$(REZ) $(REZFLAGS) \
		--copy "SeatingChart.code.bin" \
		"$(RINCLUDES)/Retro68APPL.r" \
		-t "APPL" -c "????" \
		-o SeatingChart.bin --cc SeatingChart.APPL --cc SeatingChart.dsk


SeatingChart.code.bin: main.o qr_data.h
	$(CXX) $< -o $@ $(LDFLAGS)	# C++ used for linking because RetroConsole needs it

.PHONY: clean
clean:
	rm -f SeatingChart.bin SeatingChart.APPL SeatingChart.dsk SeatingChart.code.bin SeatingChart.code.bin.gdb main.o
