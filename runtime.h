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

void* seg_generic_msg_send(obj obj, msg_t msg, ...);

obj class_new(class class, msg_t msg);

class get_class(char *class_name);

void seg_class_init(char* cls_name, int class_instance_size, int instance_size, class superclass);

void _seg_declare_instance_method(class class, char* message_name, void *IMP);

#define seg_extern_def_class(class_name) extern char* class_name

#define seg_def_class(class_name, superclass_name, ivars) char* class_name = #class_name;\
typedef struct ivars class_name ##_;\
\
__attribute((constructor (101))) void class_name ##_load() {seg_class_init(class_name, sizeof(class_t), sizeof(class_name ##_), get_class(superclass_name));}

#define seg_declare_instance_method(class_name, message_name)\
__attribute((constructor (102))) void class_name ## _add_function_ ## message_name () {_seg_declare_instance_method(get_class(class_name), #message_name, class_name ## _ ## message_name);}

seg_extern_def_class(object);

#define send_msg(obj, msg, ...) seg_generic_msg_send(obj, msg, ##__VA_ARGS__)
#define super_send_msg(obj, msg) seg_generic_msg_send_class(obj, msg, ((class)send_msg(obj, "class"))->superclass)

#define new(class_name) class_new(get_class(#class_name), "new")

#endif
