/**
 *      ptree   -   print ptree.
 *
 *  This is the C program required to print ptree. It regenerates a nodetree
 *  structure from the syscall output first, and then printout the structure
 *  as required.
 *
 */

#include "ptree.h"

#include <errno.h> /* syscall error handling */
#include <stdio.h>
#include <stdlib.h> /* malloc */

struct node {
    struct node* parent;
    struct node* sibling;
    struct node* son;
    prinfo_t* info;
};

typedef struct node node_t;

/**
 * @brief Create a node with prinfo pointer. All connective pointers will be
 * set to NULL.
 * @param info the prinfo pointer
 * @return node pointer.
 */
node_t* create_node(prinfo_t* info) {
    node_t* node = (node_t*)malloc(sizeof(node_t));
    if (node == NULL) {
        printf("malloc failed: %s\n", strerror(errno));
        return NULL;
    }
    node->parent  = NULL;
    node->sibling = NULL;
    node->son     = NULL;
    node->info    = info;
    return node;
}

/**
 * @brief Delete a node tree recursively
 * @param node Pointer of root
 * @return (void)
 */
void del(node_t* node) {
    if (node->son != NULL)
        del(node->son);
    if (node->sibling != NULL)
        del(node->sibling);
    free((void*) node);
    return;
}

/**
 * @brief Add a child to a node
 * @param node the parent node pointer
 * @param son the child node pointer
 * @return (void)
 */
void add_son(node_t* node, node_t* son) {
    node_t** sons = &(node->son);
    while (*sons != NULL)
        sons = &((*sons)->sibling);
    *sons       = son;
    son->parent = node;
    return;
}

/**
 * @brief Regenerate the tree structure from DFS sequence.
 * @param infoptr the prinfo buffer pointer
 * @param count the prinfo buffer length
 * @return the regenerated tree root node.
 */
node_t* regen(prinfo_t* infoptr, int count) {
    /* init with init_task */
    node_t *parent, *root, *newnode;
    parent = create_node(infoptr);
    root     = parent;
    infoptr++;
    count--;
    while (count > 0) {
        newnode = create_node(infoptr);
        /* malloc error handling */
        if (newnode == NULL) 
            return NULL;
        /* float up to newnode's parent */
        while (parent->info->pid != newnode->info->parent_pid)
            parent = parent->parent;

        add_son(parent, newnode);
        parent = newnode;
        infoptr++;
        count--;
    }
    return root;
}

/**
 * @brief Print the node tree in tree structure recursively.
 * @param root the root tree node pointer
 * @param indent indent number of "\t"
 * @return (void)
 */
void print_node(node_t* root, int indent) {
    prinfo_t* infoptr;
    int i;
    node_t* sons = root->son;
    infoptr      = root->info;
    /* Print sufficient \t */
    for (i = 0; i < indent; i++) 
        printf("\t");
    
    /* Print the wanted prinfo */
    /* Printing uid in %ld, as it's long */
    printf("%s,%d,%ld,%d,%d,%d,%ld\n", infoptr->comm, infoptr->pid,
           infoptr->state, infoptr->parent_pid, infoptr->first_child_pid,
           infoptr->next_sibling_pid, infoptr->uid);
        
    /* traverse on children nodes */
    while (sons != NULL) {
        print_node(sons, indent + 1);
        sons = sons->sibling;
    }
    return;
}

int main() {
    int total, real;
    prinfo_t* info_buf;
    /* Buffer size. Adjust this to modify the buffer length. */
    real               = 32768;
    info_buf = malloc(real * sizeof(prinfo_t));

    if (info_buf == NULL) {
        printf("malloc failed while creating buffer: %s\n", strerror(errno));
        return -1;
    }

    /* Invoke syscall */
    total = syscall(356, info_buf, &real);

    /* exit if ptree failed */
    if (total <= 0) {
        printf("ptree failed: %s\n", strerror(errno));
        printf("Maybe ptree module not loaded?\n");
        return -1;
    }
    node_t* root = regen(info_buf, real);
    /* exit if internal malloc failed */
    if (root == NULL) {
        printf("malloc failed while generating tree structure: %s\n",
               strerror(errno));
        return -1;
    }
    print_node(root, 0);
    del(root);
    return 0;
}
