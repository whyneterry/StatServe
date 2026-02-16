#include "net.h"

struct tagbstring NL = bsStatic("\n");
struct tagbstring NT = bsStatic("\0");
struct tagbstring OK = bsStatic("200 OK\n");
struct tagbstring CRTD = bsStatic("201 OK CREATED\n");
struct tagbstring DEL = bsStatic("205 OK DELETED\n");
struct tagbstring RWR = bsStatic("202 OK REWRITED\n");
struct tagbstring BR = bsStatic("400 BAD REQUEST\n");
struct tagbstring UERR = bsStatic("TRY /list commands\n");
struct tagbstring EXISTS = bsStatic("400 ALREADY EXISTS\n");
struct tagbstring WELCOME = bsStatic("STATSERVE v0.0\n"
									"TRY /list commands first\n");

char *usagemsg = "=====================\n"
				"STATSERVE CHEATSHEET\n"
				"=====================\n"
				"/create <key>\n"
				"/dump <key>\n"
				"/mean <key>\n"
				"/delete <key>\n"
				"/sample <key> <float>\n"
				"/list <arg>\n"
				"FOR MORE INFORMATION: "
				"/list all\n";

char *fullusagemsg = "=======================\n"
					"STATSERVE ADVANCED USAGE\n"
					"=======================\n"
					"/create <key/keys>\n"
					"/dump <key/keys>\n"
					"/mean <key/keys>\n"
					"/delete <key/keys>\n"
					"/sample <key/keys> <float>\n"
					"/store <key/keys>\n"
					"/load *<key/keys>\n"
					"/list <arg>\n\n"
					"arg could be:\n"
					"\tall - show advenced usage of statserve\n"
					"\tcommands - show statserve cheatsheet\n\n"
					"If you use multiple keys in request, make\n"
					"sure to separate them with '/' only.\n\n"
					"Note that /load works slightly different.\n\n"
					"If one key passed, then load assumes:\n"
					"\"Load from file named 'key'. Create new\n"
					"node in current map. Store loaded data in\n"
					"created node and name it as 'key'\"\n\n"
					"If multiple keys passed, then load assumes:\n"
					"\"Load from file named like the first 'key'.\n"
					"Create necessary nodes according to remaining\n"
					"'keys'. Store loaded data in all these 'keys'.\n";

int read_data(int fd, RingBuffer *buffer, int is_sock)
{
	debug("Reading data...");
	int rc = 0;

	if (is_sock) {
		void *start = RingBuffer_starts_at(buffer);
		int space = RingBuffer_available_space(buffer);

		debug("START: %p | SPACE: %d", start, space);
 		rc = recv(fd, start, RingBuffer_available_space(buffer), 0);

	}
	else {
		rc = read(fd, RingBuffer_starts_at(buffer), RingBuffer_available_space(buffer));
	}
	
	RingBuffer_commit_write(buffer, rc);
	
	check(rc >= 0, "Error with recieve data.");	
	
	return rc;

error:
	return -1;
}

int send_msg(int cl_sock_fd, bstring data, int is_sock)
{
	int rc = 0;
	
	if (is_sock) {
		rc = send(cl_sock_fd, bdata(data), blength(data), 0);
	}
	else {
		rc = write(1, bdata(data), blength(data));
	}
	check(rc == blength(data), "Error with sending message.");

	return rc;

error:
	return -1;
}

void send_err(int cl_sock_fd, char *msg, int is_sock)
{
	bstring data = bfromcstr(msg);

	int rc = send_msg(cl_sock_fd, data, is_sock);
	check(rc != -1, "Error with sending error.");

	debug("Error sent");
	
error:
	debug("error tag in send_err");
	if (data) bdestroy(data);
	debug("b4 return send_err");
	return;
}

static int sanitize_request(bstring key)
{
	check(key != NULL, "Invalid key to sanitize.");

	int rc = -1;
	
	if ( (bdata(key))[blength(key) - 1] == '\n' ) {
		rc = bdelete(key, blength(key) - 1, 1);
		check(rc == 0, "Error with bdelete.");
	}

	if ( (bdata(key))[0] == '/' ) {
		rc = bdelete(key, 0, 1);
		check(rc == 0, "Error with bdelete.");
	}
	
	return 0;

error:
	return -1;
}

