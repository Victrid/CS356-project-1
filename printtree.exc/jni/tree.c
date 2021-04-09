/* This header is identical to the file in ptree.mdl */
#include "ptree.h"

#include <stdio.h>
#include <stdlib.h>

struct node {
    struct node* parent;
    struct node* sibling;
    struct node* son;
    prinfo_t* info;
};

typedef struct node node_t;

node_t* create_node(prinfo_t* info) {
    node_t* node = (node_t*)malloc(sizeof(node_t));
    node->parent  = NULL;
    node->sibling = NULL;
    node->son     = NULL;
    node->info    = info;
    return node;
}

void del(node_t* node) {
    if (node->son != NULL)
        del(node->son);
    if (node->sibling != NULL)
        del(node->sibling);
    free((void*)node);
    return;
}

void add_son(node_t* node, node_t* son) {
    node_t** sons = &(node->son);
    while (*sons != NULL)
        sons = &((*sons)->sibling);
    *sons       = son;
    son->parent = node;
    return;
}

struct node* regen(prinfo_t* infoptr, int count) {
    /* init with init_task */
    node_t *ancestor, *root,* newnode;
    ancestor = create_node(infoptr);
    root     = ancestor;
    infoptr++;
    count--;
    while (count > 0) {
        newnode = create_node(infoptr);
        while (ancestor->info->pid != newnode->info->parent_pid) {
            ancestor = ancestor->parent;
        }
        add_son(ancestor, newnode);
        ancestor = newnode;
        infoptr++;
        count--;
    }
    return root;
}

void print_node(node_t* root, int indent) {
    prinfo_t* infoptr;
    int i;
    node_t* sons = root->son;
    infoptr      = root->info;
    for (i = 0; i < indent; i++) {
        printf("\t");
    }
    printf("%s,%d,%ld,%d,%d,%d,%d\n",infoptr->comm, infoptr->pid, 
    infoptr->state,infoptr->parent_pid, infoptr->first_child_pid,
           infoptr->next_sibling_pid, infoptr->uid);
    while (sons != NULL) {
        print_node(sons, indent + 1);
        sons = sons->sibling;
    }
    return;
}

int main() {
    int total, real;
    real               = 500;
    prinfo_t* info_buf = malloc(real * sizeof(prinfo_t));
    total              = syscall(356, info_buf, &real);
    node_t* root = regen(info_buf, real);
    print_node(root, 0);
    return 0;
}
