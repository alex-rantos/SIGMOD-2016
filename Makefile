#Compiler settings

CC = g++
CFLAGS = -c -Wall -g -std=gnu++11 -O2
SRCS = main.cpp buffer.cpp components.cpp job_scheduler.cpp
OBJS = $(SRCS:.cpp=.o)
EXE = exec
LIBS = -pthread
SCRIPT = insert_unitest.script
OUTPUT_FILE = results.txt

all : $(SRCS) $(EXE)
	@echo $(EXE) "HAS BEEN COMPILED"

$(EXE): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LIBS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

script :
	chmod +x $(SCRIPT)

clean :
	@echo "Removing files"
	rm $(EXE) $(OUTPUT_FILE) *.o

rebuild :
	make clean
	make

cleanTXT :
	rm REVERSEDSorted.txt newSorted.txt graphINCOMING_REVERSED.txt graphOUTCOMING.txt results.txt
