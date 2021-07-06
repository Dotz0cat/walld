#include "loop.h"

int event_loop_run(loop_context* context) {
	linked_node* current = context->info->picture_list;

	int efd = epoll_create1(0);

	if (efd <=0) {
		abort();
	}

	sigset_t sigs;

	if (sigemptyset(&sigs) != 0) {
		abort();
	}

	if (sigaddset(&sigs, SIGHUP) != 0) {
		abort();
	}

	int sig_fd = signalfd(-1, &sigs, 0);

	struct epoll_event signal_event;
	signal_event.events = EPOLLIN;
	signal_event.data.fd = sig_fd;

	if (epoll_ctl(efd, EPOLL_CTL_ADD, sig_fd, &signal_event) != 0) {
		close(efd);
		abort();
	}

	int timer = timerfd_create(CLOCK_MONOTONIC, 0);

	struct itimerspec timespec;

	//120 seconds for testing. will make a context struct later that will set it
	timespec.it_value.tv_sec = 60;
	timespec.it_value.tv_nsec = 0;
	timespec.it_interval.tv_sec = 60;
	timespec.it_interval.tv_nsec = 0;

	if (timerfd_settime(timer, 0, &timespec, NULL) != 0) {
		close(efd);
		close(timer);
		abort();
	}

	struct epoll_event timer_event;
	timer_event.events = EPOLLIN;
	timer_event.data.fd = timer;

	if (epoll_ctl(efd, EPOLL_CTL_ADD, timer, &timer_event) != 0) {
		close(efd);
		close(timer);
		abort();
	}

	char** env = prep_enviroment(context->info->display, context->info->x_auth, context->info->home_dir);

	feh_exec(context->info->options->feh_path, context->info->options->bg_style, current->image, env);
	current = current->next;

	int running = 0;

	struct epoll_event events[3];

	while(running == 0) {
		//things
		int num_of_events = epoll_wait(efd, events, 3, 10000);

		for (int i = 0; i < num_of_events; i++) {
			if (events[i].data.fd == sig_fd) {
				struct signalfd_siginfo fdsi;
				int bytes_read = read(events[i].data.fd, &fdsi, sizeof(fdsi));

				if (bytes_read == 0) {
					//not valid read
				}

				if (fdsi.ssi_signo == SIGHUP) {
					//regen config
				}
			}
			else if (events[i].data.fd == timer) {
				#ifdef DEBUG
				fprintf(stderr, "timer expired\r\n");
				#endif

				uint64_t expires;
				read(events[i].data.fd, &expires, sizeof(uint64_t));
				if (expires >= 1) {
					feh_exec(context->info->options->feh_path, context->info->options->bg_style, current->image, env);
					current = current->next;
				}
			}
		}
	}

	return 0;
}

static inline void feh_exec(const char* path, const char* bg_style, const char* image, char** env) {

#ifdef DEBUG
	fprintf(stderr, "feh exec fired\r\n");
	fprintf(stderr, "path: %s\r\nbg: %s\r\nimage: %s\r\n", path, bg_style, image);
#endif

	pid_t feh_pid;

	feh_pid = fork();

	if (feh_pid < 0) {
		abort();
	}

	if (feh_pid == 0) {
		execle(path, "walld-feh", bg_style, "--no-fehbg", image, NULL, env);
	}
}

char** prep_enviroment(const char* display, const char* x_auth, const char* home) {
	int count = 0;

	if (display != NULL) {
		count++;
	}
	else {
		//display must be included even if hardcoded
		count++;
	}

	if (x_auth != NULL) {
		count++;
	}

	if (home != NULL) {
		count++;
	}

	count++;

	char** env = malloc(count * sizeof(char*));

	int index = 0;

	if (display != NULL) {
		int len = snprintf(NULL, 0, "%s%s", "DISPLAY=", display);

		if (len <= 0) {
			abort();
		}

		char* display_string = malloc(len + 1U);

		if (display_string == NULL) {
			abort();
		}

		snprintf(display_string, len + 1U, "%s%s", "DISPLAY=", display);

		env[index] = display_string;

		#ifdef DEBUG
		fprintf(stderr, "display: %s\r\n", display_string);
		#endif

		index++;
	}
	else {
		env[index] = "DISPLAY=:0";

		index++;
	}

	if (x_auth != NULL) {
		int len = snprintf(NULL, 0, "%s%s", "XAUTHORITY=", x_auth);

		if (len <= 0) {
			abort();
		}

		char* x_auth_string = malloc(len + 1U);

		if (x_auth_string == NULL) {
			abort();
		}

		snprintf(x_auth_string, len + 1U, "%s%s", "XAUTHORITY=", x_auth);

		env[index] = x_auth_string;

		#ifdef DEBUG
		fprintf(stderr, "x_auth: %s\r\n", x_auth_string);
		#endif

		index++;
	}

	if (home != NULL) {
		int len = snprintf(NULL, 0, "%s%s", "HOME=", home);

		if (len <= 0) {
			abort();
		}

		char* home_string = malloc(len + 1U);

		if (home_string == NULL) {
			abort();
		}

		snprintf(home_string, len + 1U, "%s%s", "HOME=", home);

		env[index] = home_string;

		#ifdef DEBUG
		fprintf(stderr, "home: %s\r\n", home_string);
		#endif

		index++;
	}

	env[index] = NULL;

	return env;
}
