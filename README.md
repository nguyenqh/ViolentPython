Gentle Python
===================

Python 101
--------------

#### Script Skeleton

Use the following template as a base for all python scripts:

```python
#!/usr/bin/python

def main():
    pass    # Code goes in here

if __name__ == "__main__":
    main()
```

#### Executing programs
We can make use of either the `os` or `subprocess` modules to perform this. Let's take a look at the `os` module first:

```python
#!/usr/bin/python

#import the os
import os

def main():
    # We can run shell commands by using os.system('shell command')
    # This is the equivalent of running sh -c '<your command here>' so things
    # like pipes, redirects, semicolons work.

    # /usb/bin/md5sum is the program to execute
    # /etc/issue.net is the first argument to the program
    os.system("/usr/bin/md5sum /etc/issue.net")

    # Let's demonstrate that the features of the shell is available to us.
    os.system("echo 'Hello, Friend' > badfile; cat badfile")

if __name__ == "__main__":
    main()
```

The `os.system()` function is basically a wrapper to run shell commands. There is a big distinction to running a program within a shell and without as we will see when we use the subprocess module instead. Running the program yields:

```console
root@kali:~/violentpython# ./argumentsos.py
50f6c3d0173030eee3701f6cccc7f22e  /etc/issue.net
Hello, Friend
```

Checking that the file indeed exists within the current directory now:

```console
root@kali:~/violentpython# ls -la badfile
-rw-r--r-- 1 root root 14 Feb  6 03:54 badfile
```

Now, let us use the `subprocess` module.

```python
#!/usr/bin/python

# import the subprocess module
import subprocess

def main():
    # With subprocess, you no longer have a shell environment since the program
    # is called directly instead of your commands being wrapped for execution
    # in a shell. The subprocess.call() function takes a list of strings where
    # the first string is the path to the program to execute and the following
    # strings the arguments to the program.

    # /usb/bin/md5sum is the program to execute
    # /etc/issue.net is the first argument to the program
    subprocess.call(["/usr/bin/md5sum", "/etc/issue.net"])
    
    subprocess.call(["echo", "'Hello, Friend' > badfile; cat badfile"])

if __name__ == "__main__":
    main()
```

The special symbols that meant various things within the shell such as pipes, backticks, redirections now are just passed directly to the program instead of interpreted by the shell and will do nothing. Executing the python script:

```console
root@kali:~/violentpython# ./argumentssubprocess.py 
50f6c3d0173030eee3701f6cccc7f22e  /etc/issue.net
'Hello, Friend' > badfile; cat badfile
```

#### Files I/O
```python
#!/usr/bin/python

def main():
    # Let's create a file
    towrite = open("my_file", "w")  # 'w' stands for open for write

    # Now, we can write a string into it
    towrite.write("some string")

    # Close the file
    towrite.close()

    # We can do the converse option by reading the file
    toread = open("my_file")  # implicitly open for read

    # Read the contents
    contents = toread.read()
    print contents

    # Close the file
    toread.close()

if __name__ == "__main__":
    main()
```

### Sockets

The majority of the exercises we will be going through involve interacting with a service hosted on a different system. That is, we will be looking at obtaining access to a remote system. The `socket` module in Python can help you create arbitrary TCP or UDP sockets.

```python
#!/usr/bin/python

import socket

def main():
    # Create a socket
    sock = socket.socket()

    # Connect the socket to the remote service
    sock.connect(("vuln2014.picoctf.com", 4546))

    # Receive the first prompt
    prompt = sock.recv(1024)  # Receive an arbitrary max 1024 bytes
    print prompt

    # Send our name
    sock.send("Hank\n")  # Remember the newline at the end

    # Receive the flag
    line = sock.recv(1024)
    print line

    line = sock.recv(1024)
    print line

    sock.send("098172348234\n")

    line = sock.recv(1024)
    print line

if __name__ == "__main__":
    main()

```



`pawntools` library
--------------

### Installation

```console
$ sudo apt-get install python-pip
$ sudo pip install --upgrade pip
$ sudo pip install pwntools
```

### Skeleton script


I like to begin with the following [template] when starting a new exploit.

