#ifndef FSEARCH_H
#define FSEARCH_H

#ifdef USE_MPI
#include <mpi.h>
#define DEFAULT_NUM_PROCS 4
#define USING_MPI 1
#else
#define DEFAULT_NUM_PROCS 1
#define USING_MPI 0
#endif

#ifdef DEBUG
#define DEBUG_ON 1
#else
#define DEBUG_ON 0
#endif

#define DEFAULT_NUM_THREADS 64
#define MAX_BUF_SIZE 300
#define ROOT_DIR "/"

//stdout color codes
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

extern int opterr;

//Structs
typedef struct fsearch_opt {
	int num_procs;
	int num_threads;
	int debug;
	char* start_loc;
} fsearch_opt_t;



//Function prototypes
void parse_cmdline(int argc, char* argv[], fsearch_opt_t* opts);
void* start_thread(void* arg);
int compare(char* filename);
void read_dir(fsearch_opt_t* opts);
void search_dir(char* path);
int is_dot(char* path);
void dprint(const char* str, ...);
void timer_handler(int signum);
void setup_timer(struct itimerval* timer, struct sigaction* sa);
void set_buf(char* buf, char* path, char* filename);




#endif