#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#include "fsearch.h"
#include "gui.h"

char* file;
char buf[MAX_BUF_SIZE];

int thread_count;
int search_complete;
int files_checked;
int all_threads_created;


pthread_mutex_t path_lock;
pthread_mutex_t files_chkd_lock;
pthread_mutex_t printf_lock;
pthread_cond_t path_taken;

int main(int argc, char* argv[])
{
	struct itimerval timer;
	struct sigaction sa;
	printf("fsearch: multi-threaded and/or multi-process search for a file on the local filesystem\n");

	if(USING_MPI == 1)
	{
		dprint("Using MPI\n");
	}
	else
	{
		dprint("Single Process using pthread\n");
		//Init pthread
	}

	file = argv[1];
	if(file == NULL)
	{
		printf("ERROR: Need to specify file to search for.\n");
		exit(0);
	}
	printf("[-] Searching for %s\n", file);

	fsearch_opt_t opts = { DEFAULT_NUM_PROCS, DEFAULT_NUM_THREADS, 0, ROOT_DIR };
	parse_cmdline(argc, argv, &opts);
	dprint("Running fsearch with %d Processes and %d Threads\nStarting Search at %s\n", opts.num_procs, opts.num_threads, opts.start_loc);

	thread_count = 0;
	search_complete = 0;
	all_threads_created = 0;
	pthread_mutex_init(&path_lock, NULL);
	pthread_mutex_init(&printf_lock, NULL);
	pthread_mutex_init(&files_chkd_lock, NULL);
	pthread_cond_init(&path_taken, NULL);


	setup_timer(&timer, &sa);
	files_checked = 0;

	read_dir(&opts);

	printf("\nEnd of Processing.\n");
	return 0;
}

void parse_cmdline(int argc, char* argv[], fsearch_opt_t* opts)
{
	int opt;
	const char* optstring = "dgp:s:t:";

	opterr = 0;
	opt = getopt(argc, argv, optstring);
	while(opt != -1)
	{
		switch(opt)
		{
			case 'd':
				printf("DEBUG Mode Enabled.\n");
				break;
			case 'g':
				create_gui();
				break;
			case 'p':
				if(USING_MPI == 1)
				{
					opts->num_procs = atoi(optarg);
				}
				else
				{
					printf("Cannot Set Number of Processes if not Compiling with MPI.\nTo Compile with MPI run 'make mpi'\n");
				}
				break;
			case 's':
				opts->start_loc = optarg;
				dprint("[+] Set fsearch opts start_loc -> %s\n", opts->start_loc);
				break;
			default:
				printf("Unknown Command Line Argument: %c\n", opt);
				break;
		}
		opt = getopt(argc, argv, optstring);
	}
}

void read_dir(fsearch_opt_t* opts)
{
	DIR* dir;
	char* root;
	char path[64];
	struct dirent* entry;

	root = opts->start_loc;
	dir = opendir(root);

	if(strcmp(root, "/") != 0)
	{
		strcat(root, "/");
	}

	pthread_t thread_pool[opts->num_threads];

	while( (entry = readdir(dir)) != NULL )
	{
		strcpy(path, root);
		strcat(path, entry->d_name);
		pthread_create(&thread_pool[thread_count], NULL, start_thread, (void*)path);
		thread_count++;
		pthread_cond_wait(&path_taken, &path_lock);
	}

	all_threads_created = 1;

	closedir(dir);
	if(entry != NULL)
	{
		free(entry);
	}

	for(int i = 0; i < thread_count; i++)
	{
		pthread_join(thread_pool[i], NULL);
	}

	pthread_mutex_lock(&printf_lock);
	printf("\n[+] Search Complete\n[+] %d Files Checked", files_checked);
	pthread_mutex_unlock(&printf_lock);

}

void* start_thread(void* arg)
{
	pthread_mutex_lock(&path_lock);

	char* path = (char*)arg;
	char* _path = malloc((sizeof(char) * strlen(path)) + 1);

	strcpy(_path, path);

	dprint("[+] Creating Thread For File: %s\n", _path);

	pthread_cond_signal(&path_taken);
	pthread_mutex_unlock(&path_lock);


	dprint("[-] Path Taken Signalled\n");

	while(all_threads_created == 0);

	search_dir(_path);

	free(_path);
	pthread_exit(0);

	return NULL;
}

void search_dir(char* path)
{
	pthread_t thread_id = pthread_self();

	if(path == NULL)
	{
		dprint("[x] Search Path NULL\n");
		return;
	}
	if(is_dot(path) == 1)
	{
		dprint("[x] Not Searching /.\n");
		return;
	}
	DIR* dir = opendir(path);
	struct dirent* entry;

	if(dir == NULL)
	{
		dprint("[x] ERROR: Do not have permission to open: %s [THREAD %lu]\n", path, thread_id);
		closedir(dir);
		return;
	}

	char* filename;
	char buf[MAX_BUF_SIZE];

	dprint("[-] search_dir(): Directory opened: %s\n", path);

	errno = 0;
	while( (entry = readdir(dir)) != NULL )
	{
		if(errno != 0)
		{
			dprint("[x] ERROR: readdir() encountered error #%d [THREAD %lu]\n", errno, thread_id);
			break;
		}
		filename = entry->d_name;
		dprint("[-] File Entry: %s\n", filename);
		files_checked++;

		if( compare(filename) == 0 )
		{
			pthread_mutex_lock(&printf_lock);
			printf(GRN "[!] File Found%s%c%s\n" RESET, path, '/', filename);
			pthread_mutex_unlock(&printf_lock);
		}
		if(entry->d_type == DT_DIR)
		{
			set_buf(buf, path, filename);
			search_dir(buf);
		}
	}

	//Clean up
	closedir(dir);
	if(entry != NULL)
	{
		free(entry);
	}
}

int is_dot(char* path)
{
	int len = strlen(path);
	int dot = 0;
	if(len > 0)
	{
		if(path[len - 1] == '.')
		{
			dot = 1;
		}
	}
	return dot;
}

int compare(char* filename)
{
	return strcmp(filename, file);
}

void dprint(const char* str, ...)
{
	if(DEBUG_ON == 1)
	{
		va_list args;
   		va_start(args, str);
   		pthread_mutex_lock(&printf_lock);
    	vprintf(str, args);
    	pthread_mutex_unlock(&printf_lock);
    	va_end(args);
    }
}

void timer_handler(int signum)
{
	printf("\r[+] %d Files Checked", files_checked);
}

void setup_timer(struct itimerval* timer, struct sigaction* sa)
{
	memset(sa, 0, sizeof(*sa));
	sa->sa_handler = timer_handler;
	sigaction(SIGALRM, sa, NULL);
	
	 timer->it_value.tv_sec = 0;
	 timer->it_value.tv_usec = 5000;
	 /* ... and every 250 msec after that. */
	 timer->it_interval.tv_sec = 0;
	 timer->it_interval.tv_usec = 5000;
	 setitimer (ITIMER_REAL, timer, NULL);
}

void set_buf(char* buf, char* path, char* filename)
{
	memset(buf, 0, MAX_BUF_SIZE);
	strncpy(buf, path, strlen(path));
	strncat(buf, "/", 1);
	strncat(buf, filename, strlen(filename));
}