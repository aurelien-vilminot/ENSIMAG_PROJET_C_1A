#ifndef PROJETC_IG_APPLICATION_H
#define PROJETC_IG_APPLICATION_H

// All classes of widget
static ei_widgetclass_t *frame_class;
static ei_widgetclass_t *top_level_class;
static ei_widgetclass_t *button_class;

// TODO : rename globale var with 'g' at begin
// Root elements
ei_surface_t root_windows;
ei_widget_t *root_frame;

// Offscreen
ei_surface_t offscreen;

/**
 * @brief       Return a linked list which represent all widget classes
 *
 * @return      A pointeur on the linked list
 */
extern ei_widgetclass_t* get_linked_list_classes();

#endif //PROJETC_IG_APPLICATION_H
