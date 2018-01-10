#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <string.h>

extern char **environ;

int main(int argc, char *argv[])
{
    pid_t pid = fork();

    if (pid < 0)
    {
        exit(EXIT_FAILURE);
    }
    if (pid == 0)
    {

        int ret;
        char *cmd[] = {"dotnet", "./subapp/bin/Debug/netcoreapp2.0/publish/subapp.dll", (char *)0};
        ret = execve("/usr/local/share/dotnet/dotnet", cmd, environ);
        if (ret < 0)
        {
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        
        unlink("./temp.sock");
        int socket_fd = socket(PF_UNIX, SOCK_STREAM, 0);
        if (socket_fd < 0)
        {
            perror("opening stream socket");
            exit(EXIT_FAILURE);
        }

        struct sockaddr_un address;
        memset(&address, 0, sizeof(struct sockaddr_un));
        address.sun_family = AF_UNIX;
        snprintf(address.sun_path, sizeof(address.sun_path), "temp.sock");

        if (bind(socket_fd, (struct sockaddr *)&address, sizeof(struct sockaddr_un)) != 0)
        {
            perror("binding stream socket");
            exit(EXIT_FAILURE);
        }

        if (listen(socket_fd, 5) != 0)
        {
            perror("listen");
            exit(EXIT_FAILURE);
        }

        char buf[1024];
        int rval;
        int msgsock = accept(socket_fd, 0, 0);
        if (msgsock == -1)
        {
            perror("accept");
        }
        else
        {
            do
            {
                bzero(buf, sizeof(buf));
                if ((rval = read(msgsock, buf, 1024)) < 0)
                    perror("reading stream message");
                else if (rval == 0)
                {
                    printf("Ending connection\n");
                    break;
                }
                else
                    printf("-->%s\n", buf);
            } while (rval > 0);
        }
        close(msgsock);

        int status;
        wait(&status);
        if (WIFSIGNALED(status))
        {
            exit(EXIT_FAILURE);
        }
    }
}
