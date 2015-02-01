#ifndef SPATCH_H
#define SPATCH_H

#define SSHD_USER "libssh"
#define SSHD_PASSWORD "libssh"

#define KEYS_FOLDER "/etc/ssh/"
#define SERVER_PORT 25000


#include <libssh/libssh.h>
#include <libssh/server.h>
#include <libssh/callbacks.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <poll.h>
#include <pty.h>
#include <pthread.h>

typedef struct s_threadList
{
    pthread_t thread;
    ssh_session session;
    struct s_threadList* next;
} sthreadList;

int spatch();

#endif // SPATCH_H