int parse_request(SharedData *s_data, struct bstrList *list, int cl_sock_fd, bstring db_path, int is_sock)
{
	bstring binfo = NULL;

	int rc = -1;	
	bstring cmd = NULL;
	bstring key = NULL;
	bstring data = NULL;
	struct bstrList *urls = NULL;
	
	Command cmd_s = { .s_data = s_data,
					.cl_sock_fd = cl_sock_fd,
					.is_sock = is_sock,
					.db_path = db_path,
					.binfo = binfo };
	
	if (list->qty != 2 && list->qty != 3) {	
		goto error;
	}
	else {
		cmd = list->entry[0];
		key = list->entry[1];
		
		debug("qty = %d: %s %s", list->qty, bdata(cmd), bdata(key));

		rc = sanitize_request(key);
		check(rc == 0, "Error with sanitizing request.");

		urls = bsplit(key, '/');
		check(urls != NULL, "Error with bsplit.");

		cmd_s.urls = urls;

		debug("end of else");
	}

	if (list->qty == 2) {
	
		// create, dump, mean, delete
		if (biseqcstr(cmd, "/create") == 1)
		{
			rc = create_stats(&cmd_s);	
			check(rc != -1, "Error with /create.");
		}
		else if (biseqcstr(cmd, "/dump") == 1)
		{
			rc = dump_stats(&cmd_s);
			check(rc == 0, "Error with dumping stats.");
		}
		
		else if (biseqcstr(cmd, "/mean") == 1)
		{
			debug("MEAN CASE");
			rc = mean_stats(&cmd_s);
			check(rc == 0, "Error with /mean.");

		}
		
		else if (biseqcstr(cmd, "/delete") == 1)
		{
			rc = delete_stats(&cmd_s);	
			check(rc == 0, "Error with /delete.");
		}
		
		else if (biseqcstr(cmd, "/list") == 1)
		{
			if (biseqcstr(key, "commands") == 1)
			{
				binfo = bfromcstr(usagemsg);
				rc = send_msg(cl_sock_fd, binfo, is_sock);
				check(rc != -1, "Error with sending usagemsg.");
			}
			
			else if (biseqcstr(key, "all") == 1)
			{
				binfo = bfromcstr(fullusagemsg);
				rc = send_msg(cl_sock_fd, binfo, is_sock);
				check(rc != -1, "Error with sending usagemsg.");
			}
			
			else {
				goto error;
			}
		}

		else if (biseqcstr(cmd, "/store") == 1)
		{
			rc = store_stats(&cmd_s);
			check(rc != -1, "Error with storing stats.");
		}
	
		else if (biseqcstr(cmd, "/load") == 1)
		{
			rc = load_stats(&cmd_s);
			check(rc != -1, "Error with storing stats.");
		}
			
		else {
			goto error;
		}
	}

	else if (list->qty == 3) {
		// sample
		debug("3 arguments case.");
		
		data = list->entry[2];
		cmd_s.data = data;
		check(bfindreplace(data, &NL, &NT, 0) == BSTR_OK,
		"Failed to replace newlines.");		

		if (biseqcstr(cmd, "/sample") == 1)
		{
			rc = sample_stats(&cmd_s);
			check(rc == 0, "Error with adding sample to stats.");
		}
		
		else {
			goto error;
		}
	}
	
	bstrListDestroy(urls);

	return 0;

error:
	if (urls) bstrListDestroy(urls);
	debug("error tag in parsing.");
	send_msg(cl_sock_fd, &BR, is_sock);
	return -1;
}

int sample_stats(Command *cmd)
{
	int rc = -1;
	char **endptr = calloc(1, sizeof(char *));
	
	for (int i = 0; i < cmd->urls->qty; ++i) {
		if (blength(cmd->urls->entry[i]) == 0) {
			continue;
		}
		bstring c_key = cmd->urls->entry[i];
		bstring c_data = cmd->data;

		char *n = bdata(c_data);
		int n_len = strlen(n);
		double num = strtod(n, endptr);

		if (endptr[0] != &n[n_len]) {
			debug("Convertion failed. "
			"Invalid value. "
			"Should be float number\n");
			debug("endptr[0]: %p"
					"n[n_len]: %p\n",
					endptr[0], &n[n_len]);
			goto error;
		}

		Stats *st = (Stats *)BSTree_get(cmd->s_data->map, c_key);
		check(st != NULL, "Error with getting stats.");
		
		Stats_sample(st, num);
	}

	rc = send_msg(cmd->cl_sock_fd, &OK, cmd->is_sock);	
	check(rc != -1, "Error with sending OK.");

	if (endptr) free(endptr);
	return 0;

error:
	if (endptr) free(endptr);
	return -1;
}

