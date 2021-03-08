#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "list.h"

void list_init(list* l) {
    *l = NULL;
}

bool list_is_empty(list l) {
    return l == NULL;
}

static struct list_element* init_element(void* element, size_t e_size) {
    struct list_element* new = malloc(sizeof(struct list_element));
    new->element = malloc(e_size);

    memcpy(new->element, element, e_size);
    new->next = NULL;
    
    return new;
}

void list_prepend(list *l, void* element, size_t e_size) {
    struct list_element* new = init_element(element, e_size);
  
    if (list_is_empty(*l)) {
        new->next = NULL;
        *l = new;
    } else {
        new->next = *l;
        *l = new;
    }
}

void list_append(list *l, void* element, size_t e_size) {
    struct list_element* new = init_element(element, e_size);
  
    if (list_is_empty(*l)) {
        new->next = NULL;
        *l = new;
    } else {
        struct list_element* curr = *l;
        while (curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = new;
    }
}

void list_remove(list *l, void* element_to_delete, int (*element_cmp)(void*, void*)) {
    assert(l != NULL);
    assert(element_cmp != NULL);

    struct list_element* curr = *l;
    struct list_element* pre = NULL;
    struct list_element* to_delete = NULL;
      
    while (curr != NULL) {
        if (element_cmp(curr->element, element_to_delete) == 0) {
            to_delete = curr;
            if (pre == NULL) {
                *l = curr->next;
                 curr = *l;
            } else {
                 pre->next = curr->next;
                 curr = pre->next;
            }

            free(to_delete->element);
            free(to_delete);
        } else {
            pre = curr;
            curr = curr->next;
        }
    }
}

