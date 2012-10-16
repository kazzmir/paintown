all: cmd def air

peg: def.cpp air.cpp cmd.cpp peg.py cpp_generator.py

cmd2: peg.py
	./peg.py --c++-interpreter cmd.peg > cmd2.cpp
	# g++ -pg -O2 -I../../ -Wall cmd2.cpp cmd2-run.cpp util.cpp ../../exceptions/exception.cpp ../exception.cpp -o cmd2
	# g++ -g3 -I../../ -Wall cmd2.cpp cmd2-run.cpp util.cpp ../../exceptions/exception.cpp ../exception.cpp -o cmd2

x: x.cpp x-run.cpp
	g++ -g3 -Wall x-run.cpp x.cpp -o x

x.cpp: peg.py x.peg
	./peg.py x.peg --c++-interpreter > x.cpp

#all: def air cmd

def: def.cpp def-run.cpp gc.h
	# -g++ -I../.. def.cpp def-run.cpp util.cpp ../../exceptions/exception.cpp ../exception.cpp -g3 -o def
	-g++ -I../.. -g3 def.cpp def-run.cpp util.cpp ../../util/exceptions/exception.cpp ../../util/token.cpp ../../util/token.cpp ../ast/ast.cpp ../exception.cpp -o def

def.cpp: def.peg peg.py
	./peg.py --cpp def.peg > def.cpp

air: air.cpp air-run.cpp
	-g++ -I../../ air.cpp air-run.cpp util.cpp ../../util/exceptions/exception.cpp ../exception.cpp ../../util/token.cpp ../../util/token.cpp ../ast/ast.cpp -g3 -o air

air.cpp: air.peg peg.py
	./peg.py --cpp air.peg > air.cpp

cns: cns.cpp cns-run.cpp
	-g++ -I../.. cns.cpp cns-run.cpp util.cpp ../exception.cpp -g3 -o cns

cns.cpp: cns.peg peg.py
	./peg.py --cpp cns.peg > cns.cpp

cmd: cmd.cpp cmd-run.cpp
	@#-g++ -I../.. -pg -O2 cmd.cpp cmd-run.cpp util.cpp ../../exceptions/exception.cpp ../exception.cpp -o cmd
	@# -g++ -I../.. -march=native -pipe -fomit-frame-pointer -O2 cmd.cpp cmd-run.cpp util.cpp ../../exceptions/exception.cpp ../../util/token.cpp ../../util/token_exception.cpp ../ast/ast.cpp ../exception.cpp -o cmd
	-g++ -I../.. -g3 cmd.cpp cmd-run.cpp util.cpp ../../util/exceptions/exception.cpp ../../util/token.cpp ../ast/ast.cpp ../exception.cpp ../../util/debug.cpp -o cmd
	@# -g++ -I../.. -O2 -g cmd.cpp cmd-run.cpp util.cpp ../../exceptions/exception.cpp ../../util/token.cpp ../../util/token_exception.cpp ../ast/ast.cpp ../exception.cpp -o cmd

cmd.cpp: cmd.peg peg.py cpp_generator.py
	./peg.py --cpp cmd.peg > cmd.cpp
