IPS: Itmo-2021/2 backdoor search based ParSat algorithms.

# Setup
In order to start working with this project, run
```console
$ ./setup.sh
```
This script will prepare all needed symlinks, including Git hooks and utility scripts.

# Docker
To automatically prepare all need environment (and reproduce the results), you can 
use docker containers. After setup, run
```console
$ ./d
```

This will show you the list of available options to manipulate docker containers and images.
For example, to start a container and attach to it, simply run
```console
$ ./d -s -b -r -a
```

After attaching to the container, you can close the shell when needed, by calling `exit` command.
This will not stop the container, you will be able to attach to it later:
```console
$ ./d -a
```

# Running
After the environment has been prepared (via docker or manually), one can start working with the IPS.
The `ips` script is designed for it:
```console
$ ./ips
```
will show you the list of options. For example, one can build, then run integrational test (with size 0, the
minimal one, and test group `common`), via the following command:
```console
$ ./ips -g DEV_FAST --build-infra -- --run-infra --size 0 --test-groups common --
```
where the `-g dev_fast` is build mode. Available build modes are listed in `.bazelrc`.
Another example. run the solution of the default CNF task with `par_rbs` algorithm:
```console
$ ./ips -g DEV_FAST -b -v 6 -c par_rbs -s
```
