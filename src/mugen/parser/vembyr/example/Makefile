math: math.peg driver.cpp ast.cpp peg.py
	python peg.py math.peg --cpp > math.cpp
	python peg.py math.peg --h > math.h
	g++ -g3 driver.cpp math.cpp ast.cpp -o math

clean:
	rm -f math.cpp math.h math
