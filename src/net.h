#ifndef _NET_H
#define _NET_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include <openssl/md5.h>
#include "lcthw/bstrlib.h"
#include "lcthw/ringbuffer.h"
#include "lcthw/dbg.h"
#include "lcthw/bstree.h"
#include "lcthw/stats.h"

#define BACKLOGN 10
#define MSGSIZE 1024

typedef struct SharedData {
	bstring db_path;
	BSTree *map;
	pthread_mutex_t mutex; 
} SharedData;

struct ClientThread {
	int cl_sock_fd;
	SharedData *s_data;
	pthread_t thread_id;
};

typedef struct Command {
	SharedData *s_data; // holds data structure 
	struct bstrList *urls; // holds keys
	bstring data; // holds values
	int cl_sock_fd;
	bstring db_path;
	int is_sock; // for tests
	bstring binfo; // message to send to the client
} Command;

struct sockaddr_in *createAddr(char *ip, int port);

int read_data(int fd, RingBuffer *buffer, int is_sock);

int send_msg(int cl_sock_fd, bstring data, int is_sock);

void send_err(int cl_sock_fd, char *msg, int is_sock);

void *client_handler(void *arg);

int parse_request(SharedData *s_data, struct bstrList *list, int cl_sock_fd, bstring db_path, int is_sock);

int sample_stats(Command *cmd);

int create_stats(Command *cmd);

int dump_stats(Command *cmd);

int mean_stats(Command *cmd);

int delete_stats(Command *cmd);

int store_stats(Command *cmd);

int load_stats(Command *cmd);

bstring md5hash(bstring key, int outlen);

#endif
