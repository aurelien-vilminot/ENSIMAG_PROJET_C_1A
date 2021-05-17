#include "ei_widget.h"

#include "widget_manager.h"

/**
 * @brief       Update child field of parent. It is a course of linked list.
 *
 * @param       widget      The widget child
 * @param       parent      The child's parent which the children field needs to be updated
 */
static void insert_child(ei_widget_t *widget, ei_widget_t *parent) {
        if (parent->children_head) {
                ei_widget_t *current_child = parent->children_head;
                while (current_child->next_sibling) {
                        current_child = current_child->next_sibling;
                }

                current_child->next_sibling = widget;
                parent->children_tail = widget;

        } else {
                parent->children_head = widget;
                parent->children_tail = widget;
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

                // Initialisation of ei_widget_t attributs.
                // We must to survey the value of the following arguments.
                // Do we have to change pick_id etc ?
                // Please note that none geometrical element is initialised
                widget_to_return->wclass = class;
                widget_to_return->parent = parent;

                // Update parent's child, only if parent exists
                if (parent) insert_child(widget_to_return, parent);
                widget_to_return->user_data = user_data;
                widget_to_return->destructor = destructor;

                // We return a pointer on the new ei_widget_t
                return widget_to_return;
        }

        // If class is NULL so there isn't widget of class class_name !
        return NULL;
}

/**
 * @brief	Destroys a widget. Calls its destructor if it was provided.
 * 		Removes the widget from the screen if it is currently managed by the placer.
 * 		Destroys all its descendants.
 *
 * @param	widget		The widget that is to be destroyed.
 */
void			ei_widget_destroy		(ei_widget_t*		widget) {
        // Depth course of each widgets
        if (widget->children_head != NULL) {

                // Get the first child and the first child of its child (this could be NULL)
                ei_widget_t *current_widget = widget->children_head;
                ei_widget_t *first_next_child = current_widget->children_head;

                do {
                        while (current_widget != NULL) {
                                if (first_next_child == NULL) {
                                        // Put the first child, if it doesn't store
                                        first_next_child = current_widget->children_head;
                                }

                                // Call destructor if it provided
                                if (current_widget->destructor) {
                                        current_widget->destructor(current_widget);
                                }

                                current_widget = current_widget->next_sibling;
                        }

                        // Change deep level
                        current_widget = first_next_child;
                        first_next_child = NULL;
                } while (current_widget != NULL);
        }
}


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

/**
 * Allocation memory
 * @return
 */
ei_widget_t* button_alloc_func() {
        ei_button_t * m_button = calloc(1, sizeof(ei_button_t));

        // Allocate memory for all attributes
        m_button->color = calloc(1, sizeof(ei_color_t));
        m_button->border_width = calloc(1, sizeof(int));
        m_button->corner_radius = calloc(1, sizeof(int));
        m_button->relief = calloc(1, sizeof(ei_relief_t));
        m_button->text = calloc(1, sizeof(char*));
        m_button->text_font = calloc(1, sizeof(ei_font_t));
        m_button->text_color = calloc(1, sizeof(ei_color_t));
        m_button->text_anchor = calloc(1, sizeof(ei_anchor_t));
        m_button->img = calloc(1, sizeof(ei_surface_t));
        m_button->img_rect = calloc(1, sizeof(ei_rect_t*));
        m_button->img_anchor = calloc(1, sizeof(ei_anchor_t));
        m_button->callback = calloc(1, sizeof(ei_callback_t));
        m_button->user_param = calloc(1, sizeof(void*));

        return (ei_widget_t *) m_button;
}

ei_widget_t* top_level_alloc_func() {
        ei_top_level_t * m_toplevel = calloc(1, sizeof(ei_top_level_t));

        // Allocate memory for all attributes

        m_toplevel->color = calloc(1, sizeof(ei_color_t));
        m_toplevel->border_width = calloc(1, sizeof(int));
        m_toplevel->title = calloc(1, sizeof(char*));
        m_toplevel->closable = calloc(1, sizeof(ei_bool_t));
        m_toplevel->resizable = calloc(1, sizeof(ei_axis_set_t));
        m_toplevel->min_size = calloc(1, sizeof(ei_size_t*));

        return (struct ei_widget_t *) m_toplevel;

}

