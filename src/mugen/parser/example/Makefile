math: math.peg driver.cpp ast.cpp
	python peg.py math.peg --cpp > math.cpp
	python peg.py math.peg --h > math.h
	g++ -g3 driver.cpp math.cpp ast.cpp -o math