```python
#!/usr/bin/python

from pwn import *

def main():
    pass

if __name__ == '__main__':
    main()
```

Running the script is as simple as calling python on it. Try running this
[script]:

```python
#!/usr/bin/python

from pwn import *

def main():
    p = process("/bin/sh")
    p.interactive()

if __name__ == '__main__':
    main()
```

### Interacting with Target Binaries

Your target might expose itself through different vectors. Today we will focus on attacking remotely running binaries that you can connect to over the network.
First, let's see how we might interact with a local copy of a binary that accepts input on `stdin` and returns output on `stdout`.

#### Local Copy of Binary

To begin with, we will look at the [2\_interactive binary]:

```shell
$ ./secureshell
Welcome to the Super Secure Shell
Password: HelloWorld?
Incorrect password!
```

For completeness sake, here is the [source code][secureshell_src]:

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void give_shell() {
    system("/bin/sh");
}

int main() {
    // Disable buffering on stdin and stdout to make network connections better.
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);

    char * password = "TheRealPassword";
    char user_password[200];

    puts("Welcome to the Super Secure Shell");
    printf("Password: ");

    scanf("%199s", user_password);
    if (strcmp(password, user_password) == 0) {
        puts("Correct password!");
        give_shell();
    }
    else {
        puts("Incorrect password!");
    }
}
```

The point of the program is to check the user input against a hardcoded
password. If it matches, then an interactive shell is spawned.

Now that we know how to craft the input, we can write our [sample exploit] using
Pwntools.

```python
#!/usr/bin/python

from pwn import *

def main():
    # Start a local process
    p = process("../build/2_interactive")

    # Get rid of the prompt
    data1 = p.recvrepeat(0.2)
    log.info("Got data: %s" % data1)

    # Send the password
    p.sendline("TheRealPassword")

    # Check for success or failure
    data2 = p.recvline()
    log.info("Got data: %s" % data2)
    if "Correct" in data2:
        # Hand interaction over to the user if successful
        log.success("Success! Enjoy your shell!")
        p.interactive()
    else:
        log.failure("Password was incorrect.")

if __name__ == "__main__":
    main()
```

#### Simulating a Networked Application Locally

It is very easy to turn a console-based application into a networked one and there are multiple ways to do it. For now, we can use socat to simulate a server daemon, to listen for network requests and then launch the binary to serve these requests.

First, we will start a new screen session so that we can background our socat terminal.

```shell
$ screen bash
```

Next, we run the following command to start a listener on port 1330.

```shell
$ socat TCP4-listen:1330,reuseaddr,fork EXEC:./secureshell

```

It should hang there. Now return to your original bash session by holding down the following key sequence: CTRL-A-D. If you run the command `screen -ls` you should see that the `socat` screen session is in the background.


To verify that the listener is indeed listening on port 1330, we can run netcat.

```shell
$ nc localhost 1330
Welcome to the Super Secure Shell
Password: Hello
Incorrect password!
```

Now, here comes [the magic]. To modify the first script we had to work with local binaries, we may simply replace the `process()` line with `remote("localhost", 1330)`.

```python
#!/usr/bin/python

from pwn import *

def main():
    # Start a local process
    #p = process("../build/2_interactive")
    p = remote("localhost", 1330)

    # Get rid of the prompt
    data1 = p.recvrepeat(0.2)
    log.info("Got data: %s" % data1)

    # Send the password
    p.sendline("TheRealPassword")

    # Check for success or failure
    data2 = p.recvline()
    log.info("Got data: %s" % data2)
    if "Correct" in data2:
        # Hand interaction over to the user if successful
        log.success("Success! Enjoy your shell!")
        p.interactive()
    else:
        log.failure("Password was incorrect.")

if __name__ == "__main__":
    main()
```

#### References:
1. [ctf101 system 2016 by nnamon](https://github.com/nnamon/ctf101-systems-2016/blob/master/lessonplan.md)
2. [Linux exploitation course by nnamon](https://github.com/nnamon/linux-exploitation-course/blob/master/lessons/3_intro_to_tools/lessonplan.md#pwntools)
3. [`pwnlib.tubes` - Talking to the World!](http://docs.pwntools.com/en/stable/tubes.html)
