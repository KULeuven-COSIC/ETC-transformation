CC ?= clang
CFLAGS ?= -I. -Ibuild -Os -march=native -Wall -Wextra -Wpedantic -DNDEBUG -fomit-frame-pointer -std=c11 -I/home/jipi/work/code/XKCP/bin/generic64/ $(XCFLAGS)
LDFLAGS += $(XLDFLAGS)

BEARS ?= PapaBear MamaBear BabyBear

BEARS_O = $(addprefix build/,$(addsuffix /threebears.o,$(BEARS)))
BEARS_R = $(addprefix build/,$(addsuffix /ring.o,$(BEARS)))
BEARS_H = $(addprefix build/,$(addsuffix /threebears.h,$(BEARS)))

.PHONY: all clean bench test
.PRECIOUS: build/%.c build/%.h build/%/ring.h build/%/params.h

all: build/threebears build/test_fec
    
build/threebears: $(BEARS_O) $(BEARS_R) build/test_threebears.o build/melas_fec.o build/shake.o
	$(CC) $(LDFLAGS) -o $@ $^ -L/home/jipi/work/code/XKCP/bin/generic64 -lkeccak 
    
build/test_ct: $(BEARS_O) $(BEARS_R) build/test_ct.o build/melas_fec.o build/shake.o
	$(CC) $(LDFLAGS) -o $@ $^ -L/home/jipi/work/code/XKCP/bin/generic64 -lkeccak

size: $(BEARS_O) $(BEARS_R) build/melas_fec.o build/shake.o
	size $^

bench: build/threebears
	$< -q
    
build/test_fec: build/test_fec.o build/melas_fec.o
	$(CC) $(LDFLAGS) -o $@ $^

build/timestamp:
	mkdir -p build
	touch $@

build/%.o: %.c *.h build/timestamp
	$(CC) $(CFLAGS) -c -o $@ $<

build/%.o: test/%.c *.h build/timestamp
	$(CC) $(CFLAGS) -c -o $@ $<

build/test_threebears.o: build/test_threebears.c $(BEARS_H) *.h build/timestamp
	$(CC) $(CFLAGS) -c -o $@ $<

build/test_ct.o: build/test_ct.c $(BEARS_H) *.h build/timestamp
	$(CC) $(CFLAGS) -c -o $@ $<
    
build/%.c: test/%.tmpl.c *.c *.h template*.py build/timestamp
	python -B template.py $(TMPLFLAGS) --systems="$(BEARS)" -o $@ $<

build/%.h: %.tmpl.h template*.py build/timestamp
	python -B template.py $(TMPLFLAGS) -o $@ $<
    
build/%/params.h: params.tmpl.h template*.py build/timestamp
	mkdir -p `dirname $@`
	python -B template.py --systems=$$(basename $$(dirname $@)) -o $@ $<
    
build/%/ring.h: ring.tmpl.h template*.py build/timestamp
	mkdir -p `dirname $@`
	python -B template.py --systems=$$(basename $$(dirname $@)) -o $@ $<
    
build/%/threebears.h: threebears.tmpl.h template*.py build/timestamp
	mkdir -p `dirname $@`
	python -B template.py --systems=$$(basename $$(dirname $@)) -o $@ $<
	
build/%/threebears.o: threebears.c build/%/params.h build/%/ring.h build/%/threebears.h build/timestamp
	$(CC) $(CFLAGS) -I `dirname $@` -c -o $@ $<
	
build/%/ring.o: ring.c build/%/params.h build/%/ring.h build/%/threebears.h build/timestamp
	$(CC) $(CFLAGS) -I `dirname $@` -c -o $@ $<

build/trace: test/trace.hs build/timestamp
	ghc -O2 --make -hidir build -odir build -o $@ $<

# Build with -mno-adx because valgrind doesn't understand adx
build/mem.trace: build/trace build/threebears
	valgrind --tool=lackey --trace-superblocks=yes --trace-mem=yes build/threebears -n 1 2>&1 \
		| build/trace `nm build/threebears | grep Bear | grep " T "` \
		> $@

test: build/test_fec
	build/test_fec

clean:
	rm -fr build/
