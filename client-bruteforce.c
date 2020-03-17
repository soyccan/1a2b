#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#define LOG(format, ...) fprintf(stderr, "[CLIENT] " format "\n", ##__VA_ARGS__)

int main() {
    bool usedvec[4][10] = {0};
    bool guessvec[10] = {0};
    char buf[5], guess[5];
    bool done = false;

    LOG("client start");
    for (int round = 0; !done; round++) {
        int i = 0;
        while (!done && i >= 0 && i <= 4) {
            LOG("i=%d", i);
            if (i == 4) {
                guess[i] = '\0';
                LOG("round %d;\tguessing %s", round, guess);

                write(STDOUT_FILENO, guess, 4);

                int cntA = 0, cntB = 0;
                read(STDIN_FILENO, buf, 4);
                sscanf(buf, "%d %d", &cntA, &cntB);

                if (cntA == 4) {
                    done = true;
                    break;
                }
                else {
                    i--;
                    guessvec[guess[i] - '0'] = false;
                }
            }
            else {
                int d = 0;
                while ((guessvec[d] || usedvec[i][d]) && d <= 9) d++;
                if (d <= 9) {
                    guess[i] = '0' + d;
                    guessvec[d] = true;
                    usedvec[i][d] = true;
                    i++;
                }
                else {
                    for (int d = 0; d <= 9; d++)
                        usedvec[i][d] = false;
                    guessvec[guess[i-1] - '0'] = false;
                    i--;
                }
            }
        }
    }
    return 0;
}