//
//  main.c
//  Test
//
//  Created by Samuel E. Giddins on 10/2/13.
//
//

#include <stdio.h>
#include "runtime.h"

seg_def_class(subclass, object, {
    obj_t super;
    int val;
});

void subclass_cool(subclass_ *self, msg_t msg)
{
    printf("cool!\nmy value is: %d\n", self->val);
}

void subclass_set_val(subclass_ *self, msg_t msg, int val)
{
    self->val = val;
}

int subclass_val(subclass_ *self, msg_t msg)
{
    return self->val;
}

seg_declare_instance_method(subclass, cool);
seg_declare_instance_method(subclass, set_val);
seg_declare_instance_method(subclass, val);

void test()
{
    obj object = new(subclass);
    send_msg(object, "set_val", 12);
    send_msg(object, "cool");
    send_msg(object, "despots");
    printf("%s's val is '%d'\n", send_msg(object, "desc"), (int)send_msg(object, "val"));
    obj null_obj = NULL;
    printf("sending a message to %p (NULL) returns %p\n", null_obj, send_msg(null_obj, "desc"));
}

int main(int argc, const char * argv[])
{
    test();
    return 0;
}
