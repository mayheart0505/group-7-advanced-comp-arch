#!/bin/bash
rm ./results/*

cd simplesim-3.0/
make clean
make config-alpha
make
/usr/bin/gcc `./sysprobe -flags` -DDEBUG -O0 -g -Wall   -c sim-opcode.c
/usr/bin/gcc -o sim-opcode `./sysprobe -flags` -DDEBUG -O0 -g -Wall   sim-opcode.o opcode.o main.o syscall.o memory.o regs.o loader.o endian.o dlite.o symbol.o eval.o options.o stats.o eio.o range.o misc.o machine.o libexo/libexo.a `./sysprobe -libs` -lm

cd ../xbenchmarks/povray/exe
for ((i = 8 ; i < 65536 ; i*=2));
do
    ts=$(date +%s%N)
    ../../../simplesim-3.0/sim-opcode -opcode opcode -redir:sim ../../../results/povray_opcode.txt -opcode:opcode $i ./x-povray povray.ini -W320 -H200 -F +D -Q4 +I showoff/mist.pov
    echo "povray opcode $i $((($(date +%s%N) - $ts)/1000000))ms" >> ../../../timing_opcode.txt
done
for ((i = 8 ; i < 65536 ; i*=2));
do
    ts=$(date +%s%N)
    ../../../simplesim-3.0/sim-opcode -opcode bimod -redir:sim ../../../results/povray_bimod.txt -opcode:bimod $i ./x-povray povray.ini -W320 -H200 -F +D -Q4 +I showoff/mist.pov
    echo "povray bimod $i $((($(date +%s%N) - $ts)/1000000))ms" >> ../../../timing_opcode.txt
done
for ((i = 8 ; i < 65536 ; i*=2));
do
    ts=$(date +%s%N)
    ../../../simplesim-3.0/sim-opcode -opcode opcodecomb -redir:sim ../../../results/povray_opcodecomb.txt -opcode:opcodecomb $i ./x-povray povray.ini -W320 -H200 -F +D -Q4 +I showoff/mist.pov
    echo "povray opcodecomb $i $((($(date +%s%N) - $ts)/1000000))ms" >> ../../../timing_opcode.txt
done
for ((i = 8 ; i < 65536 ; i*=2));
do
    ts=$(date +%s%N)
    ../../../simplesim-3.0/sim-opcode -opcode comb -redir:sim ../../../results/povray_comb.txt -opcode:comb $i ./x-povray povray.ini -W320 -H200 -F +D -Q4 +I showoff/mist.pov
    echo "povray comb $i $((($(date +%s%N) - $ts)/1000000))ms" >> ../../../timing_opcode.txt
done
cd ../..


cd xdoom/exe
for ((i = 8 ; i < 65536 ; i*=2));
do
    ts=$(date +%s%N)
    ../../../simplesim-3.0/sim-opcode -opcode opcode -redir:sim ../../../results/xdoom_opcode.txt -opcode:opcode $i ./alphadoom -playdemo rockin
    echo "xdoom opcode $i $((($(date +%s%N) - $ts)/1000000))ms" >> ../../../timing_opcode.txt
done
for ((i = 8 ; i < 65536 ; i*=2));
do
    ts=$(date +%s%N)
    ../../../simplesim-3.0/sim-opcode -opcode bimod -redir:sim ../../../results/xdoom_bimod.txt -opcode:bimod $i ./alphadoom -playdemo rockin
    echo "xdoom bimod $i $((($(date +%s%N) - $ts)/1000000))ms" >> ../../../timing_opcode.txt
done
for ((i = 8 ; i < 65536 ; i*=2));
do
    ts=$(date +%s%N)
    ../../../simplesim-3.0/sim-opcode -opcode opcodecomb -redir:sim ../../../results/xdoom_opcodecomb.txt -opcode:opcodecomb $i ./alphadoom -playdemo rockin
    echo "xdoom opcodecomb $i $((($(date +%s%N) - $ts)/1000000))ms" >> ../../../timing_opcode.txt
done
for ((i = 8 ; i < 65536 ; i*=2));
do
    ts=$(date +%s%N)
    ../../../simplesim-3.0/sim-opcode -opcode comb -redir:sim ../../../results/xdoom_comb.txt -opcode:comb $i ./alphadoom -playdemo rockin
    echo "xdoom comb $i $((($(date +%s%N) - $ts)/1000000))ms" >> ../../../timing_opcode.txt
done
cd ../..


cd xlock/exe
for ((i = 8 ; i < 65536 ; i*=2));
do
    ts=$(date +%s%N)
    ../../../simplesim-3.0/sim-opcode -opcode opcode -redir:sim ../../../results/xlock_bouboule_opcode.txt -opcode:opcode $i ./xlock -inwindow -nolock -delay 0 -mode bouboule
    echo "xlock opcode $i $((($(date +%s%N) - $ts)/1000000))ms" >> ../../../timing_opcode.txt
done
for ((i = 8 ; i < 65536 ; i*=2));
do
    ts=$(date +%s%N)
    ../../../simplesim-3.0/sim-opcode -opcode bimod -redir:sim ../../../results/xlock_bouboule_bimod.txt -opcode:bimod $i ./xlock -inwindow -nolock -delay 0 -mode bouboule
    echo "xlock bimod $i $((($(date +%s%N) - $ts)/1000000))ms" >> ../../../timing_opcode.txt
done

for ((i = 8 ; i < 65536 ; i*=2));
do
    ts=$(date +%s%N)
    ../../../simplesim-3.0/sim-opcode -opcode opcodecomb -redir:sim ../../../results/xlock_bouboule_opcodecomb.txt -opcode:opcodecomb $i ./xlock -inwindow -nolock -delay 0 -mode bouboule
    echo "xlock opcodecomb $i $((($(date +%s%N) - $ts)/1000000))ms" >> ../../../timing_opcode.txt
done
for ((i = 8 ; i < 65536 ; i*=2));
do
    ts=$(date +%s%N)
    ../../../simplesim-3.0/sim-opcode -opcode comb -redir:sim ../../../results/xlock_bouboule_comb.txt -opcode:comb $i ./xlock -inwindow -nolock -delay 0 -mode bouboule
    echo "xlock comb $i $((($(date +%s%N) - $ts)/1000000))ms" >> ../../../timing_opcode.txt
done
cd ../..

cd ..

cd simplesim-3.0/
make clean
make config-pisa
make
/usr/bin/gcc `./sysprobe -flags` -DDEBUG -O0 -g -Wall   -c sim-opcode.c
/usr/bin/gcc -o sim-opcode `./sysprobe -flags` -DDEBUG -O0 -g -Wall   sim-opcode.o opcode.o main.o syscall.o memory.o regs.o loader.o endian.o dlite.o symbol.o eval.o options.o stats.o eio.o range.o misc.o machine.o libexo/libexo.a `./sysprobe -libs` -lm
cd ..

cd ./spec95-little/

for ((i = 8 ; i < 65536 ; i*=2));
do
    ts=$(date +%s%N)
    ../simplesim-3.0/sim-opcode -opcode opcode -redir:sim ../results/cc1_opcode.txt -opcode:opcode $i cc1.ss ../inputs/cc1.in
    echo "cc1 opcode $i $((($(date +%s%N) - $ts)/1000000))ms" >> ../../../timing_opcode.txt
done
for ((i = 8 ; i < 65536 ; i*=2));
do
    ts=$(date +%s%N)
    ../simplesim-3.0/sim-opcode -opcode bimod -redir:sim ../results/cc1_bimod.txt -opcode:bimod $i cc1.ss ../inputs/cc1.in
    echo "cc1 bimod $i $((($(date +%s%N) - $ts)/1000000))ms" >> ../../../timing_opcode.txt
done

for ((i = 8 ; i < 65536 ; i*=2));
do
    ts=$(date +%s%N)
    ../simplesim-3.0/sim-opcode -opcode opcodecomb -redir:sim ../results/cc1_opcodecomb.txt -opcode:opcodecomb $i cc1.ss ../inputs/cc1.in
    echo "cc1 opcodecomb $i $((($(date +%s%N) - $ts)/1000000))ms" >> ../../../timing_opcode.txt
done
for ((i = 8 ; i < 65536 ; i*=2));
do
    ts=$(date +%s%N)
    ../simplesim-3.0/sim-opcode -opcode comb -redir:sim ../results/cc1_comb.txt -opcode:comb $i cc1.ss ../inputs/cc1.in
    echo "cc1 comb $i $((($(date +%s%N) - $ts)/1000000))ms" >> ../../../timing_opcode.txt
done

# for ((i = 8 ; i < 65536 ; i*=2));
# do
#     ../simplesim-3.0/sim-opcode -opcode opcode -redir:sim ../results/compress95_opcode.txt -opcode:opcode $i compress95.ss ../inputs/compress95.in
# done
# for ((i = 8 ; i < 65536 ; i*=2));
# do
#     ../simplesim-3.0/sim-opcode -opcode bimod -redir:sim ../results/compress95_bimod.txt -opcode:bimod $i compress95.ss ../inputs/compress95.in
# done

# for ((i = 8 ; i < 65536 ; i*=2));
# do
#     ../simplesim-3.0/sim-opcode -opcode opcodecomb -redir:sim ../results/compress95_opcodecomb.txt -opcode:opcodecomb $i compress95.ss ../inputs/compress95.in
# done
# for ((i = 8 ; i < 65536 ; i*=2));
# do
#     ../simplesim-3.0/sim-opcode -opcode comb -redir:sim ../results/compress95_comb.txt -opcode:comb $i compress95.ss ../inputs/compress95.in
# done



# for ((i = 8 ; i < 65536 ; i*=2));
# do
#     ../simplesim-3.0/sim-opcode -opcode opcode -redir:sim ../results/go_opcode.txt -opcode:opcode $i go.ss ../inputs/go.in
# done
# for ((i = 8 ; i < 65536 ; i*=2));
# do
#     ../simplesim-3.0/sim-opcode -opcode bimod -redir:sim ../results/go_bimod.txt -opcode:bimod $i go.ss ../inputs/go.in
# done

# for ((i = 8 ; i < 65536 ; i*=2));
# do
#     ../simplesim-3.0/sim-opcode -opcode opcodecomb -redir:sim ../results/go_opcodecomb.txt -opcode:opcodecomb $i go.ss ../inputs/go.in
# done
# for ((i = 8 ; i < 65536 ; i*=2));
# do
#     ../simplesim-3.0/sim-opcode -opcode comb -redir:sim ../results/go_comb.txt -opcode:comb $i go.ss ../inputs/go.in
# done



for ((i = 8 ; i < 65536 ; i*=2));
do
    ts=$(date +%s%N)
    ../simplesim-3.0/sim-opcode -opcode opcode -redir:sim ../results/perl_opcode.txt -opcode:opcode $i perl.ss ../inputs/perl.in
    echo "perl opcode $i $((($(date +%s%N) - $ts)/1000000))ms" >> ../../../timing_opcode.txt
done
for ((i = 8 ; i < 65536 ; i*=2));
do
    ts=$(date +%s%N)
    ../simplesim-3.0/sim-opcode -opcode bimod -redir:sim ../results/perl_bimod.txt -opcode:bimod $i perl.ss ../inputs/perl.in
    echo "perl bimod $i $((($(date +%s%N) - $ts)/1000000))ms" >> ../../../timing_opcode.txt
done

for ((i = 8 ; i < 65536 ; i*=2));
do
    ts=$(date +%s%N)
    ../simplesim-3.0/sim-opcode -opcode opcodecomb -redir:sim ../results/perl_opcodecomb.txt -opcode:opcodecomb $i perl.ss ../inputs/perl.in
    echo "perl opcodecomb $i $((($(date +%s%N) - $ts)/1000000))ms" >> ../../../timing_opcode.txt
done
for ((i = 8 ; i < 65536 ; i*=2));
do
    ts=$(date +%s%N)
    ../simplesim-3.0/sim-opcode -opcode comb -redir:sim ../results/perl_comb.txt -opcode:comb $i perl.ss ../inputs/perl.in
    echo "perl comb $i $((($(date +%s%N) - $ts)/1000000))ms" >> ../../../timing_opcode.txt
done

cd ..


