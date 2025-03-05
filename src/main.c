/*
Copyright 2021-2022, 2025 Dotz0cat

This file is part of walld.

    walld is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    walld is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with walld.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "main_private.h"

int main(int argc, char **argv) {

	//set enviroment variable to fix segfault when linked against clang's openmp
	//no trouble on bsd. as far as I can tell. also no futex
	#if defined(__clang__) && defined(__linux__)
	setenv("KMP_LOCK_KIND", "futex", 1);
	#endif

	int time_minutes = 0;
	char *config_from_cmd_line = NULL;
	char *source_from_line = NULL;
	int monitors = 0;
	int opt = 0;

	while((opt = getopt(argc, argv, "c:t:s:m:v")) != -1) {
		switch(opt) {
			case 'c':
				config_from_cmd_line = strdup(optarg);
				break;
			case 't':
				time_minutes = atoi(optarg);
				break;
			case 's':
				source_from_line = strdup(optarg);
				break;
			case 'm':
				monitors = atoi(optarg);
				break;
			case 'v':
				printf("%s version: %s\r\n", argv[0], VERSION);
				return EXIT_SUCCESS;
				break;
			default: /* ? */
				fprintf(stderr, "Usage: %s [-c config] [-t time] [-s source] [-m monitors] [-v]\r\n", argv[0]);
				return EXIT_FAILURE;
		}
	}

	magick_start(argv[1]);
	magick_threads(1);

	struct pre_init_stuff *info = pre_init(config_from_cmd_line, time_minutes, source_from_line, monitors);

	init_daemon();

	syslog(LOG_NOTICE, "Walld is inited");

	magick_threads(4);

	struct loop_context *context;
	context = malloc(sizeof(struct loop_context));

	context->info = info;

	syslog(LOG_NOTICE, "Walld is started");

	event_loop_run(context);

	free_list(info->options->sources);
	free_list(info->options->xrdb_argv);
	free(info->options->xrdb_path);
	free(info->options->feh_path);
	free(info->options->bg_style);
	if (info->options->x_auth != NULL) {
		free(info->options->x_auth);
	}
	if (info->options->display != NULL) {
		free(info->options->display);
	}
	free(info->options);

	free(info->home_dir);
	if (info->source != NULL) {
		free(info->source);
	}
	free(info->x_auth);
	free(info->display);
	free(info->config);
	free_circular_list(info->picture_list);
	free(info->feh_path);
	free(info->xrdb_path);
	free(info);

	free(context->feh_path);
	free(context->xrdb_path);

	free(context);

	syslog(LOG_NOTICE, "Walld has quit");

	closelog();

	magick_stop();

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

	fclose(stdin);
	fclose(stdout);
	fclose(stderr);

	openlog("walld", LOG_PID, LOG_DAEMON);
}

static struct pre_init_stuff *pre_init(char *config, int time, char *source, int monitors) {

	struct pre_init_stuff* info = malloc(sizeof(struct pre_init_stuff));

	char* home = getenv("HOME");

	if (home == NULL) {
		uid_t uid = getuid();
        struct passwd *pw = getpwuid(uid);
        home = pw->pw_dir;
	}

	info->home_dir = strdup(home);

	char *x_auth = strdup(getenv("XAUTHORITY"));

	info->x_auth = x_auth;

	char *display = strdup(getenv("DISPLAY"));

	info->display = display;

	//get feh's path from the $PATH
	char *path = getenv("PATH");

	char *path_tok = strtok(path, ":");

	int going = 0;

	while(going == 0) {
		int count = snprintf(NULL, 0, "%s%s", path_tok, "/feh");

		if (count <= 0) {
			abort();
		}

		char *file = malloc(count + 1U);

		if (file == NULL) {
			abort();
		}

		snprintf(file, count + 1U, "%s%s", path_tok, "/feh");

		int worked = access(file, X_OK);

		free(file);

		if (worked == 0) {
			going = 1;
		}
		else {
			path_tok = strtok(NULL, ":");

			if (path_tok == NULL) {
				going = 1;
			}
		}
	}

	if (path_tok != NULL) {
		int count = snprintf(NULL, 0, "%s%s", path_tok, "/feh");

		if (count <= 0) {
			abort();
		}

		char *file = malloc(count + 1U);

		if (file == NULL) {
			abort();
		}

		snprintf(file, count + 1U, "%s%s", path_tok, "/feh");

		info->feh_path = file;
	}
	else {
		//Set non NULL value so nothing fails. true on most distros
		info->feh_path = strdup("/usr/bin/feh");
	}

	//get xrdb from the PATH
	path_tok = strtok(path, ":");

	going = 0;

	while(going == 0) {
		int count = snprintf(NULL, 0, "%s%s", path_tok, "/xrdb");

		if (count <= 0) {
			abort();
		}

		char *file = malloc(count + 1U);

		if (file == NULL) {
			abort();
		}

		snprintf(file, count + 1U, "%s%s", path_tok, "/xrdb");

		int worked = access(file, X_OK);

		free(file);

		if (worked == 0) {
			going = 1;
		}
		else {
			path_tok = strtok(NULL, ":");

			if (path_tok == NULL) {
				going = 1;
			}
		}
	}

	if (path_tok != NULL) {
		int count = snprintf(NULL, 0, "%s%s", path_tok, "/xrdb");

		if (count <= 0) {
			abort();
		}

		char *file = malloc(count + 1U);

		if (file == NULL) {
			abort();
		}

		snprintf(file, count + 1U, "%s%s", path_tok, "/xrdb");

		info->xrdb_path = file;
	}
	else {
		//Set non NULL value so nothing fails. true on most distros
		info->xrdb_path = strdup("/usr/bin/xrdb");
	}

	if (config != NULL) {
		info->config = config;
	}
	else {
		int count = snprintf(NULL, 0, "%s%s", home, "/.walld/.walldrc");

		if (count <= 0) {
			abort();
		}

		char *config_file = malloc(count + 1U);

		if (config_file == NULL) {
			abort();
		}

		snprintf(config_file, count + 1U, "%s%s", home, "/.walld/.walldrc");

		info->config = config_file;
	}

	info->source = source;

	info->options = read_config(info->config, home, info->source);

	info->time = time;

	info->monitors = monitors;

	//config file takes piority over enviroment variables
	if (info->options->x_auth != NULL) {
		if (info->x_auth != NULL) {
			free(x_auth);
		}

		info->x_auth = info->options->x_auth;
	}

	if (info->options->display != NULL) {
		if (info->display != NULL) {
			free(display);
		}

		info->display = info->options->display;
	}

	info->picture_list = get_images(info->options->sources);

	if (info->picture_list == NULL) {

		fprintf(stderr, "Walld found no suitable images to use\r\n");
		abort();
	}

	info->picture_list = shuffle(info->picture_list);

	return info;
}