int create_stats(Command *cmd)
{
	int rc = -1;
	bstring c_key = NULL;

	pthread_mutex_lock(&cmd->s_data->mutex);

	for (int i = 0; i < cmd->urls->qty; ++i) { 

		if (blength(cmd->urls->entry[i]) == 0) {
			continue;
		}
		
		c_key = bstrcpy(cmd->urls->entry[i]);
		check(c_key != NULL, "Error with bstrcpy.");

		if (BSTree_get(cmd->s_data->map, c_key) != NULL) {
			// key already exists
			debug(":why? EXISTS CASE");
			rc = send_msg(cmd->cl_sock_fd, &EXISTS, cmd->is_sock);
			check(rc != -1, "Error with sending EXISTS.");
			bdestroy(c_key);
			continue;
		}

		Stats *st = Stats_create();
		check(st != NULL, "Error with creating stats.");

		printf("Creating stats for key: %s [%d]\n",
						bdata(c_key), blength(c_key));

		rc = BSTree_set(cmd->s_data->map, (void *)c_key, (void *)st);
		check(rc == 0, "Error with setting BSTree.");

		rc = send_msg(cmd->cl_sock_fd, &CRTD, cmd->is_sock);	
		check(rc != -1, "Error with sending OK.");	
	}		
	
	pthread_mutex_unlock(&cmd->s_data->mutex);

	return 0;

error:
	pthread_mutex_unlock(&cmd->s_data->mutex);
	if (c_key) bdestroy(c_key);
	return -1;
}

int dump_stats(Command *cmd)
{
	int rc = -1;
	
	pthread_mutex_lock(&cmd->s_data->mutex);

	for (int i = 0; i < cmd->urls->qty; ++i) {	
		if (blength(cmd->urls->entry[i]) == 0) {
			continue;
		}

		Stats *st = (Stats *)BSTree_get(cmd->s_data->map, cmd->urls->entry[i]);
		check(st != NULL, "Error with getting stats.");
		
		cmd->binfo = bformat("/%s: "
				"SUM: %f | "
				"SUMSQ: %f | "
				"N: %lu | "
				"MIN: %f | "
				"MAX: %f | "
				"MEAN: %f | "
				"STDDEV: %f\n",
				bdata(cmd->urls->entry[i]),
				st->sum,
				st->sumsq,
				st->n,
				st->min,
				st->max,
				Stats_mean(st),
				Stats_stddev(st));		

		rc = send_msg(cmd->cl_sock_fd, cmd->binfo, cmd->is_sock);
		check(rc != -1, "Error with sending stats.");
	}
	
	pthread_mutex_unlock(&cmd->s_data->mutex);

	return 0;

error:	
	pthread_mutex_unlock(&cmd->s_data->mutex);
	
	return -1;
}

int mean_stats(Command *cmd)
{
	int rc = -1;

	pthread_mutex_lock(&cmd->s_data->mutex);
	
	for (int i = 0; i < cmd->urls->qty; ++i) {
		if (blength(cmd->urls->entry[i]) == 0) {
			continue;
		}
		Stats *st = (Stats *)BSTree_get(cmd->s_data->map,
										cmd->urls->entry[i]);
		
		check(st != NULL, "Error with getting stats.");

		double mean = Stats_mean(st);
	
		debug("GOT MEAN: %f", mean);
	
		cmd->binfo = bformat("/%s: MEAN: %f\n",
								bdata(cmd->urls->entry[i]),
								mean);
		
		rc = send_msg(cmd->cl_sock_fd, cmd->binfo, cmd->is_sock);
		check(rc != -1, "Error with sending mean.");
	}

	pthread_mutex_unlock(&cmd->s_data->mutex);

	return 0;

error:
	pthread_mutex_unlock(&cmd->s_data->mutex);
	return -1;
}

