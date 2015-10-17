# Trivial makefile for the calculator scanner/parser.
# Depends on default (built-in) rules for C compilation.

# Note that rule for goal (parse) must be the first one in this file.



parse: parse.o scan.o
	g++ -o parse parse.o scan.o

clean:
	rm -f scan.o parse.o parse

parse.o: scan.h
scan.o: scan.h

target: parse < test.txt > stdout

test0:
	./parse < ./tests/test00.txt
	./parse < ./tests/test01.txt
	./parse < ./tests/test02.txt
	./parse < ./tests/test03.txt
	./parse < ./tests/test04.txt
	./parse < ./tests/test05.txt
	./parse < ./tests/test06.txt
	./parse < ./tests/test07.txt
	./parse < ./tests/test08.txt
	./parse < ./tests/test09.txt

test1:
	./parse < ./tests/test10.txt
	./parse < ./tests/test11.txt
	./parse < ./tests/test12.txt
	./parse < ./tests/test13.txt
	./parse < ./tests/test14.txt
	./parse < ./tests/test15.txt
	./parse < ./tests/test16.txt
	./parse < ./tests/test17.txt
	./parse < ./tests/test18.txt
	./parse < ./tests/test19.txt

test2:
	./parse < ./tests/test20.txt
	./parse < ./tests/test21.txt
	./parse < ./tests/test22.txt
	./parse < ./tests/test23.txt
	./parse < ./tests/test24.txt
	./parse < ./tests/test25.txt
	./parse < ./tests/test26.txt
	./parse < ./tests/test27.txt
	./parse < ./tests/test28.txt
	./parse < ./tests/test29.txt

test3:
	./parse < ./tests/test30.txt
	./parse < ./tests/test31.txt
	./parse < ./tests/test32.txt
	./parse < ./tests/test33.txt
	./parse < ./tests/test34.txt
	./parse < ./tests/test35.txt
	./parse < ./tests/test36.txt
	./parse < ./tests/test37.txt
	./parse < ./tests/test38.txt
	./parse < ./tests/test39.txt

test4:
	./parse < ./tests/test40.txt
	./parse < ./tests/test41.txt
	./parse < ./tests/test42.txt
	./parse < ./tests/test43.txt
	./parse < ./tests/test44.txt
	./parse < ./tests/test45.txt
	./parse < ./tests/test46.txt
	./parse < ./tests/test47.txt
	./parse < ./tests/test48.txt
	./parse < ./tests/test49.txt

test5:
	./parse < ./tests/test50.txt
	./parse < ./tests/test51.txt
	./parse < ./tests/test52.txt
	./parse < ./tests/test53.txt
	./parse < ./tests/test54.txt
	./parse < ./tests/test55.txt
	./parse < ./tests/test56.txt
	./parse < ./tests/test57.txt
	./parse < ./tests/test58.txt
	./parse < ./tests/test59.txt

test6:
	./parse < ./tests/test60.txt
	./parse < ./tests/test61.txt
	./parse < ./tests/test62.txt
	./parse < ./tests/test63.txt
	./parse < ./tests/test64.txt
	./parse < ./tests/test65.txt
	./parse < ./tests/test66.txt
	./parse < ./tests/test67.txt
	./parse < ./tests/test68.txt
	./parse < ./tests/test69.txt

test7:
	./parse < ./tests/test70.txt
	./parse < ./tests/test71.txt
	./parse < ./tests/test72.txt
	./parse < ./tests/test73.txt
	./parse < ./tests/test74.txt
	./parse < ./tests/test75.txt
	./parse < ./tests/test76.txt
	./parse < ./tests/test77.txt
	./parse < ./tests/test78.txt
	./parse < ./tests/test79.txt


