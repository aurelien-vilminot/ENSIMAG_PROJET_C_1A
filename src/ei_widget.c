#include <ei_utils.h>

#include "ei_widget.h"
#include "widget_manager.h"

/**
 * @brief       All is in the title
 *
 * @param       surface             Used to know rgba position in the corresponding surface
 * @param       color_to_convert    Element which is convert into ei_color_t
 *
 * @return      A color corresponding to the 32 bits give as argument. This color must be free by user.
 */
ei_color_t *inverse_map_rgba(ei_surface_t surface, uint32_t color_to_convert){
        // Place of colors
        int red_place;
        int green_place;
        int blue_place;
        int alpha_place;
        hw_surface_get_channel_indices(surface, &red_place, &green_place, &blue_place, &alpha_place);

        // Pointer on the color_to_convert
        uint8_t *p = (uint8_t *) &color_to_convert;

        // Associate each value of the color with the corresponding bits
        ei_color_t *color_to_return = malloc(sizeof(ei_color_t));
        color_to_return->red = p[red_place];
        color_to_return->blue = p[blue_place];
        color_to_return->green = p[green_place];
        color_to_return->alpha = p[6 - (blue_place + red_place + green_place)];

        return color_to_return;
}

/**
 * @brief       Update child field of parent. It is a course of linked list.
 *
 * @param       widget      The widget child
 * @param       parent      The child's parent which the children field needs to be updated
 */
static void insert_child(ei_widget_t *widget, ei_widget_t *parent) {
        if (parent->children_head) {
                parent->children_tail->next_sibling = widget;
        } else {
                parent->children_head = widget;
        }
        parent->children_tail = widget;

}

/**
 * @brief       This function return the higher id used into tree which represents all the widgets.
 *              It useful because the last element inserted is not necessarily on the bottom-right corner of the tree.
 *
 * @return      An uint32_t which is the id.
 */
static uint32_t last_id(void) {
        // Init
        ei_widget_t *current_widget = g_root_frame;
        uint32_t count_id = 0;

        // Depth course of each widgets
        do {
                if (current_widget->children_head) {
                        current_widget = current_widget->children_head;
                        count_id++;
                } else {
                        while (current_widget != g_root_frame && current_widget->next_sibling == NULL) {
                                current_widget = current_widget->parent;
                        }

                        if (current_widget->next_sibling) {
                                current_widget = current_widget->next_sibling;
                                count_id++;
                        }
                }
        } while (current_widget != g_root_frame);
        return count_id;
}

/**
 * @brief       Return widget class which correspond to class name give in parameter
 *
 * @param       ll              The linked list which contained all classes (button, top-level and frame)
 * @param       class_name      The class name
 *
 * @return      The class corresponding of class_name if exists. If not, return NULL;
 */
ei_widgetclass_t *get_class(ei_widgetclass_t *ll, ei_widgetclass_name_t class_name) {
        while (ll != NULL) {
                char * class_name_node = ll->name;
                char * class_name_param = ei_widgetclass_stringname(class_name);
                if (strcmp(class_name_node, class_name_param) == 0) {
                        return ll;
                }
                ll = ll->next;
        }
        return ll;
}

/*
 * Allocation functions
 */

/**
 * @brief       Allocate memory used by a button widget
 * @return      The corresponding widget
 */
ei_widget_t* button_alloc_func() {
        ei_button_t * button = calloc(1, sizeof(ei_button_t));

        // Alloc memory for specific widget attributes
        button->widget.content_rect = calloc(1, sizeof(ei_rect_t));

        return (ei_widget_t *) button;
}

/**
 * @brief       Allocate memory used by a top-level widget
 * @return      The corresponding widget
 */
ei_widget_t* top_level_alloc_func() {
        ei_top_level_t *toplevel = calloc(1, sizeof(ei_top_level_t));
        toplevel->resize_rect = calloc(1, sizeof(ei_rect_t));
        toplevel->top_bar = calloc(1, sizeof(ei_rect_t));

        // Alloc memory for specific widget attributes
        toplevel->widget.content_rect = calloc(1, sizeof(ei_rect_t));

        // Initialisation of close button widget
        toplevel->close_button = (ei_button_t *) button_alloc_func();
        toplevel->close_button->widget.parent = (ei_widget_t *) toplevel;

        return (ei_widget_t *) toplevel;
}

/**
 * @brief       Allocate memory used by a frame widget
 * @return      The corresponding widget
 */
