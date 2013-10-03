//
//  runtime.c
//  Objective-SEG
//
//  Created by Samuel E. Giddins on 10/2/13.
//
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "runtime.h"

typedef struct {
    void *car;
    void *cdr;
} linked_list_node;

typedef struct {
    linked_list_node *first_node;
} linked_list;

typedef struct {
    msg_t name;
    void *imp;
} method;

static linked_list *class_list;

linked_list_node *linked_list_node_create(void *obj)
{
    linked_list_node *lln = calloc(1, sizeof(linked_list_node));
    lln->car = obj;
    return lln;
}

void linked_list_add(linked_list *ll, void *obj)
{
    linked_list_node *node = ll->first_node;
    if (!node) {
        ll->first_node = linked_list_node_create(obj);
        return;
    }
    while (node && node->cdr)
        node = node->cdr;
    node->cdr = linked_list_node_create(obj);
}

linked_list *linked_list_create()
{
    linked_list *ll = calloc(1, sizeof(linked_list));
    return ll;
}

method *method_create(char *name, void *imp)
{
    assert(imp);
    method *mthd = malloc(sizeof(method));
    mthd->name = name;
    mthd->imp = imp;
    return mthd;
}

void *seg_generic_msg_send_class(obj obj, msg_t msg, class class)
{
    if (!obj || !class) return NULL;
    linked_list *methods = class->methods;
    linked_list_node *node = methods->first_node;
    method *mthd = NULL;
    while (node) {
        if (!strcmp(((method*)(node->car))->name, msg)) {
            mthd = node->car;
            break;
        }
        node = node->cdr;
    }
    if (mthd) {
        void *(*imp)(obj_t*, msg_t) = mthd->imp;
        return imp(obj, msg);
    } else if (class->superclass) {
        return seg_generic_msg_send_class(obj, msg, class->superclass);
    }
    printf("%s does not recognize the message '%s'\n", seg_generic_msg_send(obj, "desc"), msg);
    return NULL;
}

void* seg_generic_msg_send(obj obj, msg_t msg)
{
    if (!obj) return NULL;
    return seg_generic_msg_send_class(obj, msg, obj->class);
}

obj class_new(class class, msg_t msg)
{
    obj new = calloc(1, class->instance_size);
    new->class = class;
    new->retain_count = 1;
    return new;
}

class get_class(char *class_name)
{
    linked_list_node *node = class_list->first_node;
    class class = node ? node->car : NULL;
    while (class) {
        if (!strcmp(class_name, class->name))
            break;
        class = node->cdr;
    }
    return class;
}

void class_add_instance_method(class class, method *method)
{
    linked_list_add(class->methods, method);
}

__attribute((constructor))
void seg_runtime_load()
{
    class_list = linked_list_create();
}

seg_def_class(object, NULL, {});

class object_class(obj self, msg_t msg)
{
    return self->class;
}

obj object_retain(obj self, msg_t msg)
{
    self->retain_count = self->retain_count + 1;
    return self;
}

obj object_release(obj self, msg_t msg)
{
    int retcount = self->retain_count;
    if (!--retcount) seg_generic_msg_send(self, "free");
    return retcount ? self : NULL;
}

obj object_free(obj self, msg_t msg)
{
    free(self);
    return self;
}

char* object_desc(obj self, msg_t msg)
{
    char * class_name = ((class)seg_generic_msg_send(self, "class"))->name;
    char * desc = malloc(1024);
    snprintf(desc, 1024, "<%p: %s>", self, class_name);
    return desc;
}

seg_declare_instance_method(object, class);
seg_declare_instance_method(object, retain);
seg_declare_instance_method(object, release);
seg_declare_instance_method(object, free);
seg_declare_instance_method(object, desc);
