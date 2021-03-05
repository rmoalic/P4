#ifndef include_list
#define include_list 1

struct list_element {
    void* element;
    struct list_element* next;
};
typedef struct list_element* list;

void list_init(list* l);
bool list_is_empty(list l);
void list_prepend(list *l, void* element, size_t e_size);
void list_append(list *l, void* element, size_t e_size);
void list_remove(list *l, void* element_to_delete, int (*element_cmp)(void*, void*));

#endif
