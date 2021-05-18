#ifndef PROJETC_IG_EVENT_MANAGER_H
#define PROJETC_IG_EVENT_MANAGER_H


// Is used to know what is the widget currently used by the user
static ei_widget_t *g_active_widget;

// Next event
static ei_event_t *next_event;

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
