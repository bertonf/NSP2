#ifndef CLIENTLISTNER_H
#define CLIENTLISTNER_H

//ssh_session connect_ssh(const char *host, charconst char *user, int port, int verbosity);
ssh_session connect_ssh(const char *host,  char* port,
                        const char *user, char* password, int verbosity);

#endif // CLIENTLISTNER_H
