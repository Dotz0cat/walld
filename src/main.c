#include "main.h"

int main(int argc, char** argv) {
	init_daemon();

	while(1) {
		//stuff
		syslog(LOG_NOTICE, "Walld is started");
		sleep(20);
		break;
	}

	syslog(LOG_NOTICE, "Walld has quit");
	closelog();

	return EXIT_SUCCESS;
}

static void init_daemon() {
	pid_t pid;

	pid = fork();

	if (pid < 0) {
		exit(EXIT_FAILURE);
	}

	if (pid > 0) {
		exit(EXIT_SUCCESS);
	}

	if (setsid() < 0) {
		exit(EXIT_FAILURE);
	}

	signal(SIGCHLD, SIG_IGN);
	signal(SIGHUP, SIG_IGN);

	pid = fork();

	if (pid < 0) {
		exit(EXIT_FAILURE);
	}

	if (pid > 0) {
		exit(EXIT_SUCCESS);
	}

	umask(0);

	chdir("/");

	int x;
	for (x = sysconf(_SC_OPEN_MAX); x>=0; x--) {
		close(x);
	}

	openlog("walld", LOG_PID, LOG_DAEMON);
}

static pre_init_stuff* pre_init() {

	pre_init_stuff* info = malloc(sizeof(pre_init_stuff));

	char* home = getenv("HOME");

	if (home == NULL) {
		uid_t uid = getuid();
        struct passwd* pw = getpwuid(uid);
        home = pw->pw_dir;
	}

	info->home_dir = home;



	return info;
}

linked_node* get_images(linked_node* source) {
	linked_node* entry_point = malloc(sizeof(linked_node));
	linked_node* current = entry_point;

	linked_node* source_iter = source;

	while (source_iter->next != NULL) {
		file_type type = get_file_type(source->image);

		switch(type) {
			case (DIRECTORY): {
				linked_node* dir_list;
				dir_list = list_files_full(source->image);

				if (dir_list != NULL) {
					linked_node* typed = get_images(dir_list);

					if (typed != NULL) {
						current->next = typed;

						current = wind_to_tail(current);
					}
					free_list(dir_list);
				}
				break;
			}	
			case(LIST): {
				break;
			}
			case(IMAGE): {
				current = add_node_to_list(current, source->image);
			}
			break;
			case(ERROR):
			default: {
			//give error
			//do nothing
			//just dont add it
			break;
			}
		}

		source_iter = source_iter->next;
	}

	if (entry_point->image == NULL) {
		free(entry_point);
		return NULL;
	}

	return entry_point;
}

file_type get_file_type(const char* path) {
	DIR* dir = opendir(path);

	if (dir != NULL) {
		closedir(dir);
		return DIRECTORY;
	}

	if (errno == ENOTDIR) {
		return image_or_text(path);
	}
	else {
		return ERROR;
	}
}

linked_node* list_files_full(const char* directory) {
	struct dirent* d;

	DIR* dir;

	dir = opendir(directory);

	linked_node* entry_point = malloc(sizeof(linked_node));

	linked_node* current = entry_point;

	if (dir != NULL) {
		while ((d = readdir(dir)) != NULL) {
			if (strcmp(d->d_name, ".") != 0 || strcmp(d->d_name, "..") != 0) {
				char* real = realpath_wrap(d->d_name);

				if (real != NULL) {
					current = add_node_to_list(current, real);
				}
			}
		}
	}
	else {
		free(entry_point);
		return NULL;
	}
	

	return entry_point;
}

char* realpath_wrap(const char* path) {
	char* res = realpath(path, NULL);

#if defined(PATH_MAX) && PATH_MAX > 0

	if (res == NULL && path != NULL && errno == EINVAL) {

		char* resolved_path = malloc(PATH_MAX);

		if (resolved_path != NULL) {
			char* tmp = realpath(path, resolved_path);

			if (tmp != NULL) {
				res = strdup(tmp);
				free(tmp);
			}

			free(resolved_path);
		}
	}

#endif

	return res;
}
