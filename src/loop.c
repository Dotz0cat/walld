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

// int event_loop_run(loop_context* context) {
// 	linked_node* current = context->info->picture_list;

// 	int efd = epoll_create1(EPOLL_CLOEXEC);

// 	if (efd <=0) {
// 		abort();
// 	}

// 	int sig_fd = signalfd(-1, &context->sigs, SFD_CLOEXEC);

// 	struct epoll_event signal_event;
// 	signal_event.events = EPOLLIN;
// 	signal_event.data.fd = sig_fd;

// 	if (epoll_ctl(efd, EPOLL_CTL_ADD, sig_fd, &signal_event) != 0) {
// 		close(sig_fd);
// 		close(efd);
// 		abort();
// 	}

// 	int timer = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC);

// 	struct itimerspec timespec;

// 	long seconds;

// 	if (context->info->time > 0) {
// 		seconds = context->info->time * 60;
// 	}
// 	else if (context->info->options->minutes <= 0) {
// 		seconds = 30 * 60;
// 	}
// 	else {
// 		seconds = context->info->options->minutes * 60;
// 	}
	

// 	timespec.it_value.tv_sec = seconds;
// 	timespec.it_value.tv_nsec = 0;
// 	timespec.it_interval.tv_sec = seconds;
// 	timespec.it_interval.tv_nsec = 0;

// 	if (timerfd_settime(timer, 0, &timespec, NULL) != 0) {
// 		close(sig_fd);
// 		close(efd);
// 		close(timer);
// 		abort();
// 	}

// 	struct epoll_event timer_event;
// 	timer_event.events = EPOLLIN;
// 	timer_event.data.fd = timer;

// 	if (epoll_ctl(efd, EPOLL_CTL_ADD, timer, &timer_event) != 0) {
// 		close(sig_fd);
// 		close(efd);
// 		close(timer);
// 		abort();
// 	}

// 	size_t env_len = 0;

// 	char** env = prep_enviroment(context->info->display, context->info->x_auth, context->info->home_dir, &env_len);

// 	size_t xrdb_len = 0;

// 	char** xrdb_argv = prep_xrdb_argv(context->info->options->xrdb_argv, &xrdb_len);

// 	feh_exec(context->info->options->feh_path, context->info->options->bg_style, current->image, env);
// 	if (context->info->options->colors != 0) {
// 		write_color_file(context->info->home_dir, current->image, context->info->options->dark);

// 		if (context->info->options->xrdb_use != 0) {
// 			xrdb_exec(context->info->options->xrdb_path, xrdb_argv);
// 		}
// 	}
// 	current = current->next;

// 	int running = 0;

// 	struct epoll_event events[3];

// 	while(running == 0) {
// 		//things
// 		int num_of_events = epoll_wait(efd, events, 3, 10000);

// 		for (int i = 0; i < num_of_events; i++) {
// 			if (events[i].data.fd == sig_fd) {
// 				struct signalfd_siginfo* fdsi;
// 				fdsi = malloc(sizeof(struct signalfd_siginfo));
// 				int bytes_read = 0;
// 				bytes_read = read(events[i].data.fd, fdsi, sizeof(struct signalfd_siginfo));

// 				if (fdsi->ssi_signo == SIGTERM) {
// 					syslog(LOG_NOTICE, "SIGTERM has been recived: Quiting");
// 					running = 1;
// 				}
// 				else if (fdsi->ssi_signo == SIGQUIT) {
// 					syslog(LOG_NOTICE, "SIGQUIT has been recived: Quiting");
// 					running = 1;
// 				}
// 				else if (fdsi->ssi_signo == SIGINT) {
// 					syslog(LOG_NOTICE, "SIGINT has been recived: Quiting");
// 					running = 1;
// 				}
// 				else if (fdsi->ssi_signo == SIGHUP) {

// 					syslog(LOG_NOTICE, "SIGHUP has been recived: regening config");

// 					//regen config
// 					context->info = regen_config(context->info);

// 					if (context->info->time > 0) {
// 						seconds = context->info->time * 60;
// 					}
// 					else if (context->info->options->minutes <= 0) {
// 						seconds = 30 * 60;
// 					}
// 					else {
// 						seconds = context->info->options->minutes * 60;
// 					}

