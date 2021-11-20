# How to build and run.

## Build
```console
$ git clone git@github.com:dzhiblavi/itmo-parsat.git
$ cd itmo-parsat
$ ./update-proto.sh
$ bazel build //evol:main //evol:test
```

## Run
```console
$ ./bazel-bin/evol/main <options>
```

