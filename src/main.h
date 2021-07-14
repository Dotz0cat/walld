#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <syslog.h>

#include "config.h"
#include "list.h"
#include "loop.h"

static void init_daemon(void);
static pre_init_stuff* pre_init(void);

#endif /*__MAIN_H__*/