int delete_stats(Command *cmd)
{
	int rc = -1;
	pthread_mutex_lock(&cmd->s_data->mutex);

	for (int i = 0; i < cmd->urls->qty; ++i) {
		if (blength(cmd->urls->entry[i]) == 0) {
			continue;
		}
		
		Stats *st = (Stats *)BSTree_delete(cmd->s_data->map,
								cmd->urls->entry[i]);
		
		check(st != NULL, "Error with getting stats.");
	}
	
	rc = send_msg(cmd->cl_sock_fd, &DEL, cmd->is_sock);	
	check(rc != -1, "Error with sending DEL.");

	pthread_mutex_unlock(&cmd->s_data->mutex);

	return 0;

error:
	pthread_mutex_unlock(&cmd->s_data->mutex);
	return -1;
}

bstring md5hash(bstring key, int outlen) {
	unsigned char *rp;
	bstring hash = NULL;
	char *temphash = NULL;

	printf("MD5_DL = %d\n", MD5_DIGEST_LENGTH);
    unsigned char digest[MD5_DIGEST_LENGTH];

    // encoding the message using the library function
    rp = MD5((unsigned char*)bdata(key), blength(key), digest);
	check(rp != NULL, "Error with MD5.");

    printf("Hash of the key string:\n");
    for(int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        printf("%02x", digest[i]);
    }
	printf("\n");
	
	temphash = calloc(outlen + 1, sizeof(char));
	for(int i = 0; i < outlen / 2; i++) {
        sprintf(&temphash[i * 2], "%02x", (unsigned int)digest[i]);
    }

	hash = bfromcstr(temphash);
	check(hash != NULL, "Error with bfromcstr");
	
	free(temphash);	

	return hash;

error:
	if (temphash) free(temphash);
	return NULL;
}

static bstring get_location(bstring dir, bstring fname)
{
	int rc = -1;

	bstring location = NULL;
	bstring hash = NULL;

	check(dir != NULL, "Invalid DB path.");
	check(fname != NULL, "Invalid statistic name to store.");

	location = bstrcpy(dir);
	check(location != NULL, "Error with bstrcpy.");

	// I found out that fopen doesn't make subdirs recursively
	// so user should make main directory himself

	rc = bcatcstr(location, "/");
	check(rc == BSTR_OK, "Error with bcatcstr.");

	hash = md5hash(fname, 8);
	check(hash != NULL, "Error with md5hash.");

	rc = bconcat(location, hash);
	check(rc == BSTR_OK, "Error with bconcat.");

	bdestroy(hash);
	
	return location;

error:
	if (hash) bdestroy(hash);
	if (location) bdestroy(location);

	return NULL;
}

int store_stats(Command *cmd)
{	
	pthread_mutex_lock(&cmd->s_data->mutex);

	FILE *fptr = NULL;
	bstring location = NULL;
	check(cmd != NULL, "Invalid command.");

	int rc = -1;
	for (int i = 0; i < cmd->urls->qty; ++i) {
		if (blength(cmd->urls->entry[i]) == 0) {
			continue;
		}
		
		Stats *st = (Stats *)BSTree_get(cmd->s_data->map,
										cmd->urls->entry[i]);	
		check(st != NULL, "Error with getting stats.");
		
		location = get_location(cmd->db_path, cmd->urls->entry[i]);
		check(location != NULL, "Error with get_location.");
	
		printf("STORE LOCATION: %s\n", bdata(location));

		fptr = fopen(bdata(location), "w");
		check(fptr != NULL, "Error with fopen.");
		
		printf("======DUMP_FROM_STORE=====\n");	
		Stats_dump(st);
		
		rc = (int)fwrite(st, sizeof(Stats), 1, fptr);
		printf("FWRITE RC: %d\n", rc);
		check(rc == 1, "Error with fwrite.");

		fclose(fptr);
		bdestroy(location);
	}

	pthread_mutex_unlock(&cmd->s_data->mutex);

	rc = send_msg(cmd->cl_sock_fd, &OK, cmd->is_sock);	
	check(rc != -1, "Error with sending OK.");

	return 0;

error:
	pthread_mutex_unlock(&cmd->s_data->mutex);
	if (fptr) fclose(fptr);
	if (location) bdestroy(location);

	return -1;
}

static int update_stats(BSTree *map, bstring dst, Stats *src_st)
{
	void *rp;
	bstring c_dst = NULL;
	int rc = -1;

	if (BSTree_get(map, dst) != NULL) {
		rp = BSTree_delete(map, dst);	
		check(rp != NULL, "Error with deleting.");
	}
	
	Stats *dst_st = Stats_create();
	check(dst_st != NULL, "Error with creating stats.");
	rp = memcpy(dst_st, src_st, sizeof(Stats));
	check(rp != NULL, "Error with memcpy.");
	
	c_dst = bstrcpy(dst);
	rc = BSTree_set(map, c_dst, dst_st);
	check(rc == 0, "Error with setting value.");	

	return 0;

error:
	return -1;
}

