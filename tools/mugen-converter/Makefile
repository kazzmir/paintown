all:
	@scons -j `python ../../misc/cores.py`
	@python ../../misc/gnome-notify "Finished compiling" 2>/dev/null || echo "Done"
	
clean:
	@scons -c
