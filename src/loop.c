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

#include "loop.h"

int event_loop_run(loop_context* context) {
	linked_node* current = context->info->picture_list;

	int efd = epoll_create1(EPOLL_CLOEXEC);

	if (efd <=0) {
		abort();
	}

	int sig_fd = signalfd(-1, &context->sigs, SFD_CLOEXEC);

	struct epoll_event signal_event;
	signal_event.events = EPOLLIN;
	signal_event.data.fd = sig_fd;

	if (epoll_ctl(efd, EPOLL_CTL_ADD, sig_fd, &signal_event) != 0) {
		close(sig_fd);
		close(efd);
		abort();
	}

	int timer = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC);

	struct itimerspec timespec;

	long seconds = context->info->options->minutes * 60;

	timespec.it_value.tv_sec = seconds;
	timespec.it_value.tv_nsec = 0;
	timespec.it_interval.tv_sec = seconds;
	timespec.it_interval.tv_nsec = 0;

	if (timerfd_settime(timer, 0, &timespec, NULL) != 0) {
		close(sig_fd);
		close(efd);
		close(timer);
		abort();
	}

	struct epoll_event timer_event;
	timer_event.events = EPOLLIN;
	timer_event.data.fd = timer;

	if (epoll_ctl(efd, EPOLL_CTL_ADD, timer, &timer_event) != 0) {
		close(sig_fd);
		close(efd);
		close(timer);
		abort();
	}

	size_t env_len = 0;

	char** env = prep_enviroment(context->info->display, context->info->x_auth, context->info->home_dir, &env_len);

	size_t xrdb_len = 0;

	char** xrdb_argv = prep_xrdb_argv(context->info->options->xrdb_argv, &xrdb_len);

	feh_exec(context->info->options->feh_path, context->info->options->bg_style, current->image, env);
	if (context->info->options->colors != 0) {
		write_color_file(context->info->home_dir, current->image, context->info->options->dark);

		if (context->info->options->xrdb_use != 0) {
			xrdb_exec(context->info->options->xrdb_path, xrdb_argv);
		}
	}
	current = current->next;

	int running = 0;

	struct epoll_event events[1];

	while(running == 0) {
		//things
		int num_of_events = epoll_wait(efd, events, 1, 10000);

		for (int i = 0; i < num_of_events; i++) {
			if (events[i].data.fd == sig_fd) {
				struct signalfd_siginfo* fdsi;
				fdsi = malloc(sizeof(struct signalfd_siginfo));
				int bytes_read = 0;
				bytes_read = read(events[i].data.fd, fdsi, sizeof(struct signalfd_siginfo));

				if (fdsi->ssi_signo == SIGTERM) {
					syslog(LOG_NOTICE, "SIGTERM has been recived: Quiting");
					running = 1;
				}
				else if (fdsi->ssi_signo == SIGQUIT) {
					syslog(LOG_NOTICE, "SIGQUIT has been recived: Quiting");
					running = 1;
				}
				else if (fdsi->ssi_signo == SIGINT) {
					syslog(LOG_NOTICE, "SIGINT has been recived: Quiting");
					running = 1;
				}
				else if (fdsi->ssi_signo == SIGHUP) {

					syslog(LOG_NOTICE, "SIGHUP has been recived: regening config");

					//regen config
					context->info = regen_config(context->info);

					seconds = context->info->options->minutes * 60;

					timespec.it_value.tv_sec = seconds;
					timespec.it_value.tv_nsec = 0;
					timespec.it_interval.tv_sec = seconds;
					timespec.it_interval.tv_nsec = 0;

					if (timerfd_settime(timer, 0, &timespec, NULL) != 0) {
						abort();
					}

					free_env(env, env_len);

					env = prep_enviroment(context->info->display, context->info->x_auth, context->info->home_dir, &env_len);

					free_env(xrdb_argv, xrdb_len);

					xrdb_argv = prep_xrdb_argv(context->info->options->xrdb_argv, &xrdb_len);

					current = context->info->picture_list;

					feh_exec(context->info->options->feh_path, context->info->options->bg_style, current->image, env);
					if (context->info->options->colors != 0) {
						write_color_file(context->info->home_dir, current->image, context->info->options->dark);

						if (context->info->options->xrdb_use != 0) {
							xrdb_exec(context->info->options->xrdb_path, xrdb_argv);
						}
					}
					current = current->next;

					syslog(LOG_NOTICE, "config regened");

				}
				else if (fdsi->ssi_signo == SIGUSR1) {
					syslog(LOG_NOTICE, "SIGUSR1 has been recived: skiping ahead");
					//skip ahead
					feh_exec(context->info->options->feh_path, context->info->options->bg_style, current->image, env);
					if (context->info->options->colors != 0) {
						write_color_file(context->info->home_dir, current->image, context->info->options->dark);

						if (context->info->options->xrdb_use != 0) {
							xrdb_exec(context->info->options->xrdb_path, xrdb_argv);
						}
					}
					current = current->next;
				}
				else if (fdsi->ssi_signo == SIGUSR2) {
					//re shuffle the picture list
					syslog(LOG_NOTICE, "SIGUSR2 has been recived: reshuffling picture list");
					current = shuffle(current);

					feh_exec(context->info->options->feh_path, context->info->options->bg_style, current->image, env);
					if (context->info->options->colors != 0) {
						write_color_file(context->info->home_dir, current->image, context->info->options->dark);

						if (context->info->options->xrdb_use != 0) {
							xrdb_exec(context->info->options->xrdb_path, xrdb_argv);
						}
					}
					current = current->next;
				}

				free(fdsi);
			}
			else if (events[i].data.fd == timer) {

				uint64_t expires;
				read(events[i].data.fd, &expires, sizeof(uint64_t));
				if (expires > 0) {
					feh_exec(context->info->options->feh_path, context->info->options->bg_style, current->image, env);
					if (context->info->options->colors != 0) {
						write_color_file(context->info->home_dir, current->image, context->info->options->dark);

						if (context->info->options->xrdb_use != 0) {
							xrdb_exec(context->info->options->xrdb_path, xrdb_argv);
						}
					}
					current = current->next;
				}
			}
		}
	}

	free_env(env, env_len);
	free_env(xrdb_argv, xrdb_len);

	close(timer);
	close(sig_fd);
	close(efd);

	return 0;
}

