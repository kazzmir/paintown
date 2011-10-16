#!/usr/bin/env python

# start up windows vm
# some script should start on bootup and try to connect back to this script (through some port)
# this script should send commands for building the tree and creating the exe
# could also have a simple file transfer mechanism so I can get the exe without having to muck around with ftp or whatever

# To setup virtualbox so this script works with it do this
# Set the networking for interface 0 as NAT for the VM
# By default Virtualbox will use 10.0/16 as the subnet to supply IP's from.
# To change it to 192.168.90/8 use
#  $ VBoxManage modifyvm "xp-dev" --natnet1 "192.168.90/8"
# The gateway (host) will be 192.168.90.2 and the first address for the machine
# will be 192.168.90.15. It doesn't really matter what the client IP is,
# all that matters is the gateway IP is 192.168.90.2 (the server_ip variable).

# This is completely arbitrary but the server and client need to agree on it.
main_port = 15421
# Default network settings
server_ip = '10.0.2.2'
# network settings in jon's virtual box 
# server_ip = '192.168.90.2'
quit_message = '**quit**'
clear_errors_message = '**clear-errors**'
transfer_message = '**transfer**'
paintown_version = '3.6.0'

class CommandFailure(Exception):
    pass

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
    def connect(address, port):
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
            connection.sendall('changed directory to ' + args[1])
        else:
            process = subprocess.Popen(args, stdin = subprocess.PIPE, stdout = subprocess.PIPE, stderr = subprocess.STDOUT, shell = True)
            stdout = process.stdout
            out = stdout.readline()
            while out != None and out != "":
                log_debug("Sending line '%s'" % out)
                connection.sendall(out)
                out = stdout.readline()
            process.wait()
            if process.returncode != 0:
                log_debug("'%s' failed!" % ' '.join([str(x) for x in args]))
                connection.sendall("*FAILURE* '%s' did not succeed. returncode was %s" % (args, process.returncode))
                raise CommandFailure(args)

    def send_file(connection, port, path):
        send = connect(server_ip, port)
        file = open(path, 'rb')
        connection.sendall('Sending file %s' % path)
        send.sendall(file.read())
        file.close()
        send.close()
        connection.sendall('Sent file %s' % path)

    def read_commands(connection):
        import re
        try:
            line = re.compile('(.*)\n\n')
            data = ""
            ignore = False
            while True:
                more = connection.recv(4096)
                if not more:
                    break
                data += more
                log_debug("Buffer is now '%s'" % data)
                get = line.match(data)
                while get != None:
                    command = get.group(1)
                    if ignore:
                        if command == clear_errors_message:
                            ignore = False
                        else:
                            log_debug("Ignoring '%s'" % command)
                    elif command == clear_errors_message:
                        ignore = False
                    elif command == quit_message:
                        connection.close()
                        return
                    elif command.split(' ')[0] == transfer_message:
                        all = command.split(' ')
                        port = int(all[1])
                        file = all[2]
                        send_file(connection, port, file)
                    else:
                        log_debug("Got command '%s'" % command)
                        try:
                            out = do_command(command, connection)
                        except Exception, e:
                            log_error("Got an error.. closing the connection: " + str(e))
                            ignore = True

                    # chop of the command from the buffer
                    data = data[(len(command) + 2):]
                    get = line.match(data)
        except Exception, e:
            log_error("Got an error.. closing the connection: " + str(e))
            connection.close()

    def run():
        read_commands(connect(server_ip, main_port))

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
    def wait_for_connect(port):
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
        connection.sendall(command)
        connection.sendall("\n\n")

    def compute_md5(file):
        import hashlib
        f = open(file)
        m = hashlib.md5()
        m.update(f.read())
        f.close()
        return m.hexdigest()

    def do_receive_file(transfer, path):
        (client, ignore_address) = transfer.accept()
        size = 4096
        file = open(path, 'wb')
        data = client.recv(size)
        while data:
            file.write(data)
            data = client.recv(size)
        file.close()
        client.close()
        transfer.close()

        print "%s %s [server]" % (compute_md5(path), path)

    # gets the text output from sending commands
    def send_build_commands(connection):
        import socket
        import threading
        # send_command(connection, 'ls')
        send_command(connection, 'cd c:/svn/paintown')
        send_command(connection, 'svn update')
        send_command(connection, 'cd editor')
        send_command(connection, 'ant')
        send_command(connection, 'cd ..')
        send_command(connection, 'make %s' % ' '.join(make_commands))
        transfer = socket.socket()
        transfer.bind(('0.0.0.0', 0))
        transfer.listen(1)
        transfer_port = transfer.getsockname()[1]
        file = 'paintown-win32-%s.exe' % paintown_version
        send_command(connection, 'md5sum misc/%s' % file)

        receive = threading.Thread(target = do_receive_file, args = (transfer, file))
        receive.start()
        send_command(connection, '%s %d misc/%s' % (transfer_message, transfer_port, file))

        # If an error occurs in the guest then he will skip all commands until
        # he sees the **clear-errors** message and then will continue to process
        # commands
        send_command(connection, clear_errors_message)
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
        receive.join()

    def run():
        import time
        start = time.time()
        vm = start_windows_vm()
        send_build_commands(wait_for_connect(main_port))
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
