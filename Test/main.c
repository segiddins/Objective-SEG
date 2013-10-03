//
//  main.c
//  Test
//
//  Created by Samuel E. Giddins on 10/2/13.
//
//

#include <stdio.h>
#include "runtime.h"

void test()
{
    obj obj = class_new(get_class(object), "new");
    seg_generic_msg_send(obj, "cool");
}

int main(int argc, const char * argv[])
{
    test();
    return 0;
}

