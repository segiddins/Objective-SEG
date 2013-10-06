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
#include <stdarg.h>
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

void linked_list_each(linked_list *ll, int(^enumerator)(int index, void *object))
{
    assert(ll);
    assert(enumerator);
    linked_list_node *node = ll->first_node;
    int index = 0;
    int stop = 1;
    while (node && stop) {
        stop = enumerator(index++, node->car);
        node = node->cdr;
    }
}

method *method_create(char *name, void *imp)
{
    assert(imp);
    method *mthd = malloc(sizeof(method));
    mthd->name = name;
    mthd->imp = imp;
    return mthd;
}

void* seg_get_imp(obj obj, msg_t msg)
{
    assert(obj);
    assert(msg);
    class class = obj->class;
    __block method *mthd = NULL;
    while (!mthd && class) {
        linked_list *methods = class->methods;
        linked_list_each(methods, ^int(int index, void *object) {
            method *method = object;
            int retVal = strcmp(method->name, msg);
            if (!retVal) mthd = method;
            return retVal;
        });
        class = class->superclass;
    }
    if (!mthd) return seg_get_imp(obj, "no_such_method");
    return mthd->imp;
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
    __block class found_class = NULL;
    linked_list_each(class_list, ^int(int index, void *object) {
        class object_class = object;
        int retVal = strcmp(class_name, object_class->name);
        if (!retVal) found_class = object_class;
        return retVal;
    });
    return found_class;
}

void class_add_instance_method(class class, method *method)
{
    assert(class);
    assert(method);
    linked_list_add(class->methods, method);
}

void _seg_declare_instance_method(class class, char* message_name, void *IMP)
{
    class_add_instance_method(class, method_create(message_name, IMP));
}

__attribute((constructor (100)))
void seg_runtime_load()
{
    class_list = linked_list_create();
}

void seg_class_init(char* cls_name, int class_instance_size, int instance_size, class superclass)
{
    class class_ = calloc(1, class_instance_size);
    class_->name = cls_name;
    class_->instance_size = instance_size;
    class_->methods = linked_list_create();
    class_->class_methods = linked_list_create();
    class_->superclass = superclass;
    linked_list_add(class_list, class_);
}

seg_def_class(object, NULL, {
    class_t *class;
    int retain_count;
});

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

void object_no_such_method(obj self, msg_t msg)
{
    printf("%s does not respond to the message '%s'\n", send_msg(self, "desc"), msg);
//    abort();
}

seg_declare_instance_method(object, class);
seg_declare_instance_method(object, retain);
seg_declare_instance_method(object, release);
seg_declare_instance_method(object, free);
seg_declare_instance_method(object, desc);
seg_declare_instance_method(object, no_such_method);
