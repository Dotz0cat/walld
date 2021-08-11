/*
Copyright 2021 Dotz0cat

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
	#ifdef __clang__
	setenv("KMP_LOCK_KIND", "futex", 1);
	#endif

	signal(SIGHUP, SIG_IGN);
	signal(SIGCHLD, SIG_IGN);
	signal(SIGUSR1, SIG_IGN);
	signal(SIGUSR2, SIG_IGN);

	signal(SIGTERM, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGINT, SIG_IGN);

	sigset_t sigs;

	if (sigemptyset(&sigs) != 0) {
		abort();
	}

	if (sigaddset(&sigs, SIGHUP) != 0) {
		abort();
	}

	if (sigaddset(&sigs, SIGUSR1) != 0) {
		abort();
	}

	if (sigaddset(&sigs, SIGUSR2) != 0) {
		abort();
	}

	if (sigaddset(&sigs, SIGTERM) != 0) {
		abort();
	}

	if (sigaddset(&sigs, SIGQUIT) != 0) {
		abort();
	}

	if (sigaddset(&sigs, SIGINT) != 0) {
		abort();
	}

	if (sigprocmask(SIG_BLOCK, &sigs, NULL) != 0) {
		abort();
	}

	magick_start(argv[1]);
	magick_threads(1);

	pre_init_stuff* info = pre_init();

	init_daemon();

	syslog(LOG_NOTICE, "Walld is inited");

	fprintf(stderr, "this is stderr");

	magick_threads(4);

	loop_context* context;
	context = malloc(sizeof(loop_context));

	context->info = info;

	context->sigs = sigs;

	syslog(LOG_NOTICE, "Walld is started");

	event_loop_run(context);

	free_list(info->options->sources);
	free(info->options->feh_path);
	free(info->options->bg_style);
	free(info->options->x_auth);
	free(info->options->display);
	free(info->options);

	free(info->home_dir);
	free(info->x_auth);
	free(info->display);
	free(info->config);
	free_list(info->picture_list);
	free(info);

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

	stdout = fopen("/home/seth/.walld/log", "w");
	stderr = stdout;

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

	char* x_auth = strdup(getenv("XAUTHORITY"));

	info->x_auth = x_auth;

	char* display = strdup(getenv("DISPLAY"));

	info->display = display;

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
		abort();
	}

	info->picture_list = shuffle(info->picture_list);

	return info;
}
