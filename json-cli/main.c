#include <libgen.h>
#include <ctype.h>
#include <time.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdarg.h>
#include <string.h>
#include <libgen.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <assert.h>



struct termios tp;
int console = 0;
int sig_type = 0;
int ignore = 0;

static void die (const char *format, ...)
{
	va_list args;
	va_start (args, format);
	vfprintf (stderr, format, args);
	va_end (args);
	fprintf (stderr, "\n");
	exit (1);
}

int get_console(void)
{
	int console = 0;

	if(isatty(0))
	{
		console = ! ((strcmp(ttyname(0), "/dev/ttyS0")) && (strcmp(ttyname(0), "/dev/console")));
	}

	return console;
}

void setalarm(int n, int signal_type)
{
	sig_type = signal_type;

	if(!console)
	{
		alarm(n);
	}
}

void hd_alarm()
{
	switch(sig_type)
	{
		case 1:
			if(isatty(0)) tcsetattr(0, TCSANOW, &tp);
			fprintf(stderr,"\n");
			fprintf(stderr,"CLI timed out after 30 minutes..\n");
			exit(0);
			break;
		case 2:
			if(isatty(0)) tcsetattr(0, TCSANOW, &tp);
			fprintf(stderr,"\ntimed out..\n");
			if(!console)
			{
				exit(0);
			} 
			break;
	}
}

static char *progname;

int main(int argc, char *argv[])
{
	char *cmd = NULL;
	char remote_host[256] = {0};
	int opt,has_server = 0 ;
	int c_flag = 0;
	int cmd_length = 0;
	FILE *fp = NULL;
	int yyrv = 0;

	console = get_console();
	setlinebuf(stdout);
	progname = basename(argv[0]);

	while ((opt = getopt(argc, argv, "cif:s:h")) > 0)
	{
		switch (opt)
		{
			case 'c':
				c_flag = 1;
				break;

			case 'i':
				ignore = 1;
				break;

			case 'f':
				fp = fopen(optarg, "r");
				if (fp == NULL)
				{
					fprintf(stderr, "Cannot open file: %s\n", optarg);
					exit(1);
				}
				break;

			case 's':
				if(sprintf(remote_host,"%s",optarg))
					has_server = 1;
				break;

			case 'h':
			default:
				die("Usage: %s [-f <file>] [-c cmd] [-s server]\n", progname);
				break;
		}
	}

	if(!has_server)
	{
		sprintf(remote_host,"127.0.0.1");
		//printf ("No server specified,127.0.0.1 \n");
	}


	if(isatty(0)){ 
		tcgetattr(0, &tp);
	}
	
	signal(SIGHUP, SIG_DFL);
	signal(SIGINT, SIG_IGN);
	signal(SIGALRM, (void *)hd_alarm);



	if(( c_flag == 1) && (optind < argc) )
	{
		int i;
		for(i = optind; i < argc; i++)
		{
			cmd_length += strlen(argv[i]);
		}

		cmd_length += argc - optind + 1;

		cmd = (char *)calloc(cmd_length, sizeof(char));

		for(i = optind; i < argc; i++)
		{
			strcat(cmd,argv[i]);
			if(i == argc - 1)
			{
				//strcat(cmd,"\n");
			}
			else
			{
				strcat(cmd," ");
			}
		}

	}else if(c_flag == 1)
		die("Usage: %s [-f <file>] [-c cmd] [-s server]\n", progname);

	start_cli(cmd, fp);
	return 0;
}
