#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#include <stdio.h>
#include <string.h>
#include "node.h"
/*
Name: Jacob Gerega
Pledge: I pledge my honor that I have abided by the Stevens Honor System.
Date: 2/26/21
*/

typedef struct linked_list {
    node *head;
    node *tail;
    size_t num_nodes;
} linked_list;

linked_list* create_linked_list() {
    return (linked_list *)calloc(1, sizeof(linked_list));
}

/**
 * Inserts a non-NULL node into a non-NULL linked list.
 * The node is inserted so that the data in the linked list is in
 * non-decreasing order.
 * A node with a value already in the list is inserted AFTER the node(s)
 * already in the list.
 */
void insert_in_order(linked_list *list, node *n,
                     int (*cmp)(const void*, const void*)) {
    if(list->head == NULL){
        n->prev = NULL;
        n->next = NULL;
        list->head = n;
        list->tail = n;
        list->num_nodes++;
    }
    else if(list->head->next == NULL){
        if(cmp(list->head->data, n->data) > 0){
            n->next = list->head;
            n->prev = NULL;
            list->head->prev = n;
            list->tail = list->head;
            list->head = n;
        }
        else{
            n->prev = list->head;
            n->next = NULL;
            list->head->next = n;
            list->tail = n;
        }
        list->num_nodes++;
    }
    else{
        node* curr = list->head;
        node* currn = list->head->next;
        while((currn != NULL) && (cmp(n->data, currn->data) > 0)){
            curr = curr->next;
            currn = currn->next;
        }
        if(currn == NULL){
            n->prev = curr;
            n->next = NULL;
            curr->next = n;
            list->tail = n;
        }
        else if(currn != NULL){
            n->prev = curr;
            n->next = currn;
            curr->next = n;
            currn->prev = n;
        }
        list->num_nodes++;
    }
}

void print_list(linked_list *list, void (*print_function)(void*)) {
    putchar('[');
    node *cur = list->head;
    if (cur != NULL) {
        print_function(cur->data);
        cur = cur->next;
    }
    for ( ; cur != NULL; cur = cur->next) {
        printf(", ");
        print_function(cur->data);
    }
    printf("]\n{length: %lu, head->data: ", list->num_nodes);
    list->head != NULL ? print_function(list->head->data) :
                         (void)printf("NULL");
    printf(", tail->data: ");
    list->tail != NULL ? print_function(list->tail->data) :
                         (void)printf("NULL");
    printf("}\n\n");
}

/**
 * Frees a list starting from the tail.
 * This will check if your previous pointers have been set up correctly.
 */
void free_list(linked_list *list, void (*free_data)(void *)) {
    while (list->tail != NULL) {
        node *prev = list->tail->prev;
        free_node(list->tail, free_data);
        list->tail = prev;
    }
}

#endif
