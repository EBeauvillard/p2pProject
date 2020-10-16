SOURCES := $(wildcard build/*.txt)

all:
	mkdir -p build
	javac src/main.java tests/P2TTestst1.java src/main2.java src/Clients.java src/Peer.java src/Piece.java src/Tracker_File.java tests/PearToPearTests.java src/Tracker_File.java tests/P2PTests2.java tests/P2PTests3.java -cp src/ini4j-0.5.4/ini4j-0.5.4.jar -d build

test: all
	cd build && java -cp "../src/ini4j-0.5.4/ini4j-0.5.4.jar:." P2TTestst1 && java -cp "../src/ini4j-0.5.4/ini4j-0.5.4.jar:." main2

p2ptests: all
	rm -f $(SOURCES)
	cd build && java -cp "../src/ini4j-0.5.4/ini4j-0.5.4.jar:." PearToPearTests

donwloadtests: all
	rm -f $(SOURCES)
	cd build && java -cp "../src/ini4j-0.5.4/ini4j-0.5.4.jar:." P2PTests2 && java -cp "../src/ini4j-0.5.4/ini4j-0.5.4.jar:." P2PTests3

clean:
	rm -r build

executable: all
	rm -f $(SOURCES)
	cd build && java -cp "../src/ini4j-0.5.4/ini4j-0.5.4.jar:." main