#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#define LOG(format, ...) fprintf(stderr, "\e[34m[CLIENT]\e[0m " format "\n", ##__VA_ARGS__)

char candidates[10000][5];
size_t list_bk[10000], list_fd[10000];
bool exists[10000];

size_t gen_candidates() {
    bool usedvec[4][10] = {0};
    bool guessvec[10] = {0};
    char cand[5];
    size_t len = 1;

    int i = 0;
    while (i >= 0 && i <= 4) {
        // LOG("i=%d", i);
        if (i == 4) {
            cand[4] = '\0';
            memcpy(candidates[len], cand, 5);
            // LOG("candidates[%d] = %s", len, candidates[len]);

            i--;
            guessvec[cand[i] - '0'] = false;
            len++;
        }
        else {
            int d = 0;
            while ((guessvec[d] || usedvec[i][d]) && d <= 9) d++;
            if (d <= 9) {
                cand[i] = '0' + d;
                guessvec[d] = true;
                usedvec[i][d] = true;
                i++;
            }
            else {
                for (int d = 0; d <= 9; d++)
                    usedvec[i][d] = false;
                guessvec[cand[i-1] - '0'] = false;
                i--;
            }
        }
    }
    for (size_t i = 0; i < len; i++) {
        list_fd[i] = i+1;
        list_bk[i] = i-1;
        exists[i] = true;
    }
    list_fd[len] = 0;
    list_bk[0] = len-1;
    return len;
}

void countAB(char ans[], char guess[], int* A, int* B) {
    bool vec[10] = {0};
    for (size_t i = 0; i < 4; i++) {
        vec[ans[i] - '0'] = true;
    }
    for (size_t i = 0; i < 4; i++) {
        if (guess[i] == ans[i])
            (*A)++;
        else if (vec[guess[i] - '0'])
            (*B)++;
    }
}

int main() {
    char buf[5];

    LOG("client start");
    size_t cand_num = gen_candidates();
    for (int round = 1; ; round++) {
        LOG("round %d", round);

        // we want that the distribution is uniform
        // so after filtering, there is as few left as possible
        int opt_idx = 0, opt_val = 2147483647;
        for (size_t i = 0; i < cand_num; i++) {
            int cnt[5][5] = {0};
            for (size_t j = list_fd[0]; j != 0; j = list_fd[j]) {
                int A = 0, B = 0;
                countAB(candidates[j], candidates[i], &A, &B);
                cnt[A][B]++;
            }
            int val = 0;
            for (size_t j = 0; j <= 4; j++) {
                for (size_t k = 0; k <= 4; k++) {
                    val += cnt[j][k] * cnt[j][k];
                }
            }
            if (val < opt_val || (val == opt_val && exists[i])) {
                opt_idx = i;
                opt_val = val;
            }
        }

        LOG("opt_idx=%d candadate=%s", opt_idx, candidates[opt_idx]);

        write(STDOUT_FILENO, candidates[opt_idx], 4);

        int cntA = 0, cntB = 0;
        read(STDIN_FILENO, buf, 4);
        sscanf(buf, "%d %d", &cntA, &cntB);
        if (cntA == 4) {
            break;
        }

        for (size_t i = list_fd[0]; i != 0; i = list_fd[i]) {
            int A = 0, B = 0;
            countAB(candidates[i], candidates[opt_idx], &A, &B);
            if (A != cntA || B != cntB) {
                list_fd[list_bk[i]] = list_fd[i];
                list_bk[list_fd[i]] = list_bk[i];
                exists[i] = false;
            }
        }
    }
    return 0;
}