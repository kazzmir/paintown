#!/usr/bin/env python

# start up windows vm
# some script should start on bootup and try to connect back to this script (through some port)
# this script should send commands for building the tree and creating the exe
# could also have a simple file transfer mechanism so I can get the exe without having to muck around with ftp or whatever

port = 15421

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
    def do_command(command):
        pass

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
                    log_debug("Got command '%s'" % command)
                    do_command(command)
                    # chop of the command from the buffer
                    data = data[(len(command) + 2):]
                    get = line.match(data)
        except:
            log_error("Got an error.. closing the connection")
            connection.close()

    def run():
        read_commands(connect('localhost'))

    run()

def server_side():
    def start_windows_vm():
        def start_virtualbox():
            import subprocess
            # specific to jon's setup
            vm_name = "xp-dev"
            executable = "VBoxSDL"
            return subprocess.Popen([executable, "-startvm", vm_name])

        start_virtualbox()

    # returns a connection
    def wait_for_connect():
        import socket
        server = socket.socket()
        server.bind(('localhost', port))
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
        send_command(connection, 'cd c:/svn/paintown')
        send_command(connection, 'svn update')
        send_command(connection, 'make win')
        connection.close()

    def run():
        # start_windows_vm()
        send_build_commands(wait_for_connect())
        log_info("All done")

    run()

import sys
if len(sys.argv) < 2:
    log_error("pass 'client' or 'server' as an argument")
else:
    import re
    verbose_arg = re.compile('verbose=(\d+)')
    for arg in sys.argv[1:]:
        if arg == 'client':
            client_side()
        elif arg == 'server':
            server_side()
        elif verbose_arg.match(arg) != None:
            out = verbose_arg.match(arg)
            verbose = int(out.group(1))
