#ifndef PROJETC_IG_WIDGET_MANAGER_H
#define PROJETC_IG_WIDGET_MANAGER_H

#include <string.h>

#include "ei_widget.h"
#include "ei_widgetclass.h"
#include "application.h"

typedef struct ei_top_level_t {
        ei_widget_t		widget;
        ei_color_t*		color;
        int*			border_width;
        char**			title;
        ei_bool_t*		closable;
        ei_axis_set_t*		resizable;
        ei_size_t**		min_size;
} ei_top_level_t;

typedef struct ei_button_t {
        ei_widget_t		widget;
        const ei_color_t*	color;
        int*			border_width;
        int*			corner_radius;
        ei_relief_t*		relief;
        char**			text;
        ei_font_t*		text_font;
        ei_color_t*		text_color;
        ei_anchor_t*		text_anchor;
        ei_surface_t*		img;
        ei_rect_t**		img_rect;
        ei_anchor_t*		img_anchor;
        ei_callback_t*		callback;
        void**			user_param;
} ei_button_t;

typedef struct ei_frame_t {
        ei_widget_t		widget;
        const ei_color_t*	color;
        int*			border_width;
        ei_relief_t*		relief;
        char**			text;
        ei_font_t*		text_font;
        ei_color_t*		text_color;
        ei_anchor_t*		text_anchor;
        ei_surface_t*		img;
        ei_rect_t**		img_rect;
        ei_anchor_t*		img_anchor;
} ei_frame_t;


// TODO : voir dernière fonction de ei_widgetclass.h
ei_widgetclass_t *get_class(ei_widgetclass_t *ll, ei_widgetclass_name_t class_name);


/**
 * Allocation memory
 * @return
 */
ei_widget_t* button_alloc_func();
ei_widget_t* top_level_alloc_func();
ei_widget_t* frame_alloc_func();

/**
 * Release memory
 * @return
 */
void button_release(ei_widget_t* widget);
void top_level_release(ei_widget_t* widget);
void frame_release(ei_widget_t* widget);


/**
 * Draw functions
 */
void ei_draw_button (ei_widget_t* widget,
                 ei_surface_t		surface,
                 ei_surface_t		pick_surface,
                 ei_rect_t*		clipper);

void ei_draw_frame (ei_widget_t* widget,
                    ei_surface_t		surface,
                    ei_surface_t		pick_surface,
                    ei_rect_t*		clipper);

void ei_draw_top_level (ei_widget_t* widget,
                    ei_surface_t		surface,
                    ei_surface_t		pick_surface,
                    ei_rect_t*		clipper);

void set_default_button (ei_widget_t *widget);
void set_default_frame (ei_widget_t *widget);
void set_default_top_level (ei_widget_t *widget);

void button_geomnotifyfunc (struct ei_widget_t* widget, ei_rect_t rect);
void frame_geomnotifyfunc (struct ei_widget_t* widget, ei_rect_t rect);
void top_level_geomnotifyfunc (struct ei_widget_t* widget, ei_rect_t rect);

/**
 * Default values of widget parameters
 */
// BUTTON
static ei_size_t default_button_size = {50, 30};
static ei_color_t default_button_color = {0x7C, 0x87, 0x8E, 0xff};
static ei_anchor_t default_text_button_anchor = ei_anc_center;

// FRAME
static ei_size_t default_frame_size = {400, 400};
static ei_color_t default_frame_color = {0x89, 0xAb, 0xE3, 0xff};
static int default_frame_border_width = 0;
static ei_anchor_t default_text_frame_anchor = ei_anc_center;

// TOP LEVEL
static ei_size_t default_top_level_size = {400, 50};
static ei_color_t default_top_level_color = {0xD0, 0xD3, 0xD4, 0xff};
static int default_top_level_border_width = 0;
static ei_bool_t default_top_level_closable = EI_TRUE;

/**
 * @brief       Give coordinates of top-left point where a text must be display depending on the anchor
 * @param text_anchor
 * @param text_size
 * @param widget_place
 * @param widget_size
 * @return
 */
ei_point_t* text_place(ei_anchor_t *text_anchor, ei_size_t *text_size, ei_point_t *widget_place, ei_size_t *widget_size);

/**
 * @brief Inverse function of map_rgba ei_draw.c's modul
 *
 * @param a uint32_t element which is convert into ei_color_t
 *
 * @return ei_color_t corresponding to the 32 bits give as argument
 */
ei_color_t *inverse_map_rgba(ei_surface_t surface, uint32_t color_to_convert);

#endif //PROJETC_IG_WIDGET_MANAGER_H
