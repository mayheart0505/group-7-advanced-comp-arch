rm ./results/povray_bimod.txt
rm ./results/povray_opcode.txt
rm ./results/xdoom_bimod.txt
rm ./results/xdoom_opcode.txt

cd simplesim-3.0/
# make clean
make config-alpha
# make
/usr/bin/gcc `./sysprobe -flags` -DDEBUG -O0 -g -Wall   -c sim-bpred.c
/usr/bin/gcc -o sim-bpred `./sysprobe -flags` -DDEBUG -O0 -g -Wall   sim-bpred.o bpred.o main.o syscall.o memory.o regs.o loader.o endian.o dlite.o symbol.o eval.o options.o stats.o eio.o range.o misc.o machine.o libexo/libexo.a `./sysprobe -libs` -lm

cd ../xbenchmarks/povray/exe
for ((i = 1 ; i < 16384 ; i*=2));
do
    ../../../simplesim-3.0/sim-bpred -bpred opcode -redir:sim ../../../results/povray_opcode.txt -bpred:opcode $i ./x-povray povray.ini -W320 -H200 -F +D -Q4 +I showoff/mist.pov 
done
for ((i = 1 ; i < 16384 ; i*=2));
do
    ../../../simplesim-3.0/sim-bpred -bpred bimod -redir:sim ../../../results/povray_bimod.txt -bpred:bimod $i ./x-povray povray.ini -W320 -H200 -F +D -Q4 +I showoff/mist.pov 
done
cd ../..


cd xdoom/exe
for ((i = 1 ; i < 16384 ; i*=2));
do
    ../../../simplesim-3.0/sim-bpred -bpred opcode -redir:sim ../../../results/xdoom_opcode.txt -bpred:opcode $i ./alphadoom -playdemo rockin
done
for ((i = 1 ; i < 16384 ; i*=2));
do
    ../../../simplesim-3.0/sim-bpred -bpred bimod -redir:sim ../../../results/xdoom_bimod.txt -bpred:bimod $i ./alphadoom -playdemo rockin
done
cd ../..