// 					timespec.it_value.tv_sec = seconds;
// 					timespec.it_value.tv_nsec = 0;
// 					timespec.it_interval.tv_sec = seconds;
// 					timespec.it_interval.tv_nsec = 0;

// 					if (timerfd_settime(timer, 0, &timespec, NULL) != 0) {
// 						abort();
// 					}

// 					free_env(env, env_len);

// 					env = prep_enviroment(context->info->display, context->info->x_auth, context->info->home_dir, &env_len);

// 					free_env(xrdb_argv, xrdb_len);

// 					xrdb_argv = prep_xrdb_argv(context->info->options->xrdb_argv, &xrdb_len);

// 					current = context->info->picture_list;

// 					feh_exec(context->info->options->feh_path, context->info->options->bg_style, current->image, env);
// 					if (context->info->options->colors != 0) {
// 						write_color_file(context->info->home_dir, current->image, context->info->options->dark);

// 						if (context->info->options->xrdb_use != 0) {
// 							xrdb_exec(context->info->options->xrdb_path, xrdb_argv);
// 						}
// 					}
// 					current = current->next;

// 					syslog(LOG_NOTICE, "config regened");

// 				}
// 				else if (fdsi->ssi_signo == SIGUSR1) {
// 					syslog(LOG_NOTICE, "SIGUSR1 has been recived: skiping ahead");
// 					//skip ahead
// 					feh_exec(context->info->options->feh_path, context->info->options->bg_style, current->image, env);
// 					if (context->info->options->colors != 0) {
// 						write_color_file(context->info->home_dir, current->image, context->info->options->dark);

// 						if (context->info->options->xrdb_use != 0) {
// 							xrdb_exec(context->info->options->xrdb_path, xrdb_argv);
// 						}
// 					}
// 					current = current->next;

// 					//reprime timer
// 					if (context->info->time > 0) {
// 						seconds = context->info->time * 60;
// 					}
// 					else if (context->info->options->minutes <= 0) {
// 						seconds = 30 * 60;
// 					}
// 					else {
// 						seconds = context->info->options->minutes * 60;
// 					}

// 					timespec.it_value.tv_sec = seconds;
// 					timespec.it_value.tv_nsec = 0;
// 					timespec.it_interval.tv_sec = seconds;
// 					timespec.it_interval.tv_nsec = 0;

// 					if (timerfd_settime(timer, 0, &timespec, NULL) != 0) {
// 						abort();
// 					}
// 				}
// 				else if (fdsi->ssi_signo == SIGUSR2) {
// 					//re shuffle the picture list
// 					syslog(LOG_NOTICE, "SIGUSR2 has been recived: reshuffling picture list");
// 					current = shuffle(current);

// 					feh_exec(context->info->options->feh_path, context->info->options->bg_style, current->image, env);
// 					if (context->info->options->colors != 0) {
// 						write_color_file(context->info->home_dir, current->image, context->info->options->dark);

// 						if (context->info->options->xrdb_use != 0) {
// 							xrdb_exec(context->info->options->xrdb_path, xrdb_argv);
// 						}
// 					}
// 					current = current->next;

// 					//reprime timer
// 					if (context->info->time > 0) {
// 						seconds = context->info->time * 60;
// 					}
// 					else if (context->info->options->minutes <= 0) {
// 						seconds = 30 * 60;
// 					}
// 					else {
// 						seconds = context->info->options->minutes * 60;
// 					}

// 					timespec.it_value.tv_sec = seconds;
// 					timespec.it_value.tv_nsec = 0;
// 					timespec.it_interval.tv_sec = seconds;
// 					timespec.it_interval.tv_nsec = 0;

// 					if (timerfd_settime(timer, 0, &timespec, NULL) != 0) {
// 						abort();
// 					}
// 				}

// 				free(fdsi);
// 			}
// 			else if (events[i].data.fd == timer) {

// 				uint64_t expires;
// 				read(events[i].data.fd, &expires, sizeof(uint64_t));
// 				if (expires > 0) {
// 					feh_exec(context->info->options->feh_path, context->info->options->bg_style, current->image, env);
// 					if (context->info->options->colors != 0) {
// 						write_color_file(context->info->home_dir, current->image, context->info->options->dark);

// 						if (context->info->options->xrdb_use != 0) {
// 							xrdb_exec(context->info->options->xrdb_path, xrdb_argv);
// 						}
// 					}
// 					current = current->next;
// 				}
// 			}
// 		}
// 	}

