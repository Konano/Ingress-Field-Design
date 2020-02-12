CXX = g++
CXXFLAG = --std=c++14 -Wall

ifeq ($(OS), Windows_NT)
	SUFFIX = .exe
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		SUFFIX = .out
	endif
	ifeq ($(UNAME_S),Darwin)
		SUFFIX =
	endif
endif

release: BAF MultField HCF-SingleWork

BAF: src/BAF
	$(CXX) $(CXXFLAG) src/$@/baf-choose.cpp -o release/$@/baf-choose$(SUFFIX)

MultField: src/MultField
	$(CXX) $(CXXFLAG) src/$@/mult-choose.cpp -o release/$@/mult-choose$(SUFFIX)

HCF-SingleWork: src/HCF-SingleWork
	$(CXX) $(CXXFLAG) src/$@/act.cpp -o release/$@/act$(SUFFIX)
