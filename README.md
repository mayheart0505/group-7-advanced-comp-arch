This is a repository for our final project in CDA-5106 (Advanced Computer Architecture), in which we are modifying a branch predictor to use opcode information to improve branch prediction.

## Introduction

This project relies on the open source hardware simulation software [SimpleSim 3.0](https://github.com/toddmaustin/simplesim-3.0) for branch prediction, which we modify slightly for our purposes. This software uses one of two ISAs to simulate low level processes such as branch prediction, caching, etc. To use this project, SimpleSim must first be compiled, which can be done by following instructions b-f as listed under the installation instructions of the SimpleSim README.

SimpleSim can be compiled in one of two ways, one for each of the ISAs it uses. For this reason, to run SimpleSim you need binaries specifically compiled with a special C compiler for the given instruction set. The ones most useful for this project are the PISA binaries in `/spec95-little`, which we will be using for our results, but you may also try using the Alpha binaries in `/xbenchmarks`. Just make sure you recompile SimpleSim for the correct instruction set.

## Compilation

To compile SimpleSim, follow the instructions b-f as listed in the SimpleSim readme (located at `/simplesim-3.0/README.md`). This will produce 6 executables: sim-fast, sim-cache, sim-bpred, sim-profile, sim-outorder, sim-safe. Since our project only involves branch prediction we will only be using sim-bpred, but you may use the other programs for testing.

## Running SimpleSim

After checking that SimpleSim has compiled correctly, you should check that SimpleSim can work on your system. Run the following commands.

```bash
cd spec95-little/
../simplesim-3.0/sim-bpred applu.ss
```

You should see two small windows appear, one of which has an animation that starts with zooming in on a book.
