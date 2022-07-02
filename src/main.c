/*
Copyright 2021-2022 Dotz0cat

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

#include "main.h"

int main(int argc, char** argv) {

	//set enviroment variable to fix segfault when linked against clang's openmp
	//no trouble on bsd. as far as I can tell. also no futex
	#ifdef __clang__ && __linux__
	setenv("KMP_LOCK_KIND", "futex", 1);
	#endif

	int time_minutes = 0;
	char* config_from_cmd_line = NULL;
	char* source_from_line = NULL;
	int monitors = 0;
	int opt = 0;

	while((opt = getopt(argc, argv, "c:t:s:m:")) != -1) {
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
			default: /* ? */
				fprintf(stderr, "Usage: %s [-c config] [-t time] [-s source] [-m monitors]\r\n", argv[0]);
				return EXIT_FAILURE;
		}
	}

	magick_start(argv[1]);
	magick_threads(1);

	pre_init_stuff* info = pre_init(config_from_cmd_line, time_minutes, source_from_line, monitors);

	init_daemon(info->home_dir);

	syslog(LOG_NOTICE, "Walld is inited");

	magick_threads(4);

	loop_context* context;
	context = malloc(sizeof(loop_context));

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
	free(info);

	free(context);

	syslog(LOG_NOTICE, "Walld has quit");

	closelog();

	magick_stop();

	return EXIT_SUCCESS;
}

static void init_daemon(const char* home_dir) {
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

	int char_count = snprintf(NULL, 0, "%s%s", home_dir, "/.walld/log");

	if (char_count <= 0) {
		abort();
	}

	char* log_file = malloc(char_count + 1U);

	if (log_file == NULL) {
		abort();
	}

	snprintf(log_file, char_count + 1U, "%s%s", home_dir, "/.walld/log");

	stdin = fopen("/dev/null", "r");
	stdout = fopen(log_file, "w");

	free(log_file);

	char_count = 0;

	char_count = snprintf(NULL, 0, "%s%s", home_dir, "/.walld/error-log");

	if (char_count <= 0) {
		abort();
	}

	char* err_log_file = malloc(char_count + 1U);

	if (err_log_file == NULL) {
		abort();
	}

	snprintf(err_log_file, char_count + 1U, "%s%s", home_dir, "/.walld/error-log");


	stderr = fopen(err_log_file, "w");

	free(err_log_file);

	openlog("walld", LOG_PID, LOG_DAEMON);
}

static pre_init_stuff* pre_init(char* config, int time, char* source, int monitors) {

	pre_init_stuff* info = malloc(sizeof(pre_init_stuff));

	char* home = getenv("HOME");

	if (home == NULL) {
		uid_t uid = getuid();
        struct passwd* pw = getpwuid(uid);
        home = pw->pw_dir;
	}

	info->home_dir = strdup(home);

	char* x_auth = strdup(getenv("XAUTHORITY"));

	info->x_auth = x_auth;

	char* display = strdup(getenv("DISPLAY"));

	info->display = display;

	//get feh's path from the $PATH
	char* path = getenv("PATH");

	char* path_tok = strtok(path, ":");

	int going = 0;

	while(going == 0) {
		int count = snprintf(NULL, 0, "%s%s", path_tok, "/feh");

		if (count <= 0) {
			abort();
		}

		char* file = malloc(count + 1U);

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

		char* file = malloc(count + 1U);

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

		char* file = malloc(count + 1U);

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

		char* file = malloc(count + 1U);

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

		char* config_file = malloc(count + 1U);

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