// 	free_env(env, env_len);
// 	free_env(xrdb_argv, xrdb_len);

// 	close(timer);
// 	close(sig_fd);
// 	close(efd);

// 	return 0;
// }

int event_loop_run(loop_context* context) {
	context->current = context->info->picture_list;

	context->event_box = malloc(sizeof(events_box));

	context->event_box->base = event_base_new();

	if (!context->event_box->base) {
		//if it breaks here I cant help
		abort();
	}

	context->event_box->signal_sigquit = evsignal_new(context->event_box->base, SIGQUIT, sig_int_quit_term_cb, (void*) context);
	if (!context->event_box->signal_sigquit || event_add(context->event_box->signal_sigquit, NULL) < 0) abort();

	context->event_box->signal_sigterm = evsignal_new(context->event_box->base, SIGTERM, sig_int_quit_term_cb, (void*) context);
	if (!context->event_box->signal_sigterm || event_add(context->event_box->signal_sigterm, NULL) < 0) abort();

	context->event_box->signal_sigint = evsignal_new(context->event_box->base, SIGINT, sig_int_quit_term_cb, (void*) context);
	if (!context->event_box->signal_sigint || event_add(context->event_box->signal_sigint, NULL) < 0) abort();

	context->event_box->signal_sighup = evsignal_new(context->event_box->base, SIGHUP, sighup_cb, (void*) context);
	if (!context->event_box->signal_sighup || event_add(context->event_box->signal_sighup, NULL) < 0) abort();

	context->event_box->signal_sigusr1 = evsignal_new(context->event_box->base, SIGUSR1, sigusr1_cb, (void*) context);
	if (!context->event_box->signal_sigusr1 || event_add(context->event_box->signal_sigusr1, NULL) < 0) abort();

	context->event_box->signal_sigusr2 = evsignal_new(context->event_box->base, SIGUSR2, sigusr2_cb, (void*) context);
	if (!context->event_box->signal_sigusr2 || event_add(context->event_box->signal_sigusr2, NULL) < 0) abort();

	context->event_box->timer = event_new(context->event_box->base, -1, EV_PERSIST, timer_expire_cb, (void*) context);

	context->seconds = malloc(sizeof(struct timeval));

	if (context->info->time > 0) {
		context->seconds->tv_sec = context->info->time * 60;
	}
	else if (context->info->options->minutes <= 0) {
		context->seconds->tv_sec = 30 * 60;
	}
	else {
		context->seconds->tv_sec = context->info->options->minutes * 60;
	}

	if (!context->event_box->timer || event_add(context->event_box->timer, context->seconds) < 0) abort();

	context->env_len = 0;

	context->env = prep_enviroment(context->info->display, context->info->x_auth, context->info->home_dir, &(context->env_len));

	context->xrdb_len = 0;

	context->xrdb_argv = prep_xrdb_argv(context->info->options->xrdb_argv, &(context->xrdb_len));

	feh_exec(context->info->options->feh_path, context->info->options->bg_style, context->current->image, context->env);
	if (context->info->options->colors != 0) {
		write_color_file(context->info->home_dir, context->current->image, context->info->options->dark);

		if (context->info->options->xrdb_use != 0) {
			xrdb_exec(context->info->options->xrdb_path, context->xrdb_argv);
		}
	}
	context->current = context->current->next;



	event_base_loop(context->event_box->base, EVLOOP_NO_EXIT_ON_EMPTY);

	//clean up
	event_del(context->event_box->signal_sigquit);
	event_del(context->event_box->signal_sigint);
	event_del(context->event_box->signal_sigterm);
	event_del(context->event_box->signal_sighup);
	event_del(context->event_box->signal_sigusr1);
	event_del(context->event_box->signal_sigusr2);
	event_del(context->event_box->timer);

	event_base_free(context->event_box->base);

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

	info->options = read_config(info->config, info->home_dir, info->source);

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

//I know the name is bad. 
static void sig_int_quit_term_cb(evutil_socket_t sig, short events, void* user_data) {
	loop_context* context = (loop_context*) user_data;
	//do I even want the delay
	struct timeval delay = {1, 0};

	event_base_loopexit(context->event_box->base, &delay);
}

static void sighup_cb(evutil_socket_t sig, short events, void* user_data) {
	loop_context* context = (loop_context* ) user_data;

	syslog(LOG_NOTICE, "SIGHUP has been recived: regening config");

	//regen config
	context->info = regen_config(context->info);

	if (context->info->time > 0) {
		context->seconds->tv_sec = context->info->time * 60;
	}
	else if (context->info->options->minutes <= 0) {
		context->seconds->tv_sec = 30 * 60;
	}
	else {
		context->seconds->tv_sec = context->info->options->minutes * 60;
	}

	//reprime time
	event_del(context->event_box->timer);

	event_add(context->event_box->timer, context->seconds);

	//rebuild enviroment
	free_env(context->env, context->env_len);

	context->env = prep_enviroment(context->info->display, context->info->x_auth, context->info->home_dir, &(context->env_len));

	free_env(context->xrdb_argv, context->xrdb_len);

	context->xrdb_argv = prep_xrdb_argv(context->info->options->xrdb_argv, &(context->xrdb_len));

	context->current = context->info->picture_list;

	feh_exec(context->info->options->feh_path, context->info->options->bg_style, context->current->image, context->env);
	if (context->info->options->colors != 0) {
		write_color_file(context->info->home_dir, context->current->image, context->info->options->dark);

		if (context->info->options->xrdb_use != 0) {
			xrdb_exec(context->info->options->xrdb_path, context->xrdb_argv);
		}
	}
	context->current = context->current->next;

	syslog(LOG_NOTICE, "config regened");
}

static void sigusr1_cb(evutil_socket_t sig, short events, void* user_data) {
	loop_context* context = (loop_context*) user_data;

	syslog(LOG_NOTICE, "SIGUSR1 has been recived: skiping ahead");
	//skip ahead
	feh_exec(context->info->options->feh_path, context->info->options->bg_style, context->current->image, context->env);
	if (context->info->options->colors != 0) {
		write_color_file(context->info->home_dir, context->current->image, context->info->options->dark);

		if (context->info->options->xrdb_use != 0) {
			xrdb_exec(context->info->options->xrdb_path, context->xrdb_argv);
		}
	}
	context->current = context->current->next;

	//reprime timer
	if (context->info->time > 0) {
		context->seconds->tv_sec = context->info->time * 60;
	}
	else if (context->info->options->minutes <= 0) {
		context->seconds->tv_sec = 30 * 60;
	}
	else {
		context->seconds->tv_sec = context->info->options->minutes * 60;
	}

	// timer stuff
	event_del(context->event_box->timer);

	event_add(context->event_box->timer, context->seconds);


}

static void sigusr2_cb(evutil_socket_t sig, short events, void* user_data) {
	loop_context* context = (loop_context*) user_data;

	//re shuffle the picture list
	syslog(LOG_NOTICE, "SIGUSR2 has been recived: reshuffling picture list");
	context->current = shuffle(context->current);

	feh_exec(context->info->options->feh_path, context->info->options->bg_style, context->current->image, context->env);
	if (context->info->options->colors != 0) {
		write_color_file(context->info->home_dir, context->current->image, context->info->options->dark);

		if (context->info->options->xrdb_use != 0) {
			xrdb_exec(context->info->options->xrdb_path, context->xrdb_argv);
		}
	}
	context->current = context->current->next;

	//reprime timer
	if (context->info->time > 0) {
		context->seconds->tv_sec = context->info->time * 60;
	}
	else if (context->info->options->minutes <= 0) {
		context->seconds->tv_sec = 30 * 60;
	}
	else {
		context->seconds->tv_sec = context->info->options->minutes * 60;
	}

	//timer stuff
	event_del(context->event_box->timer);

	event_add(context->event_box->timer, context->seconds);

}

static void timer_expire_cb(evutil_socket_t fd, short events, void* user_data) {
	loop_context* context = (loop_context*) user_data;

	feh_exec(context->info->options->feh_path, context->info->options->bg_style, context->current->image, context->env);
	if (context->info->options->colors != 0) {
		write_color_file(context->info->home_dir, context->current->image, context->info->options->dark);

		if (context->info->options->xrdb_use != 0) {
			xrdb_exec(context->info->options->xrdb_path, context->xrdb_argv);
		}
	}
	context->current = context->current->next;
}
