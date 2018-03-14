#if 1
#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
/**
  The  example  program  below  demonstrates  the  use  of  cap_from_text()   and
  cap_to_text().  The following shell session shows a some example runs:

  $ ./a.out "cap_chown=p cap_chown+e"
  caps_to_text() returned "= cap_chown+ep"
  $ ./a.out "all=pe cap_chown-e cap_kill-pe"
  caps_to_text() returned "=ep cap_chown-e cap_kill-ep"

  The source code of the program is as follows:
  g++ setrlimit.cpp -lcap
 **/
#include <stdlib.h>
#include <stdio.h>
#include <sys/capability.h>
#include <string.h>

#define MAX_TCP_CONNECTIONS 				1024*1024
#define MAX_NUM_OF_PROCESSES 				1024*1024

#define	SET_MAX_TCP_CONNECTIONS				RLIMIT_NOFILE
#define	SET_MAX_NUM_OF_PROCESSES			RLIMIT_NPROC

#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)

static int set_max_tcp_connections(int resource)
{
	struct rlimit rl = {0};

	getrlimit(resource, &rl);
	printf("[1] rlim_cur[%ld], rlim_max[%ld]\n", rl.rlim_cur, rl.rlim_max);

	switch(resource)
	{
		case SET_MAX_TCP_CONNECTIONS:
			rl.rlim_max = MAX_TCP_CONNECTIONS;
			break;
		case MAX_NUM_OF_PROCESSES:
			rl.rlim_max = SET_MAX_NUM_OF_PROCESSES;
			return 0;
		default:
			return -1;
	}

	printf("[2] rlim_cur[%ld], rlim_max[%ld]\n", rl.rlim_cur, rl.rlim_max);

	if(setrlimit(resource, &rl))
		perror("setrlimit3");
	printf("[3] rlim_cur[%ld], rlim_max[%ld]\n", rl.rlim_cur, rl.rlim_max);

	getrlimit(resource, &rl);
	printf("[4] rlim_cur[%ld], rlim_max[%ld]\n", rl.rlim_cur, rl.rlim_max);

	rl.rlim_cur = rl.rlim_max;
	printf("[5] rlim_cur[%ld], rlim_max[%ld]\n", rl.rlim_cur, rl.rlim_max);

	if(setrlimit(resource, &rl))
		perror("setrlimit6");
	printf("[6] rlim_cur[%ld], rlim_max[%ld]\n", rl.rlim_cur, rl.rlim_max);

	getrlimit(resource, &rl);
	printf("[7] rlim_cur[%ld], rlim_max[%ld]\n", rl.rlim_cur, rl.rlim_max);

#if 0
	for(uint8_t i=0;i<2;i++){
		if(!setrlimit(RLIMIT_NOFILE, &rl) && !getrlimit(RLIMIT_NOFILE, &rl))
			rl.rlim_cur = rl.rlim_max;
	}

	printf("%s current max tcp connections, rlim_cur[%ld], rlim_max[%ld]\n", PRINT_WIFI_TRANSMISSION_STR, rl.rlim_cur, rl.rlim_max);
#endif
	return 0;
}
#if 0
int main()
{
	set_max_tcp_connections(SET_MAX_TCP_CONNECTIONS);
	return 0;
}
#endif
int main(int argc, char *argv[])
{
	cap_t caps = {0};
	char *txt_caps = NULL;
	cap_value_t cap_list[2] = {CAP_SYS_RESOURCE, CAP_SETPCAP};

	if (!CAP_IS_SUPPORTED(CAP_SETFCAP))
		handle_error("CAP_IS_SUPPORTED");

	caps = cap_get_proc();
	if (caps == NULL)
		handle_error("cap_get_proc");
	
	txt_caps = cap_to_text(caps, NULL);
	if (txt_caps == NULL)
		handle_error("cap_to_text");

	printf("caps_to_text() returned \"%s\"\n", txt_caps);
	
	printf("cap_get_bound %d\n", cap_get_bound(CAP_SYS_RESOURCE));
	
	if (cap_set_flag(caps, CAP_EFFECTIVE, 2, cap_list, CAP_SET) == -1)
		handle_error("cap_set_flag");
	
	txt_caps = cap_to_text(caps, NULL);
	if (txt_caps == NULL)
		handle_error("cap_to_text");

	printf("caps_to_text() returned \"%s\"\n", txt_caps);
	
	if (cap_set_flag(caps, CAP_PERMITTED, 2, cap_list, CAP_SET) == -1)
		handle_error("cap_set_flag");
	
	txt_caps = cap_to_text(caps, NULL);
	if (txt_caps == NULL)
		handle_error("cap_to_text");

	printf("caps_to_text() returned \"%s\"\n", txt_caps);
	
	if (cap_set_proc(caps) == -1)
		handle_error("cap_set_proc");
	
	set_max_tcp_connections(SET_MAX_TCP_CONNECTIONS);
	set_max_tcp_connections(SET_MAX_NUM_OF_PROCESSES);
	
	memset(&caps, 0, sizeof(cap_t));
	if (cap_set_proc(caps) == -1)
		handle_error("cap_set_proc");
	
	set_max_tcp_connections(SET_MAX_TCP_CONNECTIONS);
	set_max_tcp_connections(SET_MAX_NUM_OF_PROCESSES);

	if (cap_free(caps) == -1)
		handle_error("cap_free");

	
	#if 0
	cap_t caps;
	char *txt_caps;

	if (argc != 2) {
		fprintf(stderr, "%s <textual-cap-set>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	printf("argv[1]: %s\n", argv[1]);
	caps = cap_from_text(argv[1]);
	if (caps == NULL)
		handle_error("cap_from_text");
	
	//if (cap_clear(caps) != 0)
	//	handle_error("cap_clear");
	
	memset(&caps, 0, sizeof(cap_t));
	
	if(cap_set_file(argv[0], caps) != 0)
		handle_error("cap_set_file");
	
	txt_caps = cap_to_text(caps, NULL);
	if (txt_caps == NULL)
		handle_error("cap_to_text");

	printf("caps_to_text() returned \"%s\"\n", txt_caps);

	if (cap_free(txt_caps) != 0 || cap_free(caps) != 0)
		handle_error("cap_free");

	exit(EXIT_SUCCESS);
	#endif
}
#endif