ei_widget_t* frame_alloc_func() {
        ei_frame_t * m_frame = calloc(1, sizeof(ei_frame_t));

        //Allocate memory for all attributes

        m_frame->color = calloc(1, sizeof(ei_color_t));
        m_frame->border_width = calloc(1, sizeof(int));
        m_frame->relief = calloc(1, sizeof(ei_relief_t));
        m_frame->text = calloc(1, sizeof(char*));
        m_frame->text_font = calloc(1, sizeof(ei_font_t));
        m_frame->text_color = calloc(1, sizeof(ei_color_t));
        m_frame->text_anchor = calloc(1, sizeof(ei_anchor_t));
        m_frame->img = calloc(1, sizeof(ei_surface_t));
        m_frame->img_rect = calloc(1, sizeof(ei_rect_t));
        m_frame->img_anchor = calloc(1, sizeof(ei_anchor_t));

        return (ei_widget_t *) m_frame;
}

/**
 * Release memory
 * @return
 */
void button_release(struct ei_widget_t*	widget) {
        // Cast into button widget to delete its ressources
        ei_button_t * button_widget = (ei_button_t*) widget;

        if (button_widget->color) free(&button_widget->color);
        if (button_widget->border_width) free(&button_widget->border_width);
        if (button_widget->corner_radius) free(&button_widget->corner_radius);
        if (button_widget->relief) free(&button_widget->relief);
        if (button_widget->text) free(&button_widget->text);
        if (button_widget->text_font) free(&button_widget->text_font);
        if (button_widget->text_color) free(&button_widget->text_color);
        if (button_widget->text_anchor) free(&button_widget->text_anchor);
        if (button_widget->img) free(&button_widget->img);
        if (button_widget->img_rect) free(&button_widget->img_rect);
        if (button_widget->img_anchor) free(&button_widget->img_anchor);
        if (button_widget->callback) free(&button_widget->callback);
        if (button_widget->user_param) free(&button_widget->user_param);
}

void top_level_release(struct ei_widget_t* widget) {

        // Cast into top_level widget to delete its ressources
        struct ei_top_level_t * top_level_widget = (ei_top_level_t*) widget;

        if (top_level_widget->color) free(&top_level_widget->color);
        if (top_level_widget-> border_width) free(&top_level_widget->border_width);
        if (top_level_widget->title) free(&top_level_widget->title);
        if (top_level_widget-> closable) free(&top_level_widget->closable);
        if (top_level_widget->resizable) free(&top_level_widget->resizable);
        if (top_level_widget->min_size) free(&top_level_widget->min_size);
}

void frame_release(struct ei_widget_t* widget) {
        // Cast into frame widget to delete its ressources
        ei_frame_t * frame_widget = (ei_frame_t*) widget;

        if (frame_widget->color) free(&frame_widget->color);
        if (frame_widget->border_width) free(&frame_widget->border_width);
        if (frame_widget->relief) free(&frame_widget->relief);
        if (frame_widget->text) free(&frame_widget->text);
        if (frame_widget->text_font) free(&frame_widget->text_font);
        if (frame_widget->text_color) free(&frame_widget->text_color);
        if (frame_widget->text_anchor) free(&frame_widget->text_anchor);
        if (frame_widget->img) free(&frame_widget->img);
        if (frame_widget->img_rect) free(&frame_widget->img_rect);
        if (frame_widget->img_anchor) free(&frame_widget->img_anchor);
}

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

        frame_widget->widget.requested_size = requested_size != NULL ? *requested_size : frame_widget->widget.requested_size;
        frame_widget->color = color != NULL ? color : frame_widget->color;
        frame_widget->border_width = border_width != NULL ? border_width : frame_widget-> border_width;
        frame_widget->relief = relief != NULL ? relief : frame_widget-> relief;
        frame_widget->text = text != NULL ? text : frame_widget-> text;
        frame_widget->text_font = text_font != NULL ? text_font : frame_widget-> text_font;
        frame_widget->text_color = text_color != NULL ? text_color : frame_widget-> text_color;
        frame_widget->text_anchor = text_anchor != NULL ? text_anchor : frame_widget-> text_anchor;
        frame_widget->img = img != NULL ? img : frame_widget-> img;
        frame_widget->img_rect = img_rect != NULL ? img_rect : frame_widget-> img_rect;
        frame_widget->img_anchor = img_anchor != NULL ? img_anchor : frame_widget-> img_anchor;
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
        button_widget->color = color != NULL ? color : button_widget->color;
        button_widget->border_width = border_width != NULL ? border_width : button_widget-> border_width;
        button_widget->corner_radius = corner_radius != NULL ? corner_radius : button_widget->corner_radius;
        button_widget->relief = relief != NULL ? relief : button_widget->relief;
        button_widget->text = text != NULL ? text : button_widget->text;
        button_widget->text_font = text_font != NULL ? text_font : button_widget->text_font;
        button_widget->text_color = text_color != NULL ? text_color : button_widget->text_color;
        button_widget->text_anchor = text_anchor != NULL ? text_anchor : button_widget->text_anchor;
        button_widget->img = img != NULL ? img : button_widget->img;
        button_widget->img_rect = img_rect != NULL ? img_rect : button_widget->img_rect;
        button_widget->img_anchor = img_anchor != NULL ? img_anchor : button_widget->img_anchor;
        button_widget->callback = callback != NULL ? callback : button_widget->callback;
        button_widget->user_param = user_param != NULL ? user_param : button_widget->user_param;
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
                                                                  char**			title,
                                                                  ei_bool_t*		closable,
                                                                  ei_axis_set_t*		resizable,
                                                                  ei_size_t**		min_size) {
        // Cast into top_level widget to configure it
        ei_top_level_t * top_level_widget = (ei_top_level_t*) widget;

        top_level_widget->widget.requested_size = requested_size != NULL ? *requested_size : top_level_widget->widget.requested_size;
        top_level_widget->color = color != NULL ? color :top_level_widget->color;
        top_level_widget->border_width = border_width != NULL ? border_width : top_level_widget-> border_width;
        top_level_widget->title = title != NULL ? title :top_level_widget->title;
        top_level_widget->closable = closable != NULL ? closable :top_level_widget->closable;
        top_level_widget->resizable = resizable != NULL ? resizable :top_level_widget->resizable;
        top_level_widget->min_size = min_size != NULL ? min_size :top_level_widget->min_size;
}

