#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libssh/libssh.h>

#ifdef _WIN32
#define strncasecmp _strnicmp
#endif

int verify_knownhost(ssh_session session)
{
    int state;
    unsigned char *hash = NULL;
    int hlen;

    state=ssh_is_server_known(session);
    hlen = ssh_get_pubkey_hash(session, &hash);
    if (hlen < 0)
    {
        return -1;
    }
    switch(state)
    {
    case SSH_SERVER_KNOWN_OK:
        break; /* ok */
    case SSH_SERVER_FILE_NOT_FOUND:
    case SSH_SERVER_NOT_KNOWN:
        if (ssh_write_knownhost(session) < 0) {
            free(hash);
            fprintf(stderr, "error %s\n", strerror(errno));
            return -1;
        }
        break;
    case SSH_SERVER_ERROR:
        free(hash);
        fprintf(stderr,"%s",ssh_get_error(session));
        return -1;
    default:
        return -1;
    }
    free(hash);
    return 0;
}