static inline void feh_exec(const char* path, const char* bg_style, const char* image, char** env) {

	pid_t feh_pid;

	feh_pid = fork();

	if (feh_pid < 0) {
		abort();
	}

	if (feh_pid == 0) {
		execle(path, "walld-feh", bg_style, "--no-fehbg", image, NULL, env);
	}
}

static inline void write_color_file(const char* home_dir, const char* image, int dark) {
	put_colors_in_file(home_dir, image, dark);
}

static inline void xrdb_exec(const char* path, char** xrdb_argv) {
	pid_t xrdb_pid;

	xrdb_pid = fork();

	if (xrdb_pid < 0) {
		abort();
	}

	if (xrdb_pid == 0) {
		execvp(path, xrdb_argv);
	}
}

char** prep_enviroment(const char* display, const char* x_auth, const char* home, size_t* size) {
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

	*size = count;

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

		index++;
	}
	else {
		env[index] = strdup("DISPLAY=:0");

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

		index++;
	}

	env[index] = NULL;

	return env;
}

pre_init_stuff* regen_config(pre_init_stuff* info) {
	//free the old options

	if (info->options != NULL) {
		if (info->options->feh_path != NULL) {
			free(info->options->feh_path);
		}

		if (info->options->bg_style != NULL) {
			free(info->options->bg_style);
		}

		if (info->options->x_auth != NULL) {
			free(info->options->x_auth);
		}

		if (info->options->display != NULL) {
			free(info->options->display);
		}

		if (info->options->sources != NULL) {
			free_list(info->options->sources);
		}

		if (info->options->xrdb_path != NULL) {
			free(info->options->xrdb_path);
		}

		if (info->options->xrdb_argv != NULL) {
			free_list(info->options->xrdb_argv);
		}


		free(info->options);
	}

	info->options = read_config(info->config, info->home_dir);

	if (info->options->x_auth != NULL) {
		if (info->x_auth != NULL) {
			free(info->x_auth);
		}

		info->x_auth = info->options->x_auth;
	}

	if (info->options->display != NULL) {
		if (info->display != NULL) {
			free(info->display);
		}

		info->display = info->options->display;
	}

	free_circular_list(info->picture_list);

	info->picture_list = get_images(info->options->sources);

	if (info->picture_list == NULL) {
		abort();
	}

	info->picture_list = shuffle(info->picture_list);

	return info;
}

void free_env(char** env, size_t env_len) {
	if (env != NULL) {
		//size_t env_len = sizeof(env) / sizeof(*env);
		for (size_t i = 0; i < env_len; i++) {
			if (env[i] != NULL) {
				free(env[i]);
			}
		}
		free(env);
	}
}

char** prep_xrdb_argv(linked_node* node, size_t* xrdb_len) {
	return list_to_null_termed_string_array(node, xrdb_len);
}
