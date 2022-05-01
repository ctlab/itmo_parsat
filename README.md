IPS (itmo-parsat): backdoor search algorithms and parallel SAT solver.

# Description

This project contains efficient C++ implementations of rho-backdoor search algorithms
alongside implementations of SAT instance reducing and solving. Generally, we've implemented:
* Parallel and tree-based approximation and calculation of rho-values of sets of variables.
* Rho-backdoor and Rho-backdoor-set based SAT instance splitting algorithms.
* Parallel solver of subproblems, allowing usage of different (theoretially, any) native
  solver implementations. Clause sharing is also available for use and is highly configurable.
* Straightforward and recurring strategies of solving subtasks.
* Performance bencmarks and unit tests for some subroutines.
* Integrational tests infrastructure (`infra/`).

# Current issues
Please report any bugs or other issues either here or via email (dzhiblavi@gmail.com).
Currently we are facing the following issues:
* Correctness heavily relies on native implementations (MiniSat, painless). During
  implementation of this project, they have proven themselves to be not exactly very safe.
* Native solvers contain lots of dirty code. This issue has been fixed partially,
  yet enabling warnings and errors or using sanitizers in some configurations using dirty code
  will produce errors.

# Releases and tags description, changelog
* `v1.0`. First prestable and highly tested version of the library.
* `v2.0` [TBD]. 
  - Added SAT model output capability.
  - Added rho-backdoor search utility.
  - Added SAT certificate verification tool.
  - Minor core and documentation refactoring.
  - Monor bugs and dirty pieces of code fixed.

# Working with this project

For convenience and ease, the development and usage environment has been prepared via Docker
image. The image itself is described in `scripts/docker/ips.Dockerfile`. Also, some of the
source code is covered with documentation, which is available at the `doc/` directory in
`HTML` format. Feel free to view it through your browser.

## Setup

At first, pull this project and change directory:
```console
$ git pull https://github.com/dzhiblavi/itmo-parsat.git
$ cd itmo-parsat
```

Then, run the setup script that will create all necessary links and hooks
```console
$ ./setup.sh
```

Now one is ready to build the docker image (you will need to do it only once).
The `d` script can be used to do it easily.
```console
$ ./d -b
```

After the container is successfully built, one can run the container:
```console
$ ./d -r
```

Now one can attach to the running container:
```console
$ ./d -a
```

## Building

In order to build (or rebuild) the project, just use the `ips` utility script. The
following command will build the project in `DEV_FAST` mode:
```console
$ ./ips -g DEV_FAST -b
```

The partial list of available modes are:
* `DEBUG`. The debugging build mode, enabling all information needed for gdb and using
  low (or not at all) optimization level.
* `DEV_FAST`. The 'default' build mode for development. Builds with logging, tracing but with
  high (O3) optimization level. Also enables LTO.
* `RELEASE`. The fastest available build mode: is slightly faster than `DEV_FAST`.

Other options can be found in `CMakeLists.txt` or by running `./ips --help`.

## Execution

After the build, four binaries will be produced:
* `cli/verify_bin`: SAT certificate verification tool.
* `cli/rb_search_bin`: Rho-backdoor search tool.
* `cli/solve_bin`: SAT solver itself.
* `cli/infra_bin`: Integrational testing or benchmarking tool.

Available usage options can be seen via running any of these binaries with `--help` option.

# Example

For easier dive-into, one can start from these simple use cases. Example configurations
can be found in `resources/config` directory. Some SAT problem instances can be found
in `resources/cnf` subdirectories. In order to run SAT solver on some instance, please
try the following command:
```console
$ ./ips -c 16_wop_s1-16_sing_rbs_ea -i resources/cnf/<some-instance> -s
```

The script will start the preconfigured execution, writing the following files:
* `proof.txt`: the DIMACS SAT certificate in case of SAT result.

