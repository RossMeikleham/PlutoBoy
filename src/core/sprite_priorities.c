/*
 * =====================================================================================
 *
 *       Filename:  sprite_lists.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  20/07/14 08:14:10
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */


#include <stdint.h>
#include "sprite_priorities.h"

struct node {
    uint16_t x_pos;
    struct node *prev; // Next node with lower priority
    struct node *next; // Next node with higher priority       
};


/* Sprites stored in order of inverse priority,
 * use the array indexes as a bucket to directly access the
 * x position as well as the next lower and higher priority sprite */
static Node inverse_prio_sprites[MAX_SPRITES]; 
static Node sentinal_deref;
static Node *sentinal = &sentinal_deref; 
static Node *head_ptr; //current head of queue


void init_sprite_prio_list() {

    Node *next = sentinal;
    for (int i = 0; i < MAX_SPRITES; i++) {
        Node *node = inverse_prio_sprites + i;
        node->x_pos = 0; //Lowest prio
        node->next = next;
        node->prev = 
            (i >= MAX_SPRITES - 1) ? sentinal : inverse_prio_sprites + i + 1;

        next = &inverse_prio_sprites[i];
    }

    sentinal->prev = inverse_prio_sprites;
    sentinal->next = inverse_prio_sprites + MAX_SPRITES - 1;

    head_ptr = sentinal->next;
}


static void move_after(Node *origin, Node *destination) {
    // Relink either sides of node to move
    origin->prev->next = origin->next;
    origin->next->prev = origin->prev;

    // Link the original node into new place
    origin->prev = destination;
    origin->next = destination->next;

    //Connect new links back to origin
    origin->next->prev = origin;
    origin->prev->next = origin;

}

static void move_before(Node *origin, Node *destination) {
    
    origin->prev->next = origin->next;
    origin->next->prev = origin->prev;
    
    origin->next = destination;
    origin->prev = destination->prev;
    origin->prev->next = origin;
    origin->next->prev = origin;    
}

void update_sprite_prios(int sprite_no, uint8_t x_pos) {

    Node *current_node = inverse_prio_sprites + sprite_no;
    current_node->x_pos = x_pos;    
    Node *swap_node = current_node;

   
    // Move up priorities
    if (swap_node->next != sentinal && x_pos < swap_node->next->x_pos) {
        while (swap_node->next != sentinal && x_pos < swap_node->next->x_pos) {
            swap_node = swap_node->next;
        }
        Node *temp_node = swap_node->next;
       // Equal X positions, lower sprite no has priority
       
        if (temp_node != sentinal && x_pos == temp_node->x_pos &&
            current_node < temp_node) {
            swap_node = temp_node;
        } 
        move_before(current_node, swap_node->next);
     }
    
    // Move down priorites
    else if (swap_node->prev != sentinal && x_pos > swap_node->prev->x_pos) {
        while (swap_node->prev != sentinal && x_pos > swap_node->prev->x_pos) {
            swap_node = swap_node->prev;
        }
        Node *temp_node = swap_node->prev;
        //Equal X Positions, lower sprite has priority
        if (temp_node != sentinal && x_pos == temp_node->x_pos &&
            current_node > temp_node) {
            swap_node = temp_node;
        }
        move_after(current_node, swap_node->prev);
    }

    // Set new head if replaced
    if (current_node != swap_node && swap_node == head_ptr) {
        head_ptr = current_node;
    }
}


Sprite_Iterator create_sprite_iterator() {
    Sprite_Iterator si = {.next = head_ptr};
    return si;
 }


int sprite_iterator_next(Sprite_Iterator *si) {

    if (si->next != sentinal) {
        int result = si->next - inverse_prio_sprites;
        si->next = si->next->next;
        return result;
    } else {
        return -1;
    }
}
