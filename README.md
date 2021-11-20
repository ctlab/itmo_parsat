# How to build and run

## Build
```console
$ git clone git@github.com:dzhiblavi/itmo-parsat.git
$ cd itmo-parsat
$ ./update-proto.sh
$ bazel build //evol:main
```

## Run
Example could be found in `run.sh`. Generally:

```console
$ ./bazel-bin/evol/main [args]+
```

With full logging to stderr:

```console
$ GLOG_minloglevel=0 GLOG_logtostderr=1 ./bazel-bin/evol/main [args]+
```

