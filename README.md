This is a repository for our final project in CDA-5106 (Advanced Computer Architecture), in which we are modifying a branch predictor to use opcode information to improve branch prediction.

## Compilation

This project relies on the open source hardware simulation software [SimpleSim 3.0](https://github.com/toddmaustin/simplesim-3.0) for branch prediction, which we modify slightly for our purposes. This software uses one of two ISAs to simulate low level processes such as branch prediction, caching, etc. To use this project, SimpleSim must first be compiled, which can be done by following instructions b-f as listed under the installation instructions of the SimpleSim README.

SimpleSim can be compiled in one of two ways, one for each of the ISAs it uses. For this reason, to run SimpleSim you need binaries specifically compiled with a special C compiler for the given instruction set. The ones most useful for this project are the Alpha binaries in `/xbenchmarks`, which we will be using for our results.

## Compilation

## Running Tests
