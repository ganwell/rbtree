// SPDX-FileCopyrightText: 2019 Jean-Louis Fuchs <ganwell@fangorn.ch>
//
// SPDX-License-Identifier: AGPL-3.0-or-later

#include "testing.h"

#include <stdlib.h>

int
test_insert_static(void)
{
    node_t mnodes[5];
    node_t* tree;
    my_tree_init(&tree);
    node_t* node;
    for(int i = 0; i < 5; i++) {
        node = &mnodes[i];
        rb_value_m(node) = i;
        my_node_init(node);
        TA((
            rb_parent_m(node) == my_nil_ptr &&
            rb_left_m(node) == my_nil_ptr &&
            rb_right_m(node) == my_nil_ptr &&
            rb_color_m(node) == RB_BLACK
        ), "Node not properly initialized");
    }
    node = &mnodes[0];
    my_insert(&tree, node);
    TA(rb_parent_m(node) == my_nil_ptr, "Node not root after first insert");
    TA(rb_is_black_m(rb_color_m(node)), "Node not black after first insert");

    node = &mnodes[1];
    rb_value_m(node) = 0;
    my_insert(&tree, node);
    TA((
        rb_parent_m(node) == my_nil_ptr &&
        rb_left_m(node) == my_nil_ptr &&
        rb_right_m(node) == my_nil_ptr &&
        node != tree
    ), "Node should not have been added");
    TA(tree == &mnodes[0], "Equal node should not have been inserted");

    rb_value_m(node) = 1;
    for(int i = 0; i < 2; i++) {
        my_node_init(tree);
        my_node_init(node);
        my_tree_init(&tree);
        my_insert(&tree, &mnodes[0]);
        my_insert(&tree, node);
        TA(
            rb_is_red_m(rb_color_m(node)),
            "The first insert node should be red"
        );
        TA(
            rb_right_m(tree) == node,
            "The node should have been inserted on the right"
        );
    }

    my_node_init(tree);
    my_node_init(node);
    my_tree_init(&tree);
    my_insert(&tree, &mnodes[0]);
    rb_value_m(node) = -1;
    my_insert(&tree, node);
    TA(
        rb_is_red_m(rb_color_m(node)),
        "The first insert node should be red"
    );
    TA(
        rb_left_m(tree) == node,
        "The node should have been inserted on the left"
    );

    my_node_init(tree);
    my_node_init(node);
    rb_value_m(node) = 1;
    my_tree_init(&tree);
    my_insert(&tree, &mnodes[1]);
    my_insert(&tree, &mnodes[0]);
    my_insert(&tree, &mnodes[2]);
    TA(
        rb_left_m(tree) == &mnodes[0],
        "The smaller node should be in the left"
    );
    TA(
        rb_right_m(tree) == &mnodes[2],
        "The bigger node should be in the left"
    );
    int sum = 0;
    rb_iter_decl_cx_m(my, iter, elem);

    my_iter_init(tree, &iter, &elem);
    while(elem != NULL) {
        sum += rb_value_m(elem);
        my_iter_next(iter, &elem);
    }
    TA(sum == 3, "Sum should be 3");
    sum = 0;
    for(
            my_iter_init(tree, &iter, &elem);
            elem != NULL;
            my_iter_next(iter, &elem)
    ) {
        sum += rb_value_m(elem);
    }
    TA(sum == 3, "Sum should be 3");
    sum = 0;
    rb_for_m(my, tree, iter, elem) {
        sum += rb_value_m(elem);
    }
    TA(sum == 3, "Sum should be 3");
    return 0;
}

//               .---.   rotate_right   .---.
//               | y |     ------->     | x |
//               .---.                  .---.
//              /     |                /     |
//         .---'     .-'-.        .---'      .'--.
//         | x |     | C |        | A |      | y |
//         .---.     '---'        '---'      .---.
//        /     |                           /     |
//     .-'-.    .'--.                    .-'-.    .'--.
//     | A |    | B |      <------       | B |    | C |
//     '---'    '---'    rotate_left     '---'    '---'

#define Ny 0
#define Nx 1
#define NA 2
#define NB 3
#define NC 4
#define Nr 5
#define node_m(x) (&n[x])

int
assert_right(node_t n[5]) {
    TA(
        rb_left_m(node_m(Nx))   == node_m(NA),
        "Not rotated right"
    );
    TA(
        rb_right_m(node_m(Nx))  == node_m(Ny),
        "Not rotated right"
    );
    TA(
        rb_left_m(node_m(Ny))   == node_m(NB),
        "Not rotated right"
    );
    TA(
        rb_right_m(node_m(Ny))  == node_m(NC),
        "Not rotated right"
    );
    TA(
        rb_parent_m(node_m(NA)) == node_m(Nx),
        "Not rotated right"
    );
    TA(
        rb_parent_m(node_m(Ny)) == node_m(Nx),
        "Not rotated right"
    );
    TA(
        rb_parent_m(node_m(NB)) == node_m(Ny),
        "Not rotated right"
    );
    TA(
        rb_parent_m(node_m(NC)) == node_m(Ny),
        "Not rotated right"
    );
    return 0;
}

