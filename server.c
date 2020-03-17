#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#define LOG(format, ...) printf("\e[31m[SERVER]\e[0m " format "\n", ##__VA_ARGS__)

void usage() {
    fprintf(stderr, "Usage: ./server your_magic_number");
    exit(-1);
}

int main(int argc, char** argv) {
    if (argc < 2) usage();

    char buf[8];
    char* ans = argv[1];
    bool ansvec[10] = {0};

    ans[4] = '\0';
    for (size_t i = 0; i < 4; i++) {
        char c = ans[i] - '0';
        if (0 <= c && c <= 9 && !ansvec[c])
            ansvec[c] = true;
        else
            usage();
    }

    int pip[2][2];
    pipe(pip[0]);
    pipe(pip[1]);

    pid_t pid = fork();
    if (pid > 0) {
        close(pip[0][0]);
        close(pip[1][1]);
        LOG("server started, answer is %s, waiting for client", ans);

        for (int round = 1; ; round++) {
            read(pip[1][0], buf, 4);
            buf[4] = '\0';

            int cntA = 0, cntB = 0;
            for (size_t i = 0; i < 4; i++) {
                char c = buf[i] - '0';
                if (buf[i] == ans[i])
                    cntA++;
                else if (0 <= c && c <= 9 && ansvec[c])
                    cntB++;
            }
            LOG("round %d;\tyou guessed: %s;\tresult: %dA%dB", round, buf, cntA, cntB);
            sprintf(buf, "%d %d\n", cntA, cntB);
            write(pip[0][1], buf, 4);

            if (cntA == 4) {
                LOG("Voila! You win!! The answer is %s. You used %d rounds", ans, round);
                break;
            }
        }
        wait(NULL);
    } else if (pid == 0) {
        dup2(pip[0][0], STDIN_FILENO);
        dup2(pip[1][1], STDOUT_FILENO);
        close(pip[0][1]);
        close(pip[1][0]);
        execl("./client", "./client");
    } else {
        exit(-1);
    }
    return 0;
}