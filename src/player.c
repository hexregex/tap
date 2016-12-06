#include <stdio.h>

#include "player.h"
#include "ffmpeg.h"


static inline void plr_load_lib() {
    /* TODO: Load the functions pointed to dynamically. */
    /* Currently defined in ffmpeg.c */
    plr_init = &ff_init;
    plr_open = &ff_open;
    plr_play = &ff_play;
    plr_pause = &ff_pause;
    plr_next = &ff_next;
    plr_previous = &ff_previous;
    plr_repeat = &ff_repeat;
    plr_seek = &ff_seek;
}

void *plr_thread_go(void *thread_arg)
{
    char **in_filename = (char **)thread_arg;

    plr_load_lib();

    plr_init();
    plr_open(in_filename[1]);
    plr_play();

    /* TODO: What is a useful value to return on thread termination? */
    return NULL;
}
