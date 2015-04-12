#include <stdint.h>
#include "sprite_priorities.h"
#include "rom_info.h"

struct node {
    uint16_t x_pos;
    struct node *prev; // Next node with higher priority
    struct node *next; // Next node with lower priority       
};


/* Sprites stored in order of priority,
 * use the array indexes as a bucket to directly access the
 * x position as well as the next lower and higher priority sprite */
static Node prio_sprites[MAX_SPRITES]; 
static Node sentinal_deref;
static Node *sentinal = &sentinal_deref; 
static Node *head_ptr; //current head of queue

void init_sprite_prio_list() {

    Node *prev = sentinal;
    for (int i = 0; i < MAX_SPRITES; i++) {
        Node *node = prio_sprites + i;
        node->x_pos = 0xFFFF; //Lowest prio
        node->next = (i >= MAX_SPRITES - 1) ? sentinal : prio_sprites + i + 1; 
        node->prev = prev;
        prev = &prio_sprites[i];
    }

    sentinal->next = prio_sprites;
    sentinal->prev = prio_sprites + MAX_SPRITES - 1;

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


void update_sprite_prios(int sprite_no, uint8_t x_pos) {
    
    Node *current_node = prio_sprites + sprite_no;
    // Order by x position and if equal then by sprite number
    uint16_t val = (x_pos << 8) | sprite_no;
    current_node->x_pos = val;
    Node *swap_node = current_node;

   
    // Move up priorities
    if (swap_node->prev != sentinal && val < swap_node->prev->x_pos) {
        while (swap_node->prev != sentinal && val < swap_node->prev->x_pos) {
            swap_node = swap_node->prev;
        }
        move_after(current_node, swap_node->prev);
     }
    
    // Move down priorites
    else if (swap_node->next != sentinal && val > swap_node->next->x_pos) {
        while (swap_node->next != sentinal && val > swap_node->next->x_pos) {
            swap_node = swap_node->next;
        }
        move_after(current_node, swap_node);
    }

    // Set new head if replaced
    if (current_node != swap_node && swap_node == head_ptr) {
        head_ptr = current_node;
    }
}


Sprite_Iterator create_sprite_iterator() {
	Sprite_Iterator si;
	si.next = head_ptr;
    return si;
 }


int sprite_iterator_next(Sprite_Iterator *si) {
    
    if (si->next != sentinal) {
        int result = si->next - prio_sprites;
        si->next = si->next->next;
        return result;
    } else {
        return -1;
    }
}
