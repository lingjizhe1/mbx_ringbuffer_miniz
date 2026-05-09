#pragma once
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OFFSETOF(type, member)  ((size_t)&((type *)0)->member)
#define WHERE_IS_STRUCT(ptr, type, member) \
    ((type *)((char *)(ptr) - OFFSETOF(type, member)))

typedef void (*notifier_handler_t)(int8_t event, void *data);
typedef void (*do_sth)(void *data);

typedef struct list_node {
    struct list_node *prev;
    struct list_node *next;
} list_node_t;

typedef struct notifier_node {
    uint8_t* name;
    notifier_handler_t callback;
    list_node_t node;
} notifier_node_t;

typedef struct thread_stat {
    uint8_t* name;
    uint8_t stat;
} thread_stat_t;

typedef struct thread {
    uint8_t current_stat;
    uint8_t receive_event;
    uint8_t next_stat;
    do_sth callback;
} thread_t;

typedef struct gluedata {
    thread_t* list;
    uint8_t count;
    thread_stat_t status;
    void* param;
} glue_data_t;

static inline void init_notifier_head (notifier_node_t* head)
{
    list_node_t* head_node = &(head -> node);

    head_node -> prev = head_node;
    head_node -> next = head_node;
}

static inline void add_notifier_node (notifier_node_t* head, notifier_node_t* member)
{
    list_node_t* head_node = &(head -> node);
    list_node_t* member_node = &(member ->node);
    list_node_t* temp = head_node -> next;
    
    head_node -> next = member_node;
    member_node -> next = temp;
    member_node -> prev = head_node;
    temp -> prev = member_node;
}

static inline void notify_all_members (notifier_node_t* head, int8_t event, void* data)
{
    list_node_t* head_node = &(head -> node);

    for (list_node_t* p = head_node -> next; p != head_node; p = p -> next){
        notifier_node_t* p_t = WHERE_IS_STRUCT(p, notifier_node_t, node);
        p_t -> callback(event, data);
    }
} 

static inline void thread_stat_machine (const thread_t* list, uint8_t count, thread_stat_t* status, const int8_t event, void* param)
{
    for (uint8_t i = 0; i < count; i++){
        if ((list[i].current_stat == status -> stat) && (list[i].receive_event == event))
        {
            list[i].callback(param);
            status -> stat = list[i].next_stat;
            break;
        }
    }
}
static inline void notifier_statmachine_glue (int8_t event, glue_data_t* data)
{
    thread_stat_machine(data -> list, data -> count, &(data -> status), event, data -> param);
}


#ifdef __cplusplus
}
#endif
