# How to build and run

## Build
```console
$ git clone git@github.com:dzhiblavi/itmo-parsat.git
$ cd itmo-parsat
$  ./run.sh -b
```

Build requires `g++-8` or higher. If system `g++` has lower version, use
```console
$ CC=gcc-8 CXX=g++-8 ./run.sh -b
```
instead of the last command.

## Run
Generally, everything can be done through `run.sh` script. One can run the following command
to get the list of what can be done.
```console
$ ./run.sh --help
```

For example, to run tests, use
```console
$ ./run.sh -t
```

To run solution with some 'default' configuration, use
```console
$ ./run.sh -sd
```

The verbosity is controlled via `v` option.
