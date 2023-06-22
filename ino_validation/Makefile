# Check the operating system
UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
    # Linux-specific commands
    CC = gcc
    CFLAGS = -c
    LDFLAGS = -L. -lcjson -static
else ifeq ($(UNAME), Darwin)
    # macOS-specific commands
    CC = gcc
    CFLAGS = -c
    LDFLAGS = -L. -lcjson -static -L/Library/Developer/CommandLineTools/usr/lib/swift_static/macosx/ -Bstatic
else
    $(error Unsupported operating system: $(UNAME))
endif

# Target rule for Linux and macOS
ino_validation: cJSON.o ino_validation.o
	$(CC) ino_validation.o -o ino_validation $(LDFLAGS)

# Dependency rules
cJSON.o: cJSON.c
	$(CC) $(CFLAGS) cJSON.c

ino_validation.o: ino_validation.c
	$(CC) $(CFLAGS) ino_validation.c

# Clean rule
clean:
	rm -f cJSON.o libcjson.a ino_validation.o ino_validation
