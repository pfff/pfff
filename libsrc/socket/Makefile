LIB=libSocket.a
OBJ=Socket.o FtpSocket.o

all: $(LIB)

$(LIB): $(OBJ)
	ar cr $(LIB) $(OBJ)

%.o: %.cpp %.h
	$(CXX) -c -o $@ $<

clean:
	-rm $(LIB) $(OBJ)
