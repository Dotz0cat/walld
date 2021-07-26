#include "main.h"

int main(int argc, char** argv) {

	signal(SIGHUP, SIG_IGN);
	signal(SIGUSR1, SIG_IGN);

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

	if (sigprocmask(SIG_BLOCK, &sigs, NULL) != 0) {
		abort();
	}

	pre_init_stuff* info = pre_init();

	//init_daemon();

	// while(1) {
	// 	//stuff
	// 	syslog(LOG_NOTICE, "Walld is started");
	// 	sleep(20);
	// 	break;
	// }

	// syslog(LOG_NOTICE, "Walld has quit");
	// closelog();

	loop_context* context;
	context = malloc(sizeof(loop_context));

	context->info = info;

	context->sigs = sigs;

	event_loop_run(context);

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