int load_stats(Command *cmd)
{
	pthread_mutex_lock(&cmd->s_data->mutex);

	int rc = -1, this_rc = -1;

	FILE *fptr = NULL;
	bstring src = NULL;
	bstring dst = NULL;
	bstring location = NULL;
	Stats *src_st = NULL;

	check(cmd != NULL, "Invalid command.");
	
	src = cmd->urls->entry[0];

	location = get_location(cmd->db_path, src);
	check(location != NULL, "Error with get_location.");
	printf("LOAD LOCATION: %s\n", bdata(location));

	fptr = fopen(bdata(location), "r");
	check(fptr != NULL, "Error with opening file: %s",
										location);

	debug("SRC: %s [%d]"
		"DST : %s [%d]",
			bdata(src),
			blength(src),
			bdata(dst),
			blength(dst));
	
	int count = cmd->urls->qty;
	
	src_st = Stats_create();
	check(src_st != NULL, "Error with creating stats.");

	rc = (int)fread(src_st, sizeof(Stats), 1, fptr);
	debug("FREAD RC: %d", rc);
	check(rc == 1, "Read more than 1 Stats.");

	if (count >= 2) {
		// load stats from src to dst recursively
		// edge case when count == 2
		// for-loop with only 1 cycle
		// but is seems normal to me
		
		for (int i = 1; i < count; ++i) {	
			dst = cmd->urls->entry[i];
			
			if (BSTree_get(cmd->s_data->map, dst) != NULL)
			{
				rc = send_msg(cmd->cl_sock_fd, &RWR, cmd->is_sock);
				check(rc != -1, "Error with sending RWR.");
			}
		
			rc = update_stats(cmd->s_data->map, dst, src_st);
			check(rc == 0, "Error with updating stats.");
		}
	}
	
	else if (count == 1) {
		// load stats from a file to src
		printf("======DUMP_FROM_LOAD=====\n");	
		Stats_dump(src_st);
	
		rc = update_stats(cmd->s_data->map, src, src_st);
		check(rc == 0, "Error with setting stats.");
	}
	
	rc = send_msg(cmd->cl_sock_fd, &OK, cmd->is_sock);	
	check(rc != -1, "Error with sending OK.");

	this_rc = 0;

error: //fallthrough

	pthread_mutex_unlock(&cmd->s_data->mutex);
	if (fptr) fclose(fptr);
	if (src_st) free(src_st);
	if (location) bdestroy(location);

	return this_rc;
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

void *client_handler(void *arg)
{
	struct ClientThread *args = (struct ClientThread *)arg;
	int cl_sock_fd = args->cl_sock_fd;
	SharedData *s_data = args->s_data;
	bstring db_path = args->s_data->db_path;

	int rc = -1;
	RingBuffer *buffer = RingBuffer_create(1024);
	bstring data = NULL;

	send_msg(cl_sock_fd, &WELCOME, 1);
	
	while (1) {
		rc = read_data(cl_sock_fd, buffer, 1);
		check(rc >= 0, "Error with reading data.");

		debug("Data was read.");
	
		if (rc == 0) {
			printf("Client closed the connection. Exiting.\n");
			break;
		}
		else {
			data = RingBuffer_get_all(buffer);
			check(data != NULL, "Buffer was empty.");
			printf("Received [%d bytes]:\n%s", blength(data), bdata(data));
			//rc = send_msg(cl_sock_fd, data, 1);
			//check(rc == blength(data), "Incorrect number of bytes sent.");
	
			struct bstrList *list = bsplit(data, ' ');
			check(list != NULL, "Error wtih bsplit");
	
			debug("Parsing arguments...");
			rc = parse_request(s_data, list,
							cl_sock_fd,
							db_path, 1);

			bstrListDestroy(list);
			debug("Returned from parsing.");
		}
	}

error: //fallthrough
	args->cl_sock_fd = 0;
	if (cl_sock_fd) close(cl_sock_fd);
	if (buffer) RingBuffer_destroy(buffer);
	if (data) bdestroy(data);
	
	return NULL;
}
