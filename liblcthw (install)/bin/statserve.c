#include <stdlib.h>
#include <sys/select.h>
#include <stdio.h>
#include <lcthw/ringbuffer.h>
#include <lcthw/dbg.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>

int read_data(int fd, RingBuffer *buffer)
{
	int rc = 0;

 	rc = recv(fd, RingBuffer_starts_at(buffer), RingBuffer_available_space(buffer), 0);
	check(rc >= 0, "Error with recieve data.");	

	RingBuffer_commit_write(buffer, rc);
	
	return rc;

error:
	return -1;
}

int write_data(int fd, bstring data)
{
	int rc = 0;

	//bstring data = RingBuffer_get_all(buffer);
	
	rc = send(fd, bdata(data), blength(data), 0);
	check(rc == blength(data), "Error with sending data.");
	//bdestroy(data);

	return rc;

error:
	return -1;
}

struct sockaddr_in *createAddr(char *ip, int port)
{	
	struct sockaddr_in *addr = calloc(1, sizeof(struct sockaddr_in));
	check(addr != NULL, "Error with allocate memory for address: [IP: %s, Port: %d].", ip, port);
	
	addr->sin_family = AF_INET;
	addr->sin_port = htons(port);
	int rc = inet_pton(AF_INET, ip, &addr->sin_addr.s_addr);
	check(rc == 1, "IP is invalid.");
	return addr;

error:
	if (addr) free(addr);
	return NULL;
}

int main(int argc, char *argv[])
{
	int success = 0;
	RingBuffer *buffer = RingBuffer_create(1024);
	struct sockaddr_in *serv_addr = NULL;
	bstring data = NULL;
	int cl_sock_fd = 0;
	
	int serv_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	check(serv_sock_fd != -1, "Error with getting serv_sock_fd.");

	serv_addr = createAddr("127.0.0.1", 7777);
 	check(serv_addr != NULL, "Error with creating serv_addr.");

	int rc = bind(serv_sock_fd, (struct sockaddr *)serv_addr, sizeof(struct sockaddr));
	check(rc == 0, "Error with binding serv_sock_fd to serv_addr.");

	rc = listen(serv_sock_fd, 10);
	check(rc == 0, "Error with starting listening on serv_sock_fd.");

	struct sockaddr_in *cl_addr = NULL;
	uint cl_addr_size = sizeof(struct sockaddr);
	
	cl_sock_fd = accept(serv_sock_fd, (struct sockaddr *)cl_addr, &cl_addr_size);
	check(cl_sock_fd != -1, "Error with accepting connection.");
	
	while (1) {	
		rc = read_data(cl_sock_fd, buffer);
		check(rc >= 0, "Error with reading data.");
	
		if (rc == 0) {
			printf("Client closed the connection. Exiting.");
			break;
		}
		else {
			data = RingBuffer_get_all(buffer);
			printf("Received [%d bytes]:\n%s", blength(data), bdata(data));
			rc = write_data(cl_sock_fd, data);
			check(rc == blength(data), "Incorrect number of bytes sent.");
		}
	}

	success = 1;

error: //fallthrough

	if (serv_addr) free(serv_addr);
	if (cl_addr) free(cl_addr);
	if (buffer) free(buffer);
	if (data) bdestroy(data);
	if (serv_sock_fd) close(serv_sock_fd);
	if (cl_sock_fd) close(cl_sock_fd);
	if (success) {
		return 0;
	}
	else {
		return -1;
	}
}
