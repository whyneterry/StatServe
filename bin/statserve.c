#include "net.h"

static volatile sig_atomic_t ctrl_c_flag = 0;

void signal_handler(int sig) {
	if (sig == SIGINT) {
		ctrl_c_flag = 1;
	}
}

int main(int argc, char *argv[])
{
	int rc = -1;
	int this_rc = -1;
	int cl_sock_fd = 0;
	int serv_sock_fd = 0;
	int set = 1;

	struct sockaddr_in *serv_addr = NULL;
	struct ClientThread *threads = NULL;
	BSTree *map = NULL;
	SharedData *s_data = NULL;

	check(argc > 1, "Invalid usage.\n"
					"USAGE: statserve <DB_path>\n"
					"<DB_path> should be a directory\n");

	s_data = calloc(1, sizeof(SharedData));
	check(s_data != NULL, "Error with calloc memory for SharedData.");

	map = BSTree_create(NULL);
	check(map != NULL, "Failed to create BSTree.");

	s_data->map = map;
	s_data->db_path = bfromcstr(argv[1]);

	rc = pthread_mutex_init(&s_data->mutex, NULL);
	check(rc == 0, "Error with pthread_mutex_init");

	int thread_count = 0;
	int max_threads = 100;
	threads = calloc(max_threads, sizeof(struct ClientThread));
	check(threads != NULL, "Error with calloc memory for thread.");

	serv_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	check(serv_sock_fd != -1, "Error with getting serv_sock_fd.");
	rc = setsockopt(serv_sock_fd, SOL_SOCKET, SO_REUSEADDR, &set, sizeof(int));
	check(rc == 0, "Error with setsockopt.");

	serv_addr = createAddr("127.0.0.1", 7777);
 	check(serv_addr != NULL, "Error with creating serv_addr.");

	rc = bind(serv_sock_fd, (struct sockaddr *)serv_addr, sizeof(struct sockaddr));
	check(rc == 0, "Error with binding serv_sock_fd to serv_addr.");

	rc = listen(serv_sock_fd, BACKLOGN);
	check(rc == 0, "Error with starting listening on serv_sock_fd.");

	struct sockaddr_in cl_addr;
	socklen_t cl_addr_size = sizeof(struct sockaddr);
	
	struct sigaction act;
	act.sa_handler = signal_handler;
	sigaction(SIGINT, &act, NULL);

	while (!ctrl_c_flag) {
		debug("Accepting connection...");
		cl_sock_fd = accept(serv_sock_fd, (struct sockaddr *)&cl_addr, &cl_addr_size);
		check(cl_sock_fd != -1, "Error with accepting connection.");
		debug("Client connected %d.", cl_sock_fd);

		int thread_slot = -1;
		for (int j = 0; j < max_threads; ++j) {
			if (threads[j].cl_sock_fd == 0) {
				thread_slot = j;
				break;
			}
		}

		if (thread_slot == -1) {
			debug("Max client reached. Connection aborted.");
			close(cl_sock_fd);
			continue;
		}

		threads[thread_slot].cl_sock_fd = cl_sock_fd;
		threads[thread_slot].s_data = s_data;

		rc = pthread_create(&threads[thread_slot].thread_id, NULL, client_handler, &threads[thread_slot]);
		check(rc == 0, "Error with creating a pthread.");

		//pthread_detach(threads[thread_slot].thread_id);		
	
		++thread_count;
		printf("Active clients: %d\n", thread_count);	
	}

	for (int i = 0; i < max_threads; ++i) {
		if (threads[i].cl_sock_fd != 0) {
			pthread_join(threads[i].thread_id, NULL);
		}
	}

	this_rc = 0;

error: //fallthrough
	if (serv_addr) free(serv_addr);
	if (threads) free(threads);
	if (map) BSTree_destroy(map);
	if (s_data) free(s_data);
	if (serv_sock_fd) close(serv_sock_fd);
	if (cl_sock_fd) close(cl_sock_fd);

	return this_rc;
}
