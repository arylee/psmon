.SUFFIXES: .o .cpp

GCC=g++
COPY=cp
REMOVE=rm -rf
CCLIB=-lpthread
GCCFLAGS=-O3 -Wall -mtune=core2 -msse4.2 -pthread -g

all_exes=$(psmon_exe) $(pscli_exe)

psmon_obj=ConfigFile.o Configure.o Logger.o Utils.o \
	Socket.o SocketUnix.o BaseDaemon.o PsmonDaemon.o \
	ProcessManager.o WorkerProcess.o PSMON.o

psmon_exe=psmon

pscli_obj=ConfigFile.o Configure.o Logger.o Utils.o \
	Socket.o SocketUnix.o PSCLI.o

pscli_exe=pscli

.cpp.o:
	$(GCC) -c $(GCCFLAGS) $<

all:psmon pscli

clear:
	$(REMOVE) *~ *.o

clean:
	$(REMOVE) $(all_exes) *.o

install:
	$(COPY) $(all_exes) ../bin

psmon: $(psmon_obj)
	$(GCC) -o $(@) $(psmon_obj) $(CCLIB)

pscli: $(pscli_obj)
	$(GCC) -o $(@) $(pscli_obj) $(CCLIB)
