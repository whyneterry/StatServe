CFLAGS=-g -O2 -Wall -Wextra -Isrc -rdynamic -DNDEBUG $(OPTFLAGS)
LDLIBS=-ldl $(OPTLIBS) -lm
PREFIX?=/usr/local

SOURCES=$(wildcard src/**/*.c src/*.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

TEST_SRC=$(wildcard tests/*_tests.c)
TESTS=$(patsubst %.c,%,$(TEST_SRC))

PROGS_SRC=$(wildcard bin/*.c)
PROGS=$(patsubst %.c,%,$(PROGS_SRC))

TARGET=build/liblcthw.a
SO_TARGET=$(patsubst %.a,%.so,$(TARGET))

# The Target Build
all: $(TARGET) $(SO_TARGET) tests $(PROGS)

dev: CFLAGS=-g -Wall -Isrc -Wall -Wextra $(OPTFLAGS)
dev: all

$(TARGET): CFLAGS += -fPIC
$(TARGET): build $(OBJECTS)
	ctags -R
	ar rcs $@ $(OBJECTS)
	ranlib $@

$(SO_TARGET): $(TARGET) $(OBJECTS)
	$(CC) -shared -o $@ $(OBJECTS)

build:
	@mkdir -p build
	@mkdir -p bin

# Правило для компиляции тестов
$(TESTS): %: %.c $(TARGET)
	$(CC) $(CFLAGS) $< $(TARGET) $(LDLIBS) -o $@

$(PROGS): %: %.c $(TARGET)
	$(CC) $(CFLAGS) $< $(TARGET) $(LDLIBS) -o $@

# The Unit Tests
.PHONY: tests progs
tests: $(TESTS)
	sh ./tests/runtests.sh

progs: $(PROGS)

# Очистка
clean:
	rm -rf build $(OBJECTS) $(TESTS) $(PROGS)
	rm -f tests/tests.log
	find . -name "*.gc*" -exec rm {} \;
	rm -rf `find . -name "*.dSYM" -print`

# Установка
install: all
	install -d $(DESTDIR)/$(PREFIX)/lib/
	install $(TARGET) $(SO_TARGET) $(DESTDIR)/$(PREFIX)/lib/

# Проверка
check:
	@echo "Files with potentially dangerous functions:"
	@egrep '[^_.>a-zA-Z0-9](str(n?cpy|n?cat|xfrm|n?dup|str|pbrk|tok|_)|stpn?cpy|a?sn?printf|byte_)' $(SOURCES) || true
