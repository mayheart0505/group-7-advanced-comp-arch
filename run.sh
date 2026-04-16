rm ./results/*

cd simplesim-3.0/
make clean
make config-alpha
make
/usr/bin/gcc `./sysprobe -flags` -DDEBUG -O0 -g -Wall   -c sim-bpred.c
/usr/bin/gcc -o sim-bpred `./sysprobe -flags` -DDEBUG -O0 -g -Wall   sim-bpred.o bpred.o main.o syscall.o memory.o regs.o loader.o endian.o dlite.o symbol.o eval.o options.o stats.o eio.o range.o misc.o machine.o libexo/libexo.a `./sysprobe -libs` -lm

cd ../xbenchmarks/povray/exe
for ((i = 8 ; i < 65536 ; i*=2));
do
    ../../../simplesim-3.0/sim-bpred -bpred opcode -redir:sim ../../../results/povray_opcode.txt -bpred:opcode $i ./x-povray povray.ini -W320 -H200 -F +D -Q4 +I showoff/mist.pov 
done
for ((i = 8 ; i < 65536 ; i*=2));
do
    ../../../simplesim-3.0/sim-bpred -bpred bimod -redir:sim ../../../results/povray_bimod.txt -bpred:bimod $i ./x-povray povray.ini -W320 -H200 -F +D -Q4 +I showoff/mist.pov 
done
for ((i = 8 ; i < 65536 ; i*=2));
do
    ../../../simplesim-3.0/sim-bpred -bpred opcodecomb -redir:sim ../../../results/povray_opcodecomb.txt -bpred:opcodecomb $i ./x-povray povray.ini -W320 -H200 -F +D -Q4 +I showoff/mist.pov 
done
for ((i = 8 ; i < 65536 ; i*=2));
do
    ../../../simplesim-3.0/sim-bpred -bpred comb -redir:sim ../../../results/povray_comb.txt -bpred:comb $i ./x-povray povray.ini -W320 -H200 -F +D -Q4 +I showoff/mist.pov 
done
cd ../..


cd xdoom/exe
for ((i = 8 ; i < 65536 ; i*=2));
do
    ../../../simplesim-3.0/sim-bpred -bpred opcode -redir:sim ../../../results/xdoom_opcode.txt -bpred:opcode $i ./alphadoom -playdemo rockin
done
for ((i = 8 ; i < 65536 ; i*=2));
do
    ../../../simplesim-3.0/sim-bpred -bpred bimod -redir:sim ../../../results/xdoom_bimod.txt -bpred:bimod $i ./alphadoom -playdemo rockin
done
for ((i = 8 ; i < 65536 ; i*=2));
do
    ../../../simplesim-3.0/sim-bpred -bpred opcodecomb -redir:sim ../../../results/xdoom_opcodecomb.txt -bpred:opcodecomb $i ./alphadoom -playdemo rockin
done
for ((i = 8 ; i < 65536 ; i*=2));
do
    ../../../simplesim-3.0/sim-bpred -bpred comb -redir:sim ../../../results/xdoom_comb.txt -bpred:comb $i ./alphadoom -playdemo rockin
done
cd ../..


cd xlock/exe
for ((i = 8 ; i < 65536 ; i*=2));
do
    ../../../simplesim-3.0/sim-bpred -bpred opcode -redir:sim ../../../results/xlock_bouboule_opcode.txt -bpred:opcode $i ./xlock -inwindow -nolock -delay 0 -mode bouboule
done
for ((i = 8 ; i < 65536 ; i*=2));
do
    ../../../simplesim-3.0/sim-bpred -bpred bimod -redir:sim ../../../results/xlock_bouboule_bimod.txt -bpred:bimod $i ./xlock -inwindow -nolock -delay 0 -mode bouboule
done

for ((i = 8 ; i < 65536 ; i*=2));
do
    ../../../simplesim-3.0/sim-bpred -bpred opcodecomb -redir:sim ../../../results/xlock_bouboule_opcodecomb.txt -bpred:opcodecomb $i ./xlock -inwindow -nolock -delay 0 -mode bouboule
done
for ((i = 8 ; i < 65536 ; i*=2));
do
    ../../../simplesim-3.0/sim-bpred -bpred comb -redir:sim ../../../results/xlock_bouboule_comb.txt -bpred:comb $i ./xlock -inwindow -nolock -delay 0 -mode bouboule
done
cd ../..

cd ..

cd simplesim-3.0/
make clean
make config-pisa
make
/usr/bin/gcc `./sysprobe -flags` -DDEBUG -O0 -g -Wall   -c sim-bpred.c
/usr/bin/gcc -o sim-bpred `./sysprobe -flags` -DDEBUG -O0 -g -Wall   sim-bpred.o bpred.o main.o syscall.o memory.o regs.o loader.o endian.o dlite.o symbol.o eval.o options.o stats.o eio.o range.o misc.o machine.o libexo/libexo.a `./sysprobe -libs` -lm
cd ..

cd ./spec95-little/

for ((i = 8 ; i < 65536 ; i*=2));
do
    ../simplesim-3.0/sim-bpred -bpred opcode -redir:sim ../results/cc1_opcode.txt -bpred:opcode $i cc1.ss ../inputs/cc1.in
done
for ((i = 8 ; i < 65536 ; i*=2));
do
    ../simplesim-3.0/sim-bpred -bpred bimod -redir:sim ../results/cc1_bimod.txt -bpred:bimod $i cc1.ss ../inputs/cc1.in
done

for ((i = 8 ; i < 65536 ; i*=2));
do
    ../simplesim-3.0/sim-bpred -bpred opcodecomb -redir:sim ../results/cc1_opcodecomb.txt -bpred:opcodecomb $i cc1.ss ../inputs/cc1.in
done
for ((i = 8 ; i < 65536 ; i*=2));
do
    ../simplesim-3.0/sim-bpred -bpred comb -redir:sim ../results/cc1_comb.txt -bpred:comb $i cc1.ss ../inputs/cc1.in
done

# for ((i = 8 ; i < 65536 ; i*=2));
# do
#     ../simplesim-3.0/sim-bpred -bpred opcode -redir:sim ../results/compress95_opcode.txt -bpred:opcode $i compress95.ss ../inputs/compress95.in
# done
# for ((i = 8 ; i < 65536 ; i*=2));
# do
#     ../simplesim-3.0/sim-bpred -bpred bimod -redir:sim ../results/compress95_bimod.txt -bpred:bimod $i compress95.ss ../inputs/compress95.in
# done

# for ((i = 8 ; i < 65536 ; i*=2));
# do
#     ../simplesim-3.0/sim-bpred -bpred opcodecomb -redir:sim ../results/compress95_opcodecomb.txt -bpred:opcodecomb $i compress95.ss ../inputs/compress95.in
# done
# for ((i = 8 ; i < 65536 ; i*=2));
# do
#     ../simplesim-3.0/sim-bpred -bpred comb -redir:sim ../results/compress95_comb.txt -bpred:comb $i compress95.ss ../inputs/compress95.in
# done



# for ((i = 8 ; i < 65536 ; i*=2));
# do
#     ../simplesim-3.0/sim-bpred -bpred opcode -redir:sim ../results/go_opcode.txt -bpred:opcode $i go.ss ../inputs/go.in
# done
# for ((i = 8 ; i < 65536 ; i*=2));
# do
#     ../simplesim-3.0/sim-bpred -bpred bimod -redir:sim ../results/go_bimod.txt -bpred:bimod $i go.ss ../inputs/go.in
# done

# for ((i = 8 ; i < 65536 ; i*=2));
# do
#     ../simplesim-3.0/sim-bpred -bpred opcodecomb -redir:sim ../results/go_opcodecomb.txt -bpred:opcodecomb $i go.ss ../inputs/go.in
# done
# for ((i = 8 ; i < 65536 ; i*=2));
# do
#     ../simplesim-3.0/sim-bpred -bpred comb -redir:sim ../results/go_comb.txt -bpred:comb $i go.ss ../inputs/go.in
# done



for ((i = 8 ; i < 65536 ; i*=2));
do
    ../simplesim-3.0/sim-bpred -bpred opcode -redir:sim ../results/perl_opcode.txt -bpred:opcode $i perl.ss ../inputs/perl.in
done
for ((i = 8 ; i < 65536 ; i*=2));
do
    ../simplesim-3.0/sim-bpred -bpred bimod -redir:sim ../results/perl_bimod.txt -bpred:bimod $i perl.ss ../inputs/perl.in
done

for ((i = 8 ; i < 65536 ; i*=2));
do
    ../simplesim-3.0/sim-bpred -bpred opcodecomb -redir:sim ../results/perl_opcodecomb.txt -bpred:opcodecomb $i perl.ss ../inputs/perl.in
done
for ((i = 8 ; i < 65536 ; i*=2));
do
    ../simplesim-3.0/sim-bpred -bpred comb -redir:sim ../results/perl_comb.txt -bpred:comb $i perl.ss ../inputs/perl.in
done

cd ../..