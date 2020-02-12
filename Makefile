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

release: BAF HCF HCF-MoreKeys HCF-RESWUE HCF-SingleWork MultField TriField

BAF: src/BAF
	$(CXX) $(CXXFLAG) src/$@/baf-choose.cpp -o release/$@/baf-choose$(SUFFIX)

HCF: src/HCF
	$(CXX) $(CXXFLAG) src/$@/choose.cpp -o release/$@/choose$(SUFFIX)

HCF-MoreKeys: src/HCF-MoreKeys
	$(CXX) $(CXXFLAG) src/$@/morekey.cpp -o release/$@/morekey$(SUFFIX)

HCF-RESWUE: src/HCF-RESWUE
	$(CXX) $(CXXFLAG) src/$@/reswue.cpp -o release/$@/reswue$(SUFFIX)

HCF-SingleWork: src/HCF-SingleWork
	$(CXX) $(CXXFLAG) src/$@/act.cpp -o release/$@/act$(SUFFIX)

MultField: src/MultField
	$(CXX) $(CXXFLAG) src/$@/mult-choose.cpp -o release/$@/mult-choose$(SUFFIX)

TriField: src/TriField
	$(CXX) $(CXXFLAG) src/$@/tri-choose.cpp -o release/$@/tri-choose$(SUFFIX)