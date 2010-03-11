CC=gcc
CFLAGS=-I../poly1305aes-20050218

src = aes_any_c.c \
    aes_any_constants_c.c \
    aes_any_constants.S \
    aes_any.S \
    empty.c \
    poly1305aes_any_authenticate.c \
    poly1305aes_any_clamp.c \
    poly1305aes_any_isequal_c.c \
    poly1305aes_any_isequal.S \
    poly1305aes_any_verify.c \
    poly1305_any_c.c \
    poly1305_any_constants_c.c \
    poly1305_any_constants.S \
    poly1305_any.S

objects = aes_any_c.o \
    aes_any_constants_c.o \
    aes_any_constants.o \
    aes_any.o \
    empty.o \
    poly1305aes_any_authenticate.o \
    poly1305aes_any_clamp.o \
    poly1305aes_any_isequal_c.o \
    poly1305aes_any_isequal.o \
    poly1305aes_any_verify.o \
    poly1305_any_c.o \
    poly1305_any_constants_c.o \
    poly1305_any_constants.o \
    poly1305_any.o

all: libpoly1305aes.a

libpoly1305aes.a: $(src)
	$(CC) -c $(src) $(CFLAGS)
	ar cr libpoly1305aes.a $(objects)

clean:
	-rm $(objects)
	-rm libpoly1305aes.a
