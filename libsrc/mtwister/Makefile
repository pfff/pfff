LIB=libMTwister.a
OBJ=MTwister.o

all: $(LIB)

$(LIB): $(OBJ)
	ar cr $(LIB) $(OBJ)

%.o: %.cpp %.h
	$(CXX) -c -o $@ $<

clean:
	-rm $(LIB) $(OBJ)
