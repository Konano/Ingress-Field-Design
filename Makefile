CXX = g++
CXXFLAG = --std=c++14 -Wall

release: MultField HCF-SingleWork
	cp src/portals2json.user.js release/linux/portals2json.user.js

MultField: src/MultField
	$(CXX) $(CXXFLAG) src/$@/mult-choose.cpp -o release/linux/$@/mult-choose.out 
	cp src/$@/mult-portal-sample.txt release/linux/$@/mult-portal-sample.txt
	cp src/$@/mult-result-sample.txt release/linux/$@/mult-result-sample.txt

HCF-SingleWork: src/HCF-SingleWork
	$(CXX) $(CXXFLAG) src/$@/act.cpp -o release/linux/$@/act.out 
	cp src/$@/act-sample.txt release/linux/$@/act-sample.txt
	cp src/$@/way-sample.txt release/linux/$@/way-sample.txt
	cp src/$@/label.txt release/linux/$@/label.txt
	