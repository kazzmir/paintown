#!/usr/bin/env python

# start up windows vm
# some script should start on bootup and try to connect back to this script (through some port)
# this script should send commands for building the tree and creating the exe
# could also have a simple file transfer mechanism so I can get the exe without having to muck around with ftp or whatever

port = 15421
# network settings in jon's virtual box 
server_ip = '192.168.90.2'
quit_message = '**quit**'

# higher numbers of verbose output more stuff
verbose = 1

def log_debug(str, level = 2):
    if verbose >= level:
        print str

def log_info(str):
    log_debug(str, 1)

def log_error(str):
    log_debug(str, 0)

def client_side():
    def connect(address):
        import socket
        connection = socket.socket()
        res = socket.getaddrinfo(address, port)
        af, socktype, proto, canonname, socket_address = res[0]
        log_info("Connecting to %s:%d.." % (address, port))
        connection.connect(socket_address)
        log_info("Connected!")
        return connection

    # execute a command
    def do_command(command, connection):
        import subprocess
        log_info("Execute command '%s'" % command)
        args = command.split(' ')
        if args[0] == 'cd':
            import os
            os.chdir(args[1])
            connection.send('changed directory to ' + args[1])
        else:
            process = subprocess.Popen(args, stdin = subprocess.PIPE, stdout = subprocess.PIPE, stderr = subprocess.STDOUT, shell = True)
            stdout = process.stdout
            out = stdout.readline()
            while out != None and out != "":
                log_debug("Sending line '%s'" % out)
                connection.send(out)
                out = stdout.readline()
            process.wait()

    def read_commands(connection):
        import re
        try:
            line = re.compile('(.*)\n\n')
            data = ""
            while True:
                more = connection.recv(4096)
                if not more:
                    break
                data += more
                log_debug("Buffer is now '%s'" % data)
                get = line.match(data)
                while get != None:
                    command = get.group(1)
                    if command == quit_message:
                        connection.close()
                        return
                    log_debug("Got command '%s'" % command)
                    out = do_command(command, connection)
                    # chop of the command from the buffer
                    data = data[(len(command) + 2):]
                    get = line.match(data)
        except Exception, e:
            log_error("Got an error.. closing the connection: " + str(e))
            connection.close()

    def run():
        read_commands(connect(server_ip))

    run()

def server_side(make_commands):
    def start_windows_vm():
        def start_virtualbox():
            import subprocess
            # specific to jon's setup
            vm_name = "xp-dev"
            # executable = "VBoxSDL"
            # Run with no gui
            executable = "VBoxHeadless"
            return subprocess.Popen([executable, "-startvm", vm_name])

        process = start_virtualbox()
        print "Started virtual box. Use 'rdesktop localhost' to connect to the gui"
        return process

    # returns a connection
    def wait_for_connect():
        import socket
        server = socket.socket()
        server.bind(('0.0.0.0', port))
        server.listen(1)
        log_info("Waiting for a connection on port %d.." % port)
        (client, address) = server.accept()
        log_info("Got a connection from %s!" % str(address))
        return client

    # write the command and two newlines
    def send_command(connection, command):
        connection.send(command)
        connection.send("\n\n")

    # gets the text output from sending commands
    def send_build_commands(connection):
        # send_command(connection, 'ls')
        send_command(connection, 'cd c:/svn/paintown')
        send_command(connection, 'svn update')
        send_command(connection, 'cd editor')
        send_command(connection, 'ant')
        send_command(connection, 'cd ..')
        send_command(connection, 'make %s' % ' '.join(make_commands))

        # Wait 5 seconds to give time for the quit message to reach the
        # client script.
        send_command(connection, 'shutdown -s -f -t 5')
        send_command(connection, quit_message)
        size = 4096
        data = connection.recv(size)
        while data:
            print data.strip()
            data = connection.recv(size)
        connection.close()

    def run():
        import time
        start = time.time()
        vm = start_windows_vm()
        send_build_commands(wait_for_connect())
        log_info("Waiting for vm to close")
        vm.wait()
        end = time.time()
        log_info("All done. Took %f seconds" % (end - start))

    run()

import sys
if len(sys.argv) < 2:
    log_error("""valid arguments:
  client - run as the client (use this option on windows VM)
  server - run as the server (use this option on linux/host)
  make=a,b,c - send make commands a,b,c. default is just 'win'. if you supply this argument then you must supply the 'win' target or it won't be built
  verbose=# - set verbose level. 1 is the default. higher numbers is more verbose
""")
else:
    import re
    verbose_arg = re.compile('verbose=(\d+)')
    make_arg = re.compile('make=(.*)')
    make_commands = ['win']
    for arg in sys.argv[1:]:
        if arg == 'client':
            client_side()
        elif arg == 'server':
            server_side(make_commands)
        elif make_arg.match(arg) != None:
            out = make_arg.match(arg)
            make_commands = out.group(1).split(',')
        elif verbose_arg.match(arg) != None:
            out = verbose_arg.match(arg)
            verbose = int(out.group(1))