int
assert_left(node_t n[6]) {
    TA(
        rb_left_m(node_m(Ny))   == node_m(Nx),
        "Not rotated left"
    );
    TA(
        rb_right_m(node_m(Ny))  == node_m(NC),
        "Not rotated left"
    );
    TA(
        rb_left_m(node_m(Nx))   == node_m(NA),
        "Not rotated left"
    );
    TA(
        rb_right_m(node_m(Nx))   == node_m(NB),
        "Not rotated left"
    );
    TA(
        rb_parent_m(node_m(Nx)) == node_m(Ny),
        "Not rotated left"
    );
    TA(
        rb_parent_m(node_m(NC)) == node_m(Ny),
        "Not rotated left"
    );
    TA(
        rb_parent_m(node_m(NA)) == node_m(Nx),
        "Not rotated left"
    );
    TA(
        rb_parent_m(node_m(NB)) == node_m(Nx),
        "Not rotated left"
    );
    return 0;
}

int
test_rotate(void)
{
    node_t n[6];
    node_t* tree = node_m(Nx);
    node_t* node = node_m(Nx);
    for(int i = 0; i < 6; i++) {
        rb_value_m(node_m(i)) = i;
        my_node_init(node_m(i));
    }
    /* Setup nodes */
    rb_left_m(node_m(Nx))   = node_m(NA);
    rb_right_m(node_m(Nx))  = node_m(Ny);

    rb_left_m(node_m(Ny))   = node_m(NB);
    rb_right_m(node_m(Ny))  = node_m(NC);

    rb_parent_m(node_m(NA)) = node_m(Nx);
    rb_parent_m(node_m(NB)) = node_m(Ny);
    rb_parent_m(node_m(NC)) = node_m(Ny);
    rb_parent_m(node_m(Ny)) = node_m(Nx);
    T(assert_right(n));
    TA(
        rb_parent_m(node_m(Nx)) == my_nil_ptr,
        "Not rotated right"
    );
    _rb_rotate_left_tr_m(my, tree, node);
    TA(
        tree == node_m(Ny),
        "Y should be root"
    );
    T(assert_left(n));
    TA(
        rb_parent_m(node_m(Ny))  == my_nil_ptr,
        "Not rotated left"
    );
    tree = node_m(Ny);
    node = node_m(Ny);
    _rb_rotate_right_tr_m(my, tree, node);
    TA(
        tree == node_m(Nx),
        "X should be root"
    );
    T(assert_right(n));
    TA(
        rb_parent_m(node_m(Nx)) == my_nil_ptr,
        "Not rotated right"
    );
    /* Test with extra root */
    rb_parent_m(node_m(Nx)) = node_m(Nr);
    rb_left_m(node_m(Nr))   = node_m(Nx);
    T(assert_right(n));
    tree = node_m(Nr);
    node = node_m(Nx);
    TA(
        tree == node_m(Nr),
        "r should be root"
    );
    _rb_rotate_left_tr_m(my, tree, node);
    TA(
        tree == node_m(Nr),
        "r should be root"
    );
    T(assert_left(n));
    node = node_m(Ny);
    _rb_rotate_right_tr_m(my, tree, node);
    TA(
        tree == node_m(Nr),
        "r should be root"
    );
    T(assert_right(n));
    return 0;
}

int
test_insert(int len, int* nodes, int* sorted, int count, int sum, int do_sum)
{
    int ret = 0;
    node_t* mnodes = malloc(len * sizeof(node_t));
    do {
        node_t* tree;
        my_tree_init(&tree);
        node_t* node;
        for(int i = 0; i < len; i++) {
            node = &mnodes[i];
            my_node_init(node);
            rb_value_m(node) = nodes[i];
            BA((
                rb_parent_m(node) == my_nil_ptr &&
                rb_left_m(node) == my_nil_ptr &&
                rb_right_m(node) == my_nil_ptr &&
                rb_color_m(node) == RB_BLACK
            ), "Node not properly initialized");
            my_insert(&tree, node);
            //print_tree(0, tree, NULL);
#           ifndef RB_NO_CHECK
                my_check_tree(tree);
#           endif
        }
        int tsum = 0;
        int elems = 0;
        int fail = 0;
        rb_iter_decl_cx_m(my, iter, elem);
        //printf("elems: ");
        node = tree;
        rb_for_m(my, tree, iter, elem) {
            //printf("%d ", rb_value_m(elem));
            if(sorted[elems] != rb_value_m(elem)) {
                fail = 1;
            }
            tsum += rb_value_m(elem);
            elems += 1;
        };
        BA(fail == 0, "Not correctly sorted");
        BA(node == tree, "Iter changed tree");
        //printf("\nf%d == %d, %d == %d\n", count, elems, sum, tsum);
        BA(elems == count, "Iterator count failed");
        if(do_sum) {
            BA(tsum == sum, "Iterator sum failed");
        }
        tsum = 0;
        elems = 0;
        recursive_sum(&tsum, &elems, tree);
        BA(node == tree, "Sum changed tree");
        //printf("\na%d == %d, %d == %d\n", count, elems, sum, tsum);
        BA(elems == count, "Iterator count failed");
        if(do_sum) {
            BA(tsum == sum, "Iterator sum failed");
        }
    } while(0);
    free(mnodes);
    return ret;
}
