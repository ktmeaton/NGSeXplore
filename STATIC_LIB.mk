g++ -I include -c include/ -c include/FastQ.cpp -o build/FastQ.o
g++ -I include -c include/ -c include/Patch.cpp -o build/Patch.o
g++ -I include -c include/ -c include/ProgressLog.cpp -o build/ProgressLog.o
g++ -I include -c include/ -c include/TextColor.cpp -o build/TextColor.o

ar rcs lib/libFastQProc.a build/FastQ.o build/Patch.o build/ProgressLog.o build/TextColor.o

g++ -I include -L lib -static src/modules/FastQStats.cpp -lFastQProc -o bin/FastQStats