ei_widget_t* frame_alloc_func() {
        ei_frame_t * frame = calloc(1, sizeof(ei_frame_t));

        // Alloc memory for specific widget attributes
        frame->widget.content_rect = calloc(1, sizeof(ei_rect_t));

        return (ei_widget_t *) frame;
}

/*
 * Release functions
 */


/**
 * @brief       Release memory for pointers attributes which were allocated in alloc function.
 *
 * @param       widget      The widget which resources are to be freed.
 */
void button_release(struct ei_widget_t*	widget) {
        // Cast into button widget to delete its ressources
        ei_button_t * button_widget = (ei_button_t*) widget;

        if (button_widget->text) free(button_widget->text);
        if (button_widget->img_rect) free(button_widget->img_rect);
        if (button_widget->img) hw_surface_free(button_widget->img);
}

/**
 * @brief       Release memory for pointers attributes which were allocated in alloc function.
 *
 * @param       widget      The widget which resources are to be freed.
 */
void top_level_release(struct ei_widget_t* widget) {
        // Cast into top_level widget to delete its ressources
        ei_top_level_t * top_level_widget = (ei_top_level_t*) widget;

        if (top_level_widget->title) free(top_level_widget->title);
        //if (top_level_widget->min_size) free(top_level_widget->min_size);
        if (top_level_widget->resize_rect) free(top_level_widget->resize_rect);
        if (top_level_widget->top_bar) free(top_level_widget->top_bar);

        // Delete and free close button

        // Call destructor if it provided by the user
        if (top_level_widget->close_button->widget.destructor) {
                top_level_widget->close_button->widget.destructor(widget);
        }
        button_release((ei_widget_t *) top_level_widget->close_button);
        free(top_level_widget->close_button->widget.pick_color);
        free(top_level_widget->close_button->widget.placer_params);
        top_level_widget->close_button->widget.parent = NULL;
        top_level_widget->close_button->widget.placer_params = NULL;
        top_level_widget->close_button->widget.content_rect = NULL;
        top_level_widget->close_button->widget.pick_color = NULL;
        free(top_level_widget->close_button);
}

/**
 * @brief       Release memory for pointers attributes which were allocated in alloc function.
 *
 * @param       widget      The widget which resources are to be freed.
 */
void frame_release(struct ei_widget_t* widget) {
        // Cast into frame widget to delete its ressources
        ei_frame_t * frame_widget = (ei_frame_t*) widget;

        if (frame_widget->text) free(frame_widget->text);
        if (frame_widget->img_rect) free(frame_widget->img_rect);
        if (frame_widget->img) hw_surface_free(frame_widget->img);
}

/*
 * Default set functions
 */

/**
 * \brief	A function that sets the default values for a widget button.
 *
 * @param	widget		A pointer to the widget instance to intialize.
 */
void set_default_button (ei_widget_t *widget) {
        // Cast into button widget to configure it
        ei_button_t *button_widget = (ei_button_t*) widget;

        // Set default params initialized in header file
        button_widget->widget.requested_size = default_button_size;
        button_widget->color = (ei_color_t) default_button_color;
        button_widget->border_width = (int)k_default_button_border_width;
        button_widget->corner_radius = (int)k_default_button_corner_radius;
        button_widget->text_color = (ei_color_t) ei_font_default_color;
        button_widget->text_font = ei_default_font;
        button_widget->text_anchor = default_text_button_anchor;
}

/**
 * \brief	A function that sets the default values for a widget frame.
 *
 * @param	widget		A pointer to the widget instance to intialize.
 */
void set_default_frame (ei_widget_t *widget) {
        // Cast into frame widget to configure it
        ei_frame_t * frame_widget = (ei_frame_t*) widget;

        // Set default params initialized in header file
        frame_widget->widget.requested_size = default_frame_size;
        frame_widget->color = default_frame_color;
        frame_widget->border_width = default_frame_border_width;
        frame_widget->text_color = (ei_color_t) ei_font_default_color;
        frame_widget->text_font = ei_default_font;
        frame_widget->text_anchor = default_text_frame_anchor;
}

/**
 * \brief	A function that sets the default values for a widget top-level.
 *
 * @param	widget		A pointer to the widget instance to intialize.
 */
