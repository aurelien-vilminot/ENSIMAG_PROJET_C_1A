#ifndef PROJETC_IG_EVENT_MANAGER_H
#define PROJETC_IG_EVENT_MANAGER_H


// Is used to know what is the widget currently used by the user
ei_widget_t *g_active_widget;

// Next event
ei_event_t *next_event;

// To know if this is the end of events
static ei_bool_t not_the_end = EI_TRUE;


ei_bool_t mouse_callback(ei_event_t event);

/**
 * Handle functions
 */
// TODO: Make descriptions
ei_bool_t handle_button_function(struct ei_widget_t* widget,
                                 struct ei_event_t* event);

ei_bool_t handle_top_level_function(struct ei_widget_t* widget,
                                    struct ei_event_t* event);

ei_bool_t handle_frame_function(struct ei_widget_t* widget,
                                struct ei_event_t* event);



#endif //PROJETC_IG_EVENT_MANAGER_H
