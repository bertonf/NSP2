#ifndef SPATCH_H
#define SPATCH_H

#define SSHD_USER "libssh"
#define SSHD_PASSWORD "libssh"

#define KEYS_FOLDER "/etc/ssh/"

typedef struct s_threadList
{
    pthread_t thread;
    ssh_session session;
    struct s_threadList* next;
} sthreadList;

#endif // SPATCH_H