void set_default_top_level (ei_widget_t *widget) {
        // Cast into top_level widget to configure it
        ei_top_level_t *top_level_widget = (ei_top_level_t*) widget;

        // Set default params initialized in header file
        top_level_widget->widget.requested_size = default_top_level_size;
        top_level_widget->color = default_top_level_color;
        top_level_widget->border_width = default_top_level_border_width;
        top_level_widget->closable = default_top_level_closable;
        top_level_widget->min_size = &default_top_level_min_size;
        top_level_widget->current_event = event_none;
}

/*
 * Geomnotify functions
 */

/**
 * \brief 	This function is called to notify the widget that its geometry has been modified
 *		by its geometry manager. Can set to NULL in \ref ei_widgetclass_t.
 *		Calculate or recalculated the content_rect attribute.
 *
 * @param	widget		The widget instance to notify of a geometry change.
 * @param	rect		The new rectangular screen location of the widget
 *				(i.e. = widget->screen_location).
 */
void button_geomnotifyfunc (struct ei_widget_t* widget, ei_rect_t rect) {
        // Init
        widget->screen_location = rect;
        ei_button_t *button = (ei_button_t *) widget;

        int32_t borders_to_remove = button->border_width * 2;

        // Calculate size and place of content rect
        ei_size_t size_content_rect = {widget->screen_location.size.width - borders_to_remove,
                                       widget->screen_location.size.height -  borders_to_remove};

        ei_point_t place_content_rect = {widget->screen_location.top_left.x + button->border_width, widget->screen_location.top_left.y + button->border_width};

        // Specify content_rect
        button->widget.content_rect->size = size_content_rect;
        button->widget.content_rect->top_left = place_content_rect;

         // Set top-left position to 0 if it is negative and reduce width in this case
         if (widget->screen_location.top_left.x < 0) {
                 widget->screen_location.size.width += rect.top_left.x;
                 widget->screen_location.top_left.x = 0;
         }
         if (rect.top_left.y < 0) {
                 widget->screen_location.size.height += rect.top_left.y;
                 widget->screen_location.top_left.y = 0;
         }
}

/**
 * \brief 	This function is called to notify the widget that its geometry has been modified
 *		by its geometry manager. Can set to NULL in \ref ei_widgetclass_t.
 *		Calculate or recalculated the content_rect attribute.
 *
 * @param	widget		The widget instance to notify of a geometry change.
 * @param	rect		The new rectangular screen location of the widget
 *				(i.e. = widget->screen_location).
 */
void frame_geomnotifyfunc (struct ei_widget_t* widget, ei_rect_t rect) {
        // Init
        widget->screen_location = rect;
        ei_frame_t *frame = (ei_frame_t *) widget;

        int32_t borders_to_remove = frame->border_width * 2;

        // Calculate size and place of content rect
        ei_size_t size_content_rect = {widget->screen_location.size.width - borders_to_remove,
                                       widget->screen_location.size.height -  borders_to_remove};

        ei_point_t place_content_rect = {widget->screen_location.top_left.x + frame->border_width, widget->screen_location.top_left.y + frame->border_width};

        // Specify content_rect
        frame->widget.content_rect->size = size_content_rect;
        frame->widget.content_rect->top_left = place_content_rect;

        // Set top-left position to 0 if it is negative and reduce width in this case
        if (widget->screen_location.top_left.x < 0) {
                widget->screen_location.size.width += rect.top_left.x;
                widget->screen_location.top_left.x = 0;
        }
        if (rect.top_left.y < 0) {
                widget->screen_location.size.height += rect.top_left.y;
                widget->screen_location.top_left.y = 0;
        }
}

/**
 * \brief 	This function is called to notify the widget that its geometry has been modified
 *		by its geometry manager. Can set to NULL in \ref ei_widgetclass_t.
 *		Calculate or recalculated the content_rect attribute.
 *		Configure or reconfigure the close button.
 *
 * @param	widget		The widget instance to notify of a geometry change.
 * @param	rect		The new rectangular screen location of the widget
 *				(i.e. = widget->screen_location).
 */