void set_default_button (ei_widget_t *widget) {
        ei_button_t *button_widget = (ei_button_t*) widget;

        button_widget->widget.requested_size = default_button_size;
        button_widget->color = &default_button_color;
        button_widget->border_width = &default_button_border_width;
        button_widget->corner_radius = &default_button_corner_radius;
        button_widget->text_color = (ei_color_t*)&(ei_font_default_color);
        button_widget->text_font = hw_text_font_create(ei_default_font_filename, ei_style_normal, ei_font_default_size);
        button_widget->text_anchor = &default_button_anchor;
}

void set_default_frame (ei_widget_t *widget) {
        ei_frame_t * frame_widget = (ei_frame_t*) widget;

        ei_size_t *default_size = malloc(sizeof(ei_size_t));
        default_size->width = 800;
        default_size->height = 600;
        frame_widget->widget.requested_size = *default_size;

        ei_color_t *default_color = malloc(sizeof(ei_color_t));
        default_color->red = 0x89;
        default_color->green = 0xAb;
        default_color->blue = 0xE3;
        default_color->alpha = 0xff;
        frame_widget->color = default_color;

        int * default_border_width = malloc(sizeof(int));
        *default_border_width = 0;
        frame_widget->border_width = default_border_width;

        frame_widget->text_color = (ei_color_t*)&(ei_font_default_color);
        frame_widget->text_font = hw_text_font_create(ei_default_font_filename, ei_style_normal, ei_font_default_size);

        ei_anchor_t * default_anchor = malloc(sizeof(ei_anchor_t));
        *default_anchor = ei_anc_center;
        frame_widget->text_anchor = default_anchor;
}

void set_default_top_level (ei_widget_t *widget) {
        ei_top_level_t *top_level_widget = (ei_top_level_t*) widget;

        ei_size_t *default_size = malloc(sizeof(ei_size_t));
        default_size->width = 100;
        default_size->height = 30;
        top_level_widget->widget.requested_size = *default_size;

        ei_color_t *default_color = malloc(sizeof(ei_color_t));
        default_color->red = 0xD0;
        default_color->green = 0xD3;
        default_color->blue = 0xD4;
        default_color->alpha = 0xff;
        top_level_widget->color = default_color;

        int *default_border_width = malloc(sizeof(int));
        *default_border_width = 5;
        top_level_widget->border_width = default_border_width;

        ei_bool_t *default_closable = malloc(sizeof(ei_bool_t));
        *default_closable = EI_TRUE;
        top_level_widget->closable = default_closable;
}