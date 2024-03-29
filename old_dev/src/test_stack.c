// SPDX-FileCopyrightText: 2019 Jean-Louis Fuchs <ganwell@fangorn.ch>
//
// SPDX-License-Identifier: AGPL-3.0-or-later

#include "testing.h"

item_t* stack = NULL;

void
test_stack_init(void)
{
    stack = NULL;
}

int
test_push(item_t* item)
{
    qs_push(&stack, item);
    return 0;
}

int
test_pop(int item)
{
    item_t *tmp = NULL;
    qs_pop(&stack, &tmp);
    TA(item == rb_value_m(tmp), "Wrong item popped");
    return 0;
}

int
test_stack_null(void)
{
    return stack != NULL;
}

int
test_stack_iter(int* values)
{
    (void)(values);
    qs_stack_iter_decl_cx_m(qq, iter, elem);
    int i = 0;
    rb_for_m(qs, stack, iter, elem) {
        if(rb_value_m(elem) != values[i]) {
            return 1;
        }
        i += 1;
    }
    return 0;
}

int
test_stack_top(void)
{
    item_t* item;
    qs_top(stack, &item);
    return item->value;
}
