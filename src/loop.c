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

#include "loop.h"

int event_loop_run(loop_context* context) {
	context->current = context->info->picture_list;

	//set feh path from $PATH if it was not put into config
	//may guard against a use after free. I dont want to find out
	if (context->info->options->feh_path != NULL) {
		context->feh_path = strdup(context->info->options->feh_path);
	}
	else {
		context->feh_path = strdup(context->info->feh_path);
	}

	if (context->info->options->xrdb_path != NULL) {
		context->xrdb_path = strdup(context->info->options->xrdb_path);
	}
	else {
		context->xrdb_path = strdup(context->info->xrdb_path);
	}

	context->event_box = malloc(sizeof(events_box));

	context->event_box->base = event_base_new();

	if (!context->event_box->base) {
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
	else if (context->info->options->minutes < 0) {
		context->seconds->tv_sec = 30 * 60;
	}
	else {
		context->seconds->tv_sec = context->info->options->minutes * 60;
	}

	if (!context->event_box->timer || event_add(context->event_box->timer, context->seconds) < 0) abort();

	context->feh_len = 0;

	if (context->info->monitors > 0) {
		context->feh_argv = prep_feh_argv(context->info->options->bg_style, context->current, context->info->monitors, &(context->feh_len));
	}
	else if (context->info->options->monitors <= 0) {
		context->feh_argv = prep_feh_argv(context->info->options->bg_style, context->current, 1, &(context->feh_len));
	}
	else {
		context->feh_argv = prep_feh_argv(context->info->options->bg_style, context->current, context->info->options->monitors, &(context->feh_len));
	}

	context->env_len = 0;

	context->env = prep_enviroment(context->info->display, context->info->x_auth, context->info->home_dir, &(context->env_len));

	context->xrdb_len = 0;

	context->xrdb_argv = prep_xrdb_argv(context->info->options->xrdb_argv, &(context->xrdb_len));

	feh_exec(context->feh_path, context->feh_argv, context->env);
	if (context->info->options->colors != 0) {
		write_color_file(context->info->home_dir, context->current->image, context->info->options->dark);

		if (context->info->options->xrdb_use != 0) {
			xrdb_exec(context->xrdb_path, context->xrdb_argv);
		}
	}

	if (context->info->monitors > 0) {
		context->current = wind_to_x(context->current, context->info->monitors);
	}
	else if (context->info->options->monitors <= 0) {
		context->current = wind_to_x(context->current, 1);
	}
	else {
		context->current = wind_to_x(context->current, context->info->options->monitors);
	}

	edit_feh_argv(context->feh_argv, context->current, context->feh_len);

	event_base_loop(context->event_box->base, EVLOOP_NO_EXIT_ON_EMPTY);

	//clean up
	event_del(context->event_box->signal_sigquit);
	event_del(context->event_box->signal_sigint);
	event_del(context->event_box->signal_sigterm);
	event_del(context->event_box->signal_sighup);
	event_del(context->event_box->signal_sigusr1);
	event_del(context->event_box->signal_sigusr2);
	event_del(context->event_box->timer);

	event_free(context->event_box->signal_sigquit);
	event_free(context->event_box->signal_sigint);
	event_free(context->event_box->signal_sigterm);
	event_free(context->event_box->signal_sighup);
	event_free(context->event_box->signal_sigusr1);
	event_free(context->event_box->signal_sigusr2);
	event_free(context->event_box->timer);

	event_base_free(context->event_box->base);

	free_feh_argv(context->feh_argv);
	free_env(context->env, context->env_len);
 	free_env(context->xrdb_argv, context->xrdb_len);

 	free(context->seconds);
 	free(context->event_box);

	return 0;
}

static inline void feh_exec(const char* path, char** feh_argv, char** env) {

	pid_t feh_pid;

	feh_pid = fork();

	if (feh_pid < 0) {
		abort();
	}

	if (feh_pid == 0) {
		execve(path, feh_argv, env);
		syslog(LOG_NOTICE, "feh was not in your path. the feh path can also be defined in the config");
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
		syslog(LOG_NOTICE, "xrdb was not in your path. the xrdb path can also be defined in the config");
		syslog(LOG_NOTICE, "Could xrdb not be installed?");
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

char** prep_feh_argv(const char* bg_style, linked_node* node, int monitors, size_t* feh_len) {
	char** feh_argv;

	//it needs 4 as a starting size.
	//1 for the argv1 "walld-feh" 1 for bg_style 1 for "--no-fehbg" 1 for null
	int count = 4;
	count = count + monitors;

	feh_argv = malloc(count * sizeof(char*));

	*feh_len = count;

	feh_argv[0] = strdup("walld-feh");
	feh_argv[1] = strdup(bg_style);
	feh_argv[2] = strdup("--no-fehbg");

	linked_node* add_head = node;

	for (int i = 3; i < count - 1; i++) {
		feh_argv[i] = add_head->image;
		add_head = add_head->next;
	}

	feh_argv[count - 1] = NULL;

	return feh_argv;
}

void edit_feh_argv(char** feh_argv, linked_node* node, size_t feh_len) {
	linked_node* add_head = node;

	for (int i = 3; i < (int) feh_len - 1; i++) {
		feh_argv[i] = add_head->image;
		add_head = add_head->next;
	}
}

void free_feh_argv(char** feh_argv) {
	//elements 3 to i-1 are owned by others i is null
	if (feh_argv != NULL) {
		for (size_t i = 0; i < 3; i++) {
			if (feh_argv[i] != NULL) {
				free(feh_argv[i]);
			}
		}
		free(feh_argv);
	}
}

static void sig_int_quit_term_cb(evutil_socket_t sig, short events, void* user_data) {
	loop_context* context = (loop_context*) user_data;

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
	else if (context->info->options->minutes < 0) {
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
	context->current = shuffle(context->current);

	free(context->feh_path);
	free(context->xrdb_path);

	//I love strdup, I really really love strdup
	//may guard against a use after free. I dont want to find out
	if (context->info->options->feh_path != NULL) {
		context->feh_path = strdup(context->info->options->feh_path);
	}
	else {
		context->feh_path = strdup(context->info->feh_path);
	}

	if (context->info->options->xrdb_path != NULL) {
		context->xrdb_path = strdup(context->info->options->xrdb_path);
	}
	else {
		context->xrdb_path = strdup(context->info->xrdb_path);
	}

	//rebuild feh argv
	free_feh_argv(context->feh_argv);

	if (context->info->monitors > 0) {
		context->feh_argv = prep_feh_argv(context->info->options->bg_style, context->current, context->info->monitors, &(context->feh_len));
	}
	else if (context->info->options->monitors <= 0) {
		context->feh_argv = prep_feh_argv(context->info->options->bg_style, context->current, 1, &(context->feh_len));
	}
	else {
		context->feh_argv = prep_feh_argv(context->info->options->bg_style, context->current, context->info->options->monitors, &(context->feh_len));
	}
	
	feh_exec(context->feh_path, context->feh_argv, context->env);
	if (context->info->options->colors != 0) {
		write_color_file(context->info->home_dir, context->current->image, context->info->options->dark);

		if (context->info->options->xrdb_use != 0) {
			xrdb_exec(context->xrdb_path, context->xrdb_argv);
		}
	}

	if (context->info->monitors > 0) {
		context->current = wind_to_x(context->current, context->info->monitors);
	}
	else if (context->info->options->monitors <= 0) {
		context->current = wind_to_x(context->current, 1);
	}
	else {
		context->current = wind_to_x(context->current, context->info->options->monitors);
	}
	

	edit_feh_argv(context->feh_argv, context->current, context->feh_len);

	syslog(LOG_NOTICE, "config regened");
}

static void sigusr1_cb(evutil_socket_t sig, short events, void* user_data) {
	loop_context* context = (loop_context*) user_data;

	syslog(LOG_NOTICE, "SIGUSR1 has been recived: skiping ahead");
	//skip ahead
	feh_exec(context->feh_path, context->feh_argv, context->env);
	if (context->info->options->colors != 0) {
		write_color_file(context->info->home_dir, context->current->image, context->info->options->dark);

		if (context->info->options->xrdb_use != 0) {
			xrdb_exec(context->xrdb_path, context->xrdb_argv);
		}
	}

	if (context->info->monitors > 0) {
		context->current = wind_to_x(context->current, context->info->monitors);
	}
	else if (context->info->options->monitors <= 0) {
		context->current = wind_to_x(context->current, 1);
	}
	else {
		context->current = wind_to_x(context->current, context->info->options->monitors);
	}

	edit_feh_argv(context->feh_argv, context->current, context->feh_len);

	//reprime timer
	if (context->info->time > 0) {
		context->seconds->tv_sec = context->info->time * 60;
	}
	else if (context->info->options->minutes < 0) {
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

	feh_exec(context->feh_path, context->feh_argv, context->env);
	if (context->info->options->colors != 0) {
		write_color_file(context->info->home_dir, context->current->image, context->info->options->dark);

		if (context->info->options->xrdb_use != 0) {
			xrdb_exec(context->xrdb_path, context->xrdb_argv);
		}
	}

	if (context->info->monitors > 0) {
		context->current = wind_to_x(context->current, context->info->monitors);
	}
	else if (context->info->options->monitors <= 0) {
		context->current = wind_to_x(context->current, 1);
	}
	else {
		context->current = wind_to_x(context->current, context->info->options->monitors);
	}

	edit_feh_argv(context->feh_argv, context->current, context->feh_len);

	//reprime timer
	if (context->info->time > 0) {
		context->seconds->tv_sec = context->info->time * 60;
	}
	else if (context->info->options->minutes < 0) {
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

	feh_exec(context->feh_path, context->feh_argv, context->env);
	if (context->info->options->colors != 0) {
		write_color_file(context->info->home_dir, context->current->image, context->info->options->dark);

		if (context->info->options->xrdb_use != 0) {
			xrdb_exec(context->xrdb_path, context->xrdb_argv);
		}
	}

	if (context->info->monitors > 0) {
		context->current = wind_to_x(context->current, context->info->monitors);
	}
	else if (context->info->options->monitors <= 0) {
		context->current = wind_to_x(context->current, 1);
	}
	else {
		context->current = wind_to_x(context->current, context->info->options->monitors);
	}

	edit_feh_argv(context->feh_argv, context->current, context->feh_len);
}
