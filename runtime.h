//
//  runtime.h
//  Objective-SEG
//
//  Created by Samuel E. Giddins on 10/2/13.
//
//

#ifndef Objective_SEG_runtime_h
#define Objective_SEG_runtime_h

typedef void* linked_list_t;

typedef struct class_t class_t;
struct class_t {
    char* name;
    class_t* superclass;
    unsigned int instance_size;
    linked_list_t methods;
    linked_list_t class_methods;
};

typedef struct {
    class_t *class;
    int retain_count;
} obj_t;

typedef obj_t* obj;

typedef class_t* class;
typedef char* msg_t;

void* seg_generic_msg_send(obj obj, msg_t msg);

obj class_new(class class, msg_t msg);

class get_class(char *class_name);

#define seg_extern_def_class(class_name) extern char* class_name

#define seg_def_class(class_name, superclass_name, ivars) char* class_name = #class_name;\
typedef struct ivars class_name ##_;\
void class_name ##_init(char* cls_name, int class_instance_size, int instance_size, class superclass)\
{\
    class class_ = calloc(1, class_instance_size);\
    class_->name = cls_name;\
    class_->instance_size = instance_size;\
    class_->methods = linked_list_create();\
    class_->class_methods = linked_list_create();\
    class_->superclass = superclass;\
    linked_list_add(class_list, class_);\
}\
__attribute((constructor)) void class_name ##_load() {class_name ##_init(class_name, sizeof(class_t), sizeof(class_name ##_), get_class(class_name));}

#define seg_declare_instance_method(class_name, message_name)\
__attribute((constructor)) void class_name ## _add_function_ ## message_name () {class_add_instance_method(get_class(class_name), method_create( #message_name, class_name ## _ ## message_name));}

seg_extern_def_class(object);

#endif
