
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "common/exec.h"

bool exec(const char* file, char* const* argv, char* out, int out_len) {
    int err_pipe[2];
    pipe(err_pipe);
    fcntl(err_pipe[0], F_SETFL, fcntl(err_pipe[0], F_GETFL) | O_NONBLOCK);
    int cid = fork();
    if(cid == 0) {
        dup2(err_pipe[1], STDERR_FILENO);
        dup2(err_pipe[1], STDOUT_FILENO);
        execvp(file, argv);
        fprintf(stderr, "Failed to execute 'cc'\n");
        exit(EXIT_FAILURE);
    } else if(cid > 0) {
        int stat;
        waitpid(cid, &stat, 0);
        if(stat == 0) {
            return false;
        } else {
            int read_len = read(err_pipe[0], out, out_len-1);
            out[read_len - 1] = 0;
            close(err_pipe[0]);
            close(err_pipe[1]);
            return true;
        }
    } else {
        strncpy(out, "Failed to fork", out_len);
        close(err_pipe[0]);
        close(err_pipe[1]);
        return true;
    }
}

