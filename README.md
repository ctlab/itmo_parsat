Probabilistic Generalization of Backdoor Trees with Application to SAT

# Description

This release is dedicated to support the paper <i>Semenov A., Chivilikhin D., Kochemazov S., Dzhiblavi I. Probabilistic Generalization of Backdoor Trees with Application to SAT / 37th AAAI Conference on Artificial Intelligence, 2023</i> by providing the implementation of the proposed SAT solving approach and scripts for reproducing the 
experimental results.

The SAT solving approach proposed in the paper is comprised of two consecutive steps:
* finding a probabilistic backdoor tree for the given CNF formula;
* solving the CNF formula using the found backdoor tree.

The tool implementing the developed approach can be accessed via:
* either pulling a prebuilt Docker image from Docker Hub;
* or building the environment and the tool using the supplied Dockerfile;

The scripts for reproducing the experimental results reported in the paper are also available (see release notes).

# Setup

## Docker image

The simplest way to build the tool and run the experiments is to pull a pre-build Docker image from Docker Hub.
The image contains an installation of the tool along with all necessary dependencies.
To get the image, run:

```console
$ docker pull chivilikds/itmo-parsat:latest
```

After the download is complete, you can launch the Docker container, and attach to it:
```console 
$ docker run -dt chivilikds/itmo-parsat:latest
$ docker exec -it $(docker container ls | grep "chivilikds/itmo-parsat:latest" | awk '{print $1}') /bin/bash
```

## Dockerfile

First, clone the project and change directory:
```console
$ git clone https://github.com/ctlab/itmo-parsat.git
$ cd itmo-parsat
```

Second, setup the build by calling
```console
$ ./setup.sh
```

Third, build the image using the provided `d` script:
```console
$ ./d -b
```

After the container is successfully built, one can run the container:
```console
$ ./d -r
```
and attach to the running container:
```console
$ ./d -a
```

# Running the experiments from the AAAI-23 paper

In order to run the experiments, attach to the running Docker container and execute

```console
$ ./find-backdoor-trees
```

You will find the results in the <b>results-aaai23</b> directory.

# Additional information

Apart from the implementation of probabilistic backdoor tree search algorithms, this project also efficient C++ implementations of:
* parallel and tree-based approximation and calculation of rho-values of sets of variables (candidate backdoors);
* algorithms for splitting SAT instances based on backdoor trees;
* parallel solver of subproblems, allowing usage of different (theoretially, any) native
  solver implementations. Clause sharing is also available for use and is highly configurable;
* straightforward and recursive strategies of solving subproblems.

These are a part of a parallel SAT solver based on the probabilistic backdoor trees. 
The solver itself is under development.
