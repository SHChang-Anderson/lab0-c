#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    return head ? (INIT_LIST_HEAD(head), head) : NULL;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (l) {
        element_t *entry, *safe;
        list_for_each_entry_safe (entry, safe, l, list) {
            q_release_element(entry);
        }
        free(l);
    }
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head) {
        return false;
    }
    element_t *node = malloc(sizeof(element_t));
    if (node) {
        node->value = malloc(sizeof(char) * (strlen(s) + 1));
        if (node->value) {
            strncpy(node->value, s, (strlen(s) + 1));
            list_add(&node->list, head);
            return true;
        } else {
            free(node);
            return false;
        }
    }
    return false;
}


/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head) {
        return false;
    }
    element_t *node = malloc(sizeof(element_t));
    if (node) {
        node->value = malloc(sizeof(char) * (strlen(s) + 1));
        if (node->value) {
            strncpy(node->value, s, (strlen(s) + 1));
            list_add_tail(&node->list, head);
            return true;
        } else {
            free(node);
            return false;
        }
    }
    return false;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (head && head->next) {
        element_t *removed_element = list_first_entry(head, element_t, list);
        strncpy(sp, removed_element->value, bufsize);
        list_del(&removed_element->list);
        return removed_element;
    }
    return NULL;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (head && head->next) {
        element_t *removed_element = list_last_entry(head, element_t, list);
        strncpy(sp, removed_element->value, bufsize);
        list_del(&removed_element->list);
        return removed_element;
    }
    return NULL;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (head) {
        struct list_head *node;
        int counter = 0;
        list_for_each (node, head) {
            counter += 1;
        }
        return counter;
    }
    return 0;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (head) {
        struct list_head *first, *last;
        first = head->next;
        last = head->prev;
        while (first != head && last != head) {
            if (first == last->prev || first == last) {
                list_del(first);
                q_release_element(list_entry(first, element_t, list));
                return true;
            }
            first = first->next;
            last = last->prev;
        }
    }
    return false;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (head && head->next != head && head->next->next != head) {
        struct list_head *newhead = head->next->next;
        do {
            list_move_tail(head->next->next, head);
            list_move_tail(head->next, head);
        } while (head->next != newhead && head->next->next != newhead);
        if (head->next != newhead) {
            list_move_tail(head->next, head);
        }
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (head) {
        struct list_head *node, *safe;
        list_for_each_safe (node, safe, head) {
            list_move(node, head);
        }
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

int q_merge_two(struct list_head *first, struct list_head *second, bool descend)
{
    if (!first || !second)
        return 0;
    int size = 0;
    struct list_head temp_head;
    INIT_LIST_HEAD(&temp_head);
    while (!list_empty(first) && !list_empty(second)) {
        element_t *first_front = list_first_entry(first, element_t, list);
        element_t *second_front = list_first_entry(second, element_t, list);
        char *first_str = first_front->value, *second_str = second_front->value;
        element_t *minimum =
            strcmp(first_str, second_str) < 0 ? first_front : second_front;
        list_move_tail(&minimum->list, &temp_head);
        size++;
    }
    size += q_size(first);
    list_splice_tail_init(first, &temp_head);
    size += q_size(second);
    list_splice_tail_init(second, &temp_head);
    list_splice(&temp_head, first);
    return size;
}

void mergeSortList(struct list_head *head, bool descend)
{
    // merge sort
    if (list_is_singular(head)) {
        return;
    }

    struct list_head *fast = head->next->next;
    struct list_head *slow = head->next;
    struct list_head newhead;
    INIT_LIST_HEAD(&newhead);

    // split list
    while (fast != head && fast->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }

    list_cut_position(&newhead, head, slow);

    // sort each list
    mergeSortList(&newhead, descend);
    mergeSortList(head, descend);

    // merge sorted lists
    q_merge_two(head, &newhead, descend);
}


/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    mergeSortList(head, descend);
}



/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    if (!head || head->next == head)
        return 0;
    q_reverse(head);
    struct list_head *node, *safe, *cur;
    cur = head->next;
    list_for_each_safe (node, safe, head) {
        if (strcmp(list_entry(node, element_t, list)->value,
                   list_entry(cur, element_t, list)->value) > 0) {
            list_del_init(node);
            q_release_element(list_entry(node, element_t, list));
        } else
            cur = node;
    }
    q_reverse(head);
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || head->next == head)
        return 0;
    q_reverse(head);
    struct list_head *node, *safe, *cur;
    cur = head->next;
    list_for_each_safe (node, safe, head) {
        if (strcmp(list_entry(node, element_t, list)->value,
                   list_entry(cur, element_t, list)->value) < 0) {
            list_del_init(node);
            q_release_element(list_entry(node, element_t, list));
        } else
            cur = node;
    }
    q_reverse(head);
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