void top_level_geomnotifyfunc (struct ei_widget_t* widget, ei_rect_t rect) {
        // Init
        ei_top_level_t *top_level_widget = (ei_top_level_t *) widget;
        widget->screen_location = rect;

        // Get text size
        ei_size_t *text_size = calloc(1, sizeof(ei_size_t));
        hw_text_compute_size(top_level_widget->title, ei_default_font, &(text_size->width), &(text_size->height));

        // Resize screen_location including border and top_bar
        widget->screen_location.size.width += 2*(top_level_widget->border_width);
        widget->screen_location.size.height += (top_level_widget->border_width + text_size->height);

        // Get size and place parameters
        int place_x = top_level_widget->widget.screen_location.top_left.x;
        int place_y = top_level_widget->widget.screen_location.top_left.y;

        // Set size and place for the rectangle used to model the content part of the top level (all without border)
        ei_size_t size_content_rect = rect.size;
        ei_point_t place_content_rect = {place_x + top_level_widget->border_width, place_y + text_size->height};

        // Specify content_rect
        top_level_widget->widget.content_rect->size = size_content_rect;
        top_level_widget->widget.content_rect->top_left = place_content_rect;

        // Store top-bar
        top_level_widget->top_bar->top_left = top_level_widget->widget.screen_location.top_left;
        top_level_widget->top_bar->size = ei_size(top_level_widget->widget.screen_location.size.width, top_level_widget->widget.screen_location.size.height - top_level_widget->widget.content_rect->size.height);

        // Close button configuration
        int close_button_x = widget->screen_location.top_left.x + (top_level_widget->top_bar->size.height / 4);
        int close_button_y = widget->screen_location.top_left.y + (top_level_widget->top_bar->size.height / 2);

        int close_button_width_height = top_level_widget->top_bar->size.height / 2;
        ei_size_t close_button_size = {close_button_width_height, close_button_width_height};

        int close_button_corner_radius = (top_level_widget->top_bar->size.height/ 2) / 2;
        ei_color_t close_button_color = {0xF9, 0x38, 0x22, 0xff};
        ei_relief_t close_button_relief = ei_relief_raised;

        ei_button_configure((ei_widget_t*) top_level_widget->close_button, &close_button_size, &close_button_color, 0, &close_button_corner_radius, &close_button_relief, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

        // Params widget attributes of close button
        top_level_widget->close_button->widget.screen_location.top_left.x = close_button_x;
        top_level_widget->close_button->widget.screen_location.top_left.y = close_button_y - (close_button_width_height/ 2);
        top_level_widget->close_button->widget.screen_location.size = close_button_size;
        top_level_widget->close_button->widget.content_rect = &top_level_widget->close_button->widget.screen_location;
        top_level_widget->close_button->relief = close_button_relief;

        // Free memory
        free(text_size);

        // Resizable rect
        top_level_widget->resize_rect->top_left.x = (widget->screen_location.top_left.x + widget->screen_location.size.width) - default_top_level_rect_resize;
        top_level_widget->resize_rect->top_left.y = (widget->screen_location.top_left.y + widget->screen_location.size.height) - default_top_level_rect_resize;
        top_level_widget->resize_rect->size.width = default_top_level_rect_resize;
        top_level_widget->resize_rect->size.height = default_top_level_rect_resize;
}

/**
 * @brief	Returns the widget that is at a given location on screen.
 *
 * @param	where		The location on screen, expressed in the root window coordinates.
 *
 * @return			The top-most widget at this location, or NULL if there is no widget
 *				at this location (except for the root widget).
 */
ei_widget_t*		ei_widget_pick			(ei_point_t*		where){
        // Otherwise, we search the event to treat
        // Parameters of the offscreen
        hw_surface_lock(g_offscreen);
        uint32_t *clicked_pixel = (uint32_t *) hw_surface_get_buffer(g_offscreen);
        ei_size_t offscreen_size = hw_surface_get_size(g_offscreen);

        // Compute memory location of clicked_pixel and put the its value in widget_id
        clicked_pixel += (offscreen_size.width * where->y) + where->x;
        uint32_t widget_id = *clicked_pixel;

        // Test if the clicked pixel is in the g_root_frame
        ei_widget_t *widget_to_treat = g_root_frame;
        if (widget_to_treat->pick_id == widget_id){
                return widget_to_treat;
        }

        // Depth course of each widgets
        do {
                if (widget_to_treat->children_head) {
                        widget_to_treat = widget_to_treat->children_head;
                        if (widget_to_treat->pick_id == widget_id){
                                return widget_to_treat;
                        }
                } else {
                        while (widget_to_treat != g_root_frame && widget_to_treat->next_sibling == NULL) {
                                widget_to_treat = widget_to_treat->parent;
                        }

                        if (widget_to_treat->next_sibling) {
                                widget_to_treat = widget_to_treat->next_sibling;
                                if (widget_to_treat->pick_id == widget_id){
                                        return widget_to_treat;
                                }
                        }
                }
        } while (widget_to_treat != g_root_frame);
}

/*
 * Configure functions
 */

/**
 * @brief	Configures the attributes of widgets of the class "frame".
 *
 *		Parameters obey the "default" protocol: if a parameter is "NULL" and it has never
 *		been defined before, then a default value should be used (default values are
 *		specified for each parameter). If the parameter is "NULL" but was defined on a
 *		previous call, then its value must not be changed.
 *
 * @param	widget		The widget to configure.
 * @param	requested_size	The size requested for this widget, including the widget's borders.
 *				The geometry manager may override this size due to other constraints.
 *				Defaults to the "natural size" of the widget, ie. big enough to
 *				display the border and the text or the image. This may be (0, 0)
 *				if the widget has border_width=0, and no text and no image.
 * @param	color		The color of the background of the widget. Defaults to
 *				\ref ei_default_background_color.
 * @param	border_width	The width in pixel of the border decoration of the widget. The final
 *				appearance depends on the "relief" parameter. Defaults to 0.
 * @param	relief		Appearance of the border of the widget. Defaults to
 *				\ref ei_relief_none.
 * @param	text		The text to display in the widget, or NULL. Only one of the
 *				parameter "text" and "img" should be used (i.e. non-NULL). Defaults
 *				to NULL.
 * @param	text_font	The font used to display the text. Defaults to \ref ei_default_font.
 * @param	text_color	The color used to display the text. Defaults to
 *				\ref ei_font_default_color.
 * @param	text_anchor	The anchor of the text, i.e. where it is placed within the widget.
 *				Defines both the anchoring point on the parent and on the widget.
 *				Defaults to \ref ei_anc_center.
 * @param	img		The image to display in the widget, or NULL. Any surface can be
 *				used, but usually a surface returned by \ref hw_image_load. Only one
 *				of the parameter "text" and "img" should be used (i.e. non-NULL).
 				Defaults to NULL.
 * @param	img_rect	If not NULL, this rectangle defines a subpart of "img" to use as the
 *				image displayed in the widget. Defaults to NULL.
 * @param	img_anchor	The anchor of the image, i.e. where it is placed within the widget
 *				when the size of the widget is bigger than the size of the image.
 *				Defaults to \ref ei_anc_center.
 */
void			ei_frame_configure		(ei_widget_t*		widget,
                                                       ei_size_t*		requested_size,
                                                       const ei_color_t*	color,
                                                       int*			border_width,
                                                       ei_relief_t*		relief,
                                                       char**			text,
                                                       ei_font_t*		text_font,
                                                       ei_color_t*		text_color,
                                                       ei_anchor_t*		text_anchor,
                                                       ei_surface_t*		img,
                                                       ei_rect_t**		img_rect,
                                                       ei_anchor_t*		img_anchor) {
        // Cast into frame widget to configure it
        ei_frame_t * frame_widget = (ei_frame_t*) widget;

        if (requested_size) {
                frame_widget->widget.requested_size = *requested_size;
                if (frame_widget->widget.placer_params) {
                        // If there is already a placer, change width and height
                        frame_widget->widget.placer_params->w_data = requested_size->width;
                        frame_widget->widget.placer_params->h_data = requested_size->height;
                }
                ei_place(widget, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        }

        frame_widget->color = color != NULL ? *color : frame_widget->color;
        frame_widget->border_width = border_width != NULL ? *border_width : frame_widget-> border_width;
        frame_widget->relief = relief != NULL ? *relief : frame_widget-> relief;

        if (text) {
                // Delete the old text if exists
                if (frame_widget->text) {
                        free(frame_widget->text);
                }

                // The new text must be copy into text attribute of frame widget
                frame_widget->text = calloc(strlen(*text) + 1, sizeof(char));
                strcpy(frame_widget->text, *text);
        }


        if (img_rect) {
                if (frame_widget->img_rect == NULL) {
                        // Allocate memory if its the first time
                        frame_widget->img_rect = calloc(1, sizeof(ei_rect_t));
                }
                frame_widget->img_rect->size = (*img_rect)->size;
                frame_widget->img_rect->top_left = (*img_rect)->top_left;
        }


        if (img) {
                // Create a new surface with size of img parameter
                frame_widget->img = hw_surface_create(*img, hw_surface_get_size(*img), EI_FALSE);
                // Copy img surface parameter into the surface created
                ei_copy_surface(frame_widget->img, NULL, *img, NULL, EI_TRUE);
        }


        frame_widget->text_font = text_font != NULL ? *text_font : frame_widget-> text_font;
        frame_widget->text_color = text_color != NULL ? *text_color : frame_widget-> text_color;
        frame_widget->text_anchor = text_anchor != NULL ? *text_anchor : frame_widget-> text_anchor;
        frame_widget->img_anchor = img_anchor != NULL ? *img_anchor : frame_widget-> img_anchor;
}


/**
 * @brief	Configures the attributes of widgets of the class "button".
 *
 * @param	widget, requested_size, color, border_width, relief,
 *		text, text_font, text_color, text_anchor,
 *		img, img_rect, img_anchor
 *				See the parameter definition of \ref ei_frame_configure. The only
 *				difference is that relief defaults to \ref ei_relief_raised
 *				and border_width defaults to \ref k_default_button_border_width.
 * @param	corner_radius	The radius (in pixels) of the rounded corners of the button.
 *				0 means straight corners. Defaults to \ref k_default_button_corner_radius.
 * @param	callback	The callback function to call when the user clicks on the button.
 *				Defaults to NULL (no callback).
 * @param	user_param	A programmer supplied parameter that will be passed to the callback
 *				when called. Defaults to NULL.
 */
void			ei_button_configure		(ei_widget_t*		widget,
                                                        ei_size_t*		requested_size,
                                                        const ei_color_t*	color,
                                                        int*			border_width,
                                                        int*			corner_radius,
                                                        ei_relief_t*		relief,
                                                        char**			text,
                                                        ei_font_t*		text_font,
                                                        ei_color_t*		text_color,
                                                        ei_anchor_t*		text_anchor,
                                                        ei_surface_t*		img,
                                                        ei_rect_t**		img_rect,
                                                        ei_anchor_t*		img_anchor,
                                                        ei_callback_t*		callback,
                                                        void**			user_param) {
        // Cast into button widget to configure it
        ei_button_t * button_widget = (ei_button_t*) widget;

        button_widget->widget.requested_size = requested_size != NULL ? *requested_size : button_widget->widget.requested_size;
        button_widget->color = color != NULL ? *color : button_widget->color;
        button_widget->border_width = border_width != NULL ? *border_width : button_widget-> border_width;
        button_widget->corner_radius = corner_radius != NULL ? *corner_radius : button_widget->corner_radius;
        button_widget->relief = relief != NULL ? *relief : button_widget->relief;

        if (text) {
                // Delete the old text if exists
                if (button_widget->text) {
                        free(button_widget->text);
                }

                // The new text must be copy into text attribute of frame widget
                button_widget->text = calloc(strlen(*text) + 1, sizeof(char));
                strcpy(button_widget->text, *text);
        }


        if (img_rect) {
                if (button_widget->img_rect == NULL) {
                        // Allocate memory if its the first time
                        button_widget->img_rect = calloc(1, sizeof(ei_rect_t));
                }
                button_widget->img_rect->size = (*img_rect)->size;
                button_widget->img_rect->top_left = (*img_rect)->top_left;
        }

        if (img) {
                // Create a new surface with size of img parameter
                button_widget->img = hw_surface_create(*img, hw_surface_get_size(*img), EI_FALSE);
                // Copy img surface parameter into the surface created
                ei_copy_surface(button_widget->img, NULL, *img, NULL, EI_TRUE);
        }

        button_widget->text_font = text_font != NULL ? *text_font : button_widget->text_font;
        button_widget->text_color = text_color != NULL ? *text_color : button_widget->text_color;
        button_widget->text_anchor = text_anchor != NULL ? *text_anchor : button_widget->text_anchor;
        button_widget->img_anchor = img_anchor != NULL ? *img_anchor : button_widget->img_anchor;
        button_widget->callback = callback != NULL ? *callback : button_widget->callback;
        button_widget->user_param = user_param != NULL ? *user_param : button_widget->user_param;
}

/**
 * @brief	Configures the attributes of widgets of the class "toplevel".
 *
 * @param	widget		The widget to configure.
 * @param	requested_size	The content size requested for this widget, this does not include
 *				the decorations	(border, title bar). The geometry manager may
 *				override this size due to other constraints.
 *				Defaults to (320x240).
 * @param	color		The color of the background of the content of the widget. Defaults
 *				to \ref ei_default_background_color.
 * @param	border_width	The width in pixel of the border of the widget. Defaults to 4.
 * @param	title		The string title displayed in the title bar. Defaults to "Toplevel".
 *				Uses the font \ref ei_default_font.
 * @param	closable	If true, the toplevel is closable by the user, the toplevel must
 *				show a close button in its title bar. Defaults to \ref EI_TRUE.
 * @param	resizable	Defines if the widget can be resized horizontally and/or vertically
 *				by the user. Defaults to \ref ei_axis_both.
 * @param	min_size	For resizable widgets, defines the minimum size of its content.
 *				The default minimum size of a toplevel is (160, 120).
 *				If *min_size is NULL, this requires the toplevel to be configured to
 *				its default size.
 */
void			ei_toplevel_configure		(ei_widget_t*		widget,
                                                          ei_size_t*		requested_size,
                                                          ei_color_t*		color,
                                                          int*			border_width,
                                                          char**		title,
                                                          ei_bool_t*		closable,
                                                          ei_axis_set_t*	resizable,
                                                          ei_size_t**		min_size) {
        // Cast into top_level widget to configure it
        ei_top_level_t * top_level_widget = (ei_top_level_t*) widget;

        top_level_widget->widget.requested_size = requested_size != NULL ? *requested_size : top_level_widget->widget.requested_size;
        top_level_widget->color = color != NULL ? *color : top_level_widget->color;
        top_level_widget->closable = closable != NULL ? *closable :top_level_widget->closable;
        top_level_widget->resizable = resizable != NULL ? *resizable :top_level_widget->resizable;

        if (min_size) {
                free(top_level_widget->min_size);
                top_level_widget->min_size = malloc(sizeof(ei_size_t));
                top_level_widget->min_size = *min_size;
        }

        if (border_width) {
                if (top_level_widget->widget.placer_params && top_level_widget->border_width != *border_width) {
                        // Case when top level has already called ei_place
                        top_level_widget->border_width = *border_width;

                        // Call the placer
                        ei_placer_run(widget);
                } else {
                        // Configure attributes for the first time
                        top_level_widget->border_width = *border_width;
                }
        }

        if (title) {
                if (top_level_widget->widget.placer_params && top_level_widget->title != *title) {
                        // Case when top level has already called ei_place
                        free(top_level_widget->title);
                        top_level_widget->title = calloc(strlen(*title) + 1, sizeof(char));
                        strcpy(top_level_widget->title, *title);

                        // Call the placer
                        ei_placer_run(widget);
                } else {
                        // Configure attributes for the first time
                        free(top_level_widget->title);
                        top_level_widget->title = calloc(strlen(*title) + 1, sizeof(char));
                        strcpy(top_level_widget->title, *title);
                }
        }
}

/**
 * @brief	Creates a new instance of a widget of some particular class, as a descendant of
 *		an existing widget.
 *
 *		The widget is not displayed on screen until it is managed by a geometry manager.
 *		When no more needed, the widget must be released by calling \ref ei_widget_destroy.
 *
 * @param	class_name	The name of the class of the widget that is to be created.
 * @param	parent 		A pointer to the parent widget. Can not be NULL.
 * @param	user_data	A pointer provided by the programmer for private use. May be NULL.
 * @param	destructor	A pointer to a function to call before destroying a widget structure. May be NULL.
 *
 * @return			The newly created widget, or NULL if there was an error.
 */
ei_widget_t*		ei_widget_create		(ei_widgetclass_name_t	class_name,
                                                             ei_widget_t*		parent,
                                                             void*			user_data,
                                                             ei_widget_destructor_t destructor) {
        // ll_classes is a linked list of ei_widgetclass_t elements.
        ei_widgetclass_t *ll_classes = get_linked_list_classes();
        ei_widgetclass_t *class = get_class(ll_classes, class_name);

        if (class){
                // Initialisation by functions of ei_widgetclass_t
                ei_widget_t* widget_to_return = class->allocfunc();
                class->setdefaultsfunc(widget_to_return);

                // Initialisation of ei_widget_t elements.
                // We must to survey the value of the following arguments.
                widget_to_return->wclass = class;
                widget_to_return->parent = parent;
                widget_to_return->user_data = user_data;
                widget_to_return->destructor = destructor;

                // Update parent's child, only if parent exists
                if (parent) {
                        uint32_t last_widget_id = last_id();
                        insert_child(widget_to_return, parent);
                        widget_to_return->pick_id = last_widget_id + 1;
                        widget_to_return->pick_color = inverse_map_rgba(g_offscreen, widget_to_return->pick_id);
                }

                // We return a pointer on the new ei_widget_t
                return widget_to_return;
        }

        // If class is NULL so there isn't widget of class class_name !
        return NULL;
}

/**
 * @brief       Delete and free from memory the widget given in parameter
 *
 * @param       widget      The widget to destroy
 */
static void free_widget(ei_widget_t *widget) {
        // Call destructor if it provided by the user
        if (widget->destructor) {
                widget->destructor(widget);
        }
        ei_placer_forget(widget);
        widget->wclass->releasefunc(widget);
        free(widget->pick_color);
        free(widget->content_rect);
        widget->pick_color = NULL;
        widget->content_rect = NULL;
        free(widget);
}

/**
 * @brief	Destroys a widget. Calls its destructor if it was provided.
 * 		Removes the widget from the screen if it is currently managed by the placer.
 * 		Destroys all its descendants.
 *
 * @param	widget		The widget that is to be destroyed.
 */
void			ei_widget_destroy		(ei_widget_t*		widget) {
        ei_widget_t * widget_to_destroy = widget;
        ei_widget_t *tmp_to_destroy;

        // Depth course of each widgets
        do {
                if (widget_to_destroy->children_head) {
                        widget_to_destroy = widget_to_destroy->children_head;
                } else {
                        while (widget_to_destroy != widget && widget_to_destroy->next_sibling == NULL) {
                                tmp_to_destroy = widget_to_destroy;
                                widget_to_destroy = widget_to_destroy->parent;
                                free_widget(tmp_to_destroy);
                        }

                        if (widget_to_destroy->next_sibling && widget_to_destroy != widget) {
                                tmp_to_destroy = widget_to_destroy;
                                widget_to_destroy = widget_to_destroy->next_sibling;
                                free_widget(tmp_to_destroy);
                        }
                }
        } while (widget_to_destroy != widget);

        // Forget placer param for the widget given in param and its children
        ei_placer_forget(widget_to_destroy);

        // The final widget correspond to the widget given in param
        free_widget(widget_to_destroy);
}

/**
 * @brief       Give coordinates of top-left point where a text must be display depending on the anchor
 *
 * @param       text_anchor         The text anchor
 * @param       text_size           The text size
 * @param       widget_place        The place where the widget parent is
 * @param       widget_size         The size of the widget parent
 *
 * @return      A point which represents coordinates of top-left text place
 */
 ei_point_t* text_place(ei_anchor_t *text_anchor, ei_size_t *text_size, ei_point_t *widget_place, ei_size_t *widget_size) {
         // Allocate memory
        ei_point_t *text_coord = malloc(sizeof(ei_point_t));

        // Adapt top-left coordinates at the anchor given in parameter
        switch (*text_anchor) {
                case ei_anc_center:
                        text_coord->x = widget_place->x + (widget_size->width - text_size->width) / 2;
                        text_coord->y = widget_place->y + (widget_size->height - text_size->height) / 2;
                        break;
                case ei_anc_north:
                        text_coord->x = widget_place->x + (widget_size->width - text_size->width) / 2;
                        text_coord->y = widget_place->y;
                        break;
                case ei_anc_northeast:
                        text_coord->x = widget_place->x + (widget_size->width - text_size->width);
                        text_coord->y = widget_place->y;
                        break;
                case ei_anc_northwest:
                        text_coord->x = widget_place->x;
                        text_coord->y = widget_place->y;
                        break;
                case ei_anc_south:
                        text_coord->x = widget_place->x + (widget_size->width - text_size->width) / 2;
                        text_coord->y = widget_place->y + (widget_size->height - text_size->height);
                        break;
                case ei_anc_southeast:
                        text_coord->x = widget_place->x + (widget_size->width - text_size->width);
                        text_coord->y = widget_place->y + (widget_size->height - text_size->height);
                        break;
                case ei_anc_southwest:
                        text_coord->x = widget_place->x;
                        text_coord->y = widget_place->y + (widget_size->height - text_size->height);
                        break;
                case ei_anc_east:
                        text_coord->x = widget_place->x + (widget_size->width - text_size->width);
                        text_coord->y = widget_place->y + (widget_size->height - text_size->height) / 2;
                        break;
                case ei_anc_west:
                        text_coord->x = widget_place->x;
                        text_coord->y = widget_place->y + (widget_size->height - text_size->height) / 2;
                        break;
                case ei_anc_none:
                        text_coord->x = widget_place->x + (widget_size->width - text_size->width) / 2;
                        text_coord->y = widget_place->y + (widget_size->height - text_size->height) / 2;
                        break;
        }
        return text_coord;
}