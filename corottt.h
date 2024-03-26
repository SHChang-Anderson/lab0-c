
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "agents/mcts.h"
#include "agents/negamax.h"
#include "list.h"


struct task {
    jmp_buf env;
    struct list_head list;
    char task_name[10];
    char *table;
    char turn;
};

struct arg {
    char *table;
    char turn;
    char *task_name;
};


void schedule(void);
void task0(void *arg);
void task1(void *arg);
void ttt_coro(int cvc);
