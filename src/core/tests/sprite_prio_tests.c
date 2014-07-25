#include "../sprite_priorities.c"
#include "minunit/minunit.h"
#include <stdio.h>


/* Initialise Sprite Priority list */
void setup() {
   init_sprite_prio_list(); 
}

void teardown() {
    //Nothing
}


// Check that a list is created of size MAX_SPRITES
// and that when dequeing we can obtain every unique
// sprite number
MU_TEST(correct_initialisation) {    
    
    int sprite_set[MAX_SPRITES];

    for (int i = 0; i < MAX_SPRITES; i++) {
        sprite_set[i] = 0;
    }
     
    Sprite_Iterator si = create_sprite_iterator();

    for (int i = 0; i < MAX_SPRITES; i++) {
        int n = sprite_iterator_next(&si);
        mu_check(n >= 0 && n < MAX_SPRITES);
        mu_check(!sprite_set[n]);
        sprite_set[n] = 1;
    }
}


// Check that updating a priority works
MU_TEST(updating) {
    update_sprite_prios(29, 3);
    Sprite_Iterator si = create_sprite_iterator();
    mu_assert_uint_eq(sprite_iterator_next(&si), 29);
}

/*Check order of priorities are correct
 * when all priorities are different values */
MU_TEST(priority_order_reverse) {
    // Insert highest priority first
    for (int i = 0; i < MAX_SPRITES; i++) {
        update_sprite_prios(i, i); 
    }
    // Check we get back lowest priority first
    Sprite_Iterator si = create_sprite_iterator();
    for (int i = 0; i < MAX_SPRITES; i++) {
        int n = sprite_iterator_next(&si);
        mu_assert_int_eq(n, MAX_SPRITES - 1 - i);
    }
}

MU_TEST(priority_order) {

    // Insert highest priority first
    for (int i = 0; i < MAX_SPRITES; i++) {
        update_sprite_prios(i, MAX_SPRITES - i); 
    }
    // Check we get back lowest priority first
    Sprite_Iterator si = create_sprite_iterator();
    for (int i = 0; i < MAX_SPRITES; i++) {
        int n = sprite_iterator_next(&si);
        mu_assert_int_eq(n, i);
    }
}



/* Check order of priority
 * when x positions of 2 sprites are equal */
MU_TEST(priority_equals) {
    
    update_sprite_prios(10, 27);
    update_sprite_prios(38, 27);

    Sprite_Iterator si = create_sprite_iterator();
    int n1 = sprite_iterator_next(&si);
    int n2 = sprite_iterator_next(&si);
    for (int i = 0; i <MAX_SPRITES; i++) {
        int n3 = sprite_iterator_next(&si);
        printf("%d ",n3);
    }
    // Should get largest sprite no first
    // as lower priority
    mu_assert_int_eq(n2, 10);
    mu_assert_int_eq(n1, 38);


    // Check inserting the other way round
    update_sprite_prios(21, 240);
    update_sprite_prios(3, 240);

    si = create_sprite_iterator();
    n1 = sprite_iterator_next(&si);
    n2 = sprite_iterator_next(&si);

    mu_assert_int_eq(n2, 3);
    mu_assert_int_eq(n1, 21);
}


MU_TEST_SUITE(sprite_priorities) {

	MU_SUITE_CONFIGURE(&setup, &teardown)
    ;
    MU_RUN_TEST(correct_initialisation);
    MU_RUN_TEST(updating);
    MU_RUN_TEST(priority_order);
    MU_RUN_TEST(priority_order_reverse);
    MU_RUN_TEST(priority_equals);
}


int main() {
    MU_RUN_SUITE(sprite_priorities);
    MU_REPORT();
    return 0;
}



