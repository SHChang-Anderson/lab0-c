#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "mcts.h"
#include "util.h"

struct node {
    int move;
    char player;
    int n_visits;
    unsigned long score;
    struct node *parent;
    struct node *children[N_GRIDS];
};

unsigned long fixed_mul(unsigned long a, unsigned long b)
{
    unsigned long long tmp = (unsigned long long) a * b;
    return (unsigned long) ((tmp + (1U << (FIXED_SCALE_BITS - 1))) >>
                            FIXED_SCALE_BITS);
}

unsigned long fixed_div(unsigned long a, unsigned long b)
{
    unsigned long long tmp = ((unsigned long long) a << FIXED_SCALE_BITS) / b;
    return (unsigned long) tmp;
}


unsigned long sqrt_fix(unsigned long num)
{
    unsigned long x = num;
    unsigned long y = (x + 1);
    y >>= 1;

    while (y < x) {
        x = y;
        y = (x + fixed_div(num, x));
        y >>= 1;
    }

    return x;
}

static struct node *new_node(int move, char player, struct node *parent)
{
    struct node *node = malloc(sizeof(struct node));
    node->move = move;
    node->player = player;
    node->n_visits = 0;
    node->score = 0;
    node->parent = parent;
    memset(node->children, 0, sizeof(node->children));
    return node;
}

static void free_node(struct node *node)
{
    for (int i = 0; i < N_GRIDS; i++)
        if (node->children[i])
            free_node(node->children[i]);
    free(node);
}

static inline unsigned long uct_score(int n_total,
                                      int n_visits,
                                      unsigned long score)
{
    if (n_visits == 0)
        return ~0UL;

    unsigned long n_visitsn = (unsigned long) n_visits;

    n_visitsn <<= FIXED_SCALE_BITS;

    unsigned long n_totaln = (unsigned long) n_total;
    n_totaln <<= FIXED_SCALE_BITS;
    n_totaln = log(n_totaln);
    unsigned long tmp =
        fixed_mul(256, sqrt_fix(fixed_div(n_totaln, n_visitsn)));
    return fixed_div(score, n_visitsn) + tmp;
}

static struct node *select_move(struct node *node)
{
    struct node *best_node = NULL;
    unsigned long best_score = 0UL;
    for (int i = 0; i < N_GRIDS; i++) {
        if (!node->children[i])
            continue;
        unsigned long score =
            uct_score(node->n_visits, node->children[i]->n_visits,
                      node->children[i]->score);
        if (score > best_score) {
            best_score = score;
            best_node = node->children[i];
        }
    }
    if (!best_node) {
        while (1) {
            best_node = node->children[rand() % N_GRIDS];
            if (best_node)
                break;
        }
    }
    return best_node;
}

static unsigned long simulate(char *table, char player)
{
    char current_player = player;
    char temp_table[N_GRIDS];
    memcpy(temp_table, table, N_GRIDS);
    while (1) {
        char win;
        int *moves = available_moves(temp_table);
        if (moves[0] == -1) {
            free(moves);
            break;
        }
        int n_moves = 0;
        while (n_moves < N_GRIDS && moves[n_moves] != -1)
            ++n_moves;
        int move = moves[rand() % n_moves];
        free(moves);
        temp_table[move] = current_player;
        if ((win = check_win(temp_table)) != ' ')
            return calculate_win_value(win, player);
        current_player ^= 'O' ^ 'X';
    }
    return ((unsigned long) 1UL << (FIXED_SCALE_BITS - 1));
}

static void backpropagate(struct node *node, unsigned long score)
{
    while (node) {
        node->n_visits++;
        node->score += score;
        node = node->parent;
        score = 1 - score;
    }
}

static void expand(struct node *node, char *table)
{
    int *moves = available_moves(table);
    int n_moves = 0;
    while (n_moves < N_GRIDS && moves[n_moves] != -1)
        ++n_moves;
    for (int i = 0; i < n_moves; i++) {
        node->children[i] = new_node(moves[i], node->player ^ 'O' ^ 'X', node);
    }
    free(moves);
}

int mcts(char *table, char player)
{
    char win;
    struct node *root = new_node(-1, player, NULL);
    for (int i = 0; i < ITERATIONS; i++) {
        struct node *node = root;
        char temp_table[N_GRIDS];
        memcpy(temp_table, table, N_GRIDS);
        while (1) {
            if ((win = check_win(temp_table)) != ' ') {
                unsigned long score =
                    calculate_win_value(win, node->player ^ 'O' ^ 'X');
                backpropagate(node, score);
                break;
            }
            if (node->n_visits == 0) {
                unsigned long score = simulate(temp_table, node->player);
                backpropagate(node, score);
                break;
            }
            if (node->children[0] == NULL)
                expand(node, temp_table);
            node = select_move(node);
            assert(node);
            temp_table[node->move] = node->player ^ 'O' ^ 'X';
        }
    }
    struct node *best_node = NULL;
    int most_visits = -1;
    for (int i = 0; i < N_GRIDS; i++) {
        if (root->children[i] && root->children[i]->n_visits > most_visits) {
            most_visits = root->children[i]->n_visits;
            best_node = root->children[i];
        }
    }
    int best_move = best_node->move;
    free_node(root);
    return best_move;
}