EXECUTABLES : out   
CFLAGS  = -g
LDFLAGS = -g

.c.o:
	$(CC) $(CFLAGS) -c $<

json: script/write_json.py
	python script/write_json.py 

h: script/write_rpc.h.py
	python script/write_rpc.h.py 

rpc: script/write_cli_rpc.c.py
	python script/write_cli_rpc.c.py 

out: main.o readline.o cli_rpc.o cJSON.o re2val.o dispatcher.o 
	$(CC) $(LDFLAGS) -o $@ main.o readline.o cJSON.o re2val.o cli_rpc.o dispatcher.o  -lm  -lreadline -lncurses 

check: check.o cJSON.o
	$(CC) $(LDFLAGS) -o $@ check.o  cJSON.o -lm

clean:
	$(RM) *.o
	$(RM) out 

clear:
	$(RM) *.o
	$(RM) out include/rpc*.h cli_rpc.c file/json
