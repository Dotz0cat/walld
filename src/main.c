#include "main.h"

int main(int argc, char** argv) {
	pre_init_stuff* info = pre_init();

	printf("%s\n", info->home_dir);
	printf("%s\n", info->config);
	printf("%s\n", info->options->sources->image);
	printf("%s\n", info->picture_list->image);
	//init_daemon();

	// while(1) {
	// 	//stuff
	// 	syslog(LOG_NOTICE, "Walld is started");
	// 	sleep(20);
	// 	break;
	// }

	// syslog(LOG_NOTICE, "Walld has quit");
	// closelog();

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

static pre_init_stuff* pre_init(void) {

	pre_init_stuff* info = malloc(sizeof(pre_init_stuff));

	char* home = getenv("HOME");

	if (home == NULL) {
		uid_t uid = getuid();
        struct passwd* pw = getpwuid(uid);
        home = pw->pw_dir;
	}

	info->home_dir = home;

	int count = snprintf(NULL, 0, "%s%s", home, "/.walld/.walldrc");

	if (count <= 0) {
		abort();
	}

	char* config_file = malloc(count + 1U);

	if (config_file == NULL) {
		abort();
	}

	snprintf(config_file, count + 1U, "%s%s", home, "/.walld/.walldrc");

	info->config = config_file;

	info->options = read_config(config_file, home);

	info->picture_list = get_images(info->options->sources);

	if (info->picture_list == NULL) {
		abort();
	}

	return info;
}

linked_node* get_images(linked_node* source) {
	linked_node* entry_point = add_node_to_list(NULL, NULL);

	linked_node* current = entry_point;

	do {
		file_type type = get_file_type(source->image);

		switch(type) {
			case (DIRECTORY): {
				linked_node* dir_list;
				dir_list = list_files_full(source->image);

				if (dir_list != NULL) {
					linked_node* typed = get_images(dir_list);

					if (typed != NULL) {
						if (current == entry_point) {
							free(entry_point);

							entry_point = typed;

							current = entry_point;

							current = wind_to_tail(current);
						}
						else {
							current->next = typed;

							current = wind_to_tail(current);
						}
					}
					free_list(dir_list);
				}
				break;
			}	
			case(LIST): {
				linked_node* file_contents = list_file_parse(source->image);

				if (file_contents != NULL) {
					linked_node* typed = get_images(file_contents);

					if (typed != NULL) {
						if (current == entry_point) {
							free(entry_point);

							entry_point = typed;

							current = entry_point;

							current = wind_to_tail(current);
						}
						else {
							current->next = typed;

							current = wind_to_tail(current);
						}
					}
					free_list(file_contents);
				}
				break;
			}
			case(IMAGE): {
				current = add_node_to_list(current, source->image);

				break;
			}
			case(ERROR):
			default: {
			//give error
			//do nothing
			//just dont add it
			break;
			}
		}
		if (source->next != NULL) {
			source = source->next;
		}
	} while (source->next != NULL);

	if (entry_point->image == NULL) {
		free(entry_point);
		return NULL;
	}

	return entry_point;
}

file_type get_file_type(const char* path) {
	if (path == NULL) {
		return ERROR;
	}
	else if (strcmp(path, "") == 0) {
		return ERROR;
	}

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

	linked_node* entry_point = add_node_to_list(NULL, NULL);

	linked_node* current = entry_point;

	if (dir != NULL) {
		while ((d = readdir(dir)) != NULL) {
			if (strcmp(d->d_name, ".") != 0) {
				if (strcmp(d->d_name, "..") != 0) {
					char* real = realpath_wrap(d->d_name, directory);

					if (real != NULL) {
						if (strcmp(real, ".") != 0 || strcmp(real, "..") != 0) {
							current = add_node_to_list(current, real);
						}
					}
				}
			}
		}
	}
	else {
		free(entry_point);
		return NULL;
	}

	closedir(dir);

	if (entry_point->image == NULL) {
		free(entry_point);
		return NULL;
	}
	

	return entry_point;
}

char* realpath_wrap(const char* path, const char* dir) {
	int count = snprintf(NULL, 0, "%s%s%s", dir, "/", path);

	if (count <= 0) {
		return NULL;
	}

	char* longer_path = malloc(count + 1U);

	if (longer_path == NULL) {
		return NULL;
	}

	snprintf(longer_path, count + 1U, "%s%s%s", dir, "/", path);

	char* res = realpath(longer_path, NULL);

	if (res != NULL) {
		if (strcmp(res, ".") == 0) {
			return NULL;
		}
		else if (strcmp(res, "..") == 0) {
			return NULL;
		}
	}

#if defined(PATH_MAX) && PATH_MAX > 0

	if (res == NULL && longer_path != NULL && errno == EINVAL) {

		char* resolved_path = malloc(PATH_MAX);

		if (resolved_path != NULL) {
			char* tmp = realpath(longer_path, resolved_path);

			if (tmp != NULL) {
				res = strdup(tmp);
				free(tmp);
			}

			free(resolved_path);
		}
	}

#endif

	free(longer_path);

	return res;
}
