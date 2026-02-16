CFLAGS=-g -O2 -Wall -Wextra -I/usr/local/include -Isrc -rdynamic -pthread $(OPTFLAGS)
LDLIBS=-llcthw $(OPTLIBS) -lm -lssl -lcrypto
LDFLAGS=-L/usr/local/lib -Wl,-rpath,/usr/local/lib -pthread
PREFIX?=/usr/local

SOURCES=$(wildcard src/**/*.c src/*.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

TEST_SRC=$(wildcard tests/*_tests.c)
TESTS=$(patsubst %.c,%,$(TEST_SRC))

PROGS_SRC=$(wildcard bin/*.c)
PROGS=$(patsubst %.c,%,$(PROGS_SRC))

TARGET=build/libstatserve.a
SO_TARGET=$(patsubst %.a,%.so,$(TARGET))

# The Target Build
all: $(TARGET) $(SO_TARGET) tests $(PROGS) bin/statserve

dev: CFLAGS=-g -Wall -Isrc -Wall -Wextra $(OPTFLAGS)
dev: all

bin/statserve: $(TARGET)

$(TARGET): CFLAGS += -fPIC
$(TARGET): build $(OBJECTS)
	ctags -R
	ar rcs $@ $(OBJECTS)
	ranlib $@

$(SO_TARGET): $(TARGET) $(OBJECTS)
	$(CC) -shared -o $@ $(OBJECTS) $(LDLIBS)

build:
	@mkdir -p build
	@mkdir -p bin

$(TESTS): %: %.c $(TARGET)
	$(CC) $(CFLAGS) $< $(TARGET) $(LDFLAGS) $(LDLIBS) -o $@

$(PROGS): %: %.c $(TARGET)
	$(CC) $(CFLAGS) $< $(TARGET) $(LDFLAGS) $(LDLIBS) -o $@

.PHONY: tests progs
tests: $(TESTS)
	sh ./tests/runtests.sh

progs: $(PROGS)

clean:
	rm -rf build $(OBJECTS) $(TESTS) $(PROGS)
	rm -f tests/tests.log
	find . -name "*.gc*" -exec rm {} \;
	rm -rf `find . -name "*.dSYM" -print`

install: all
	install -d $(DESTDIR)/$(PREFIX)/lib/
	install $(TARGET) $(SO_TARGET) $(DESTDIR)/$(PREFIX)/lib/

check:
	@echo "Files with potentially dangerous functions:"
	@egrep '[^_.>a-zA-Z0-9](str(n?cpy|n?cat|xfrm|n?dup|str|pbrk|tok|_)|stpn?cpy|a?sn?printf|byte_)' $(SOURCES) || true
