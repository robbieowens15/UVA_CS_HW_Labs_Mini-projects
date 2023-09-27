# Robert Owens (rao7utn)
# 11/08/2020
# Makefile for lab-10
CXX=clang++ $(CXXFLAGS)
CXXFLAGS=-O2
ENC_BINARY=-o encoder
DEC_BINARY=-o decoder
ENC_OBJECTS= huffmanenc.o huffmanNode.o heap.o
DEC_OBJECTS= huffmandec.o huffmanNode.o

create_encoder_and_decoder: encode decode

encode: $(ENC_OBJECTS)
	$(CXX) $(ENC_OBJECTS) $(ENC_BINARY)

decode: $(DEC_OBJECTS)
	$(CXX) $(DEC_OBJECTS) $(DEC_BINARY)

huffmanNode.o: huffmanNode.cpp huffmanNode.h

huffmanenc.o: huffmanenc.cpp heap.h huffmanNode.h
heap.o: heap.cpp heap.h huffmanNode.h

huffmandec.o: huffmandec.cpp huffmanNode.h

.PHONY: clean
clean:
	-rm -f *.out *.o