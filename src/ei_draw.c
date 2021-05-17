#include "ei_draw.h"

#include "single_linked_list.h"
#include "ei_create_button.h"
#include "widget_manager.h"

/* Cette fonction doit peut-être directement d'adapter au fonctions qui l'utilisent.
 * En tout cas, il faut absolument qu'en paramètre soit donné les coordonnées limite et pas le clipper.
 * Les premières lignes de la fonction sont à écrires directement dans les fonctions qui l'utilisent.
 *
 * Elle retourne True si le point donné en paramètre se situe dans le clipper également donné en paramètre.
 * Sinon elle retourne False
 */


// TODO : mettre le if dans clipper (demander à Pierre pour inline)
static int is_in_clipper(int point_x, int point_y, uint32_t x_max, uint32_t y_max, const ei_rect_t* clipper) {
        return (point_x <= x_max) && (point_y <= y_max) && (point_x >= clipper->top_left.x) && (point_y >= clipper->top_left.y);
}


/**
 * \brief	Converts the red, green, blue and alpha components of a color into a 32 bits integer
 * 		than can be written directly in the memory returned by \ref hw_surface_get_buffer.
 * 		The surface parameter provides the channel order.
 *
 * @param	surface		The surface where to store this pixel, provides the channels order.
 * @param	color		The color to convert.
 *
 * @return 			The 32 bit integer corresponding to the color. The alpha component
 *				of the color is ignored in the case of surfaces that don't have an
 *				alpha channel.
 */
uint32_t		ei_map_rgba		(ei_surface_t surface, ei_color_t color) {

        int red_place;
        int green_place;
        int blue_place;
        int alpha_place;

        hw_surface_get_channel_indices(surface, &red_place, &green_place, &blue_place, &alpha_place);

        uint32_t *color_int = malloc(sizeof(uint32_t));
        uint8_t * p = (uint8_t *) color_int;

        p[blue_place] = color.blue;
        p[red_place] = color.red;
        p[green_place] = color.green;
        p[6 - (blue_place + red_place + green_place)] = color.alpha;

        return *color_int;
}

/**
 * \brief	Draws a line that can be made of many line segments.
 *
 * @param	surface 	Where to draw the line. The surface must be *locked* by
 *				\ref hw_surface_lock.
 * @param	first_point 	The head of a linked list of the points of the polyline. It can be NULL
 *				(i.e. draws nothing), can have a single point, or more.
 *				If the last point is the same as the first point, then this pixel is
 *				drawn only once.
 * @param	color		The color used to draw the line. The alpha channel is managed.
 * @param	clipper		If not NULL, the drawing is restricted within this rectangle.
 */
void			ei_draw_polyline	(ei_surface_t			surface,
                                                     const ei_linked_point_t*	first_point,
                                                     ei_color_t			color,
                                                     const ei_rect_t*		clipper) {
        hw_surface_lock(surface);
        uint32_t *first_pixel = (uint32_t*)hw_surface_get_buffer(surface);
        ei_size_t size = hw_surface_get_size(surface);


        if (first_point != NULL) {
                // Get int color
                uint32_t color_int = ei_map_rgba(surface, color);
                // Max coordinates of clipper
                uint32_t x_max;
                uint32_t y_max;
                if (clipper) {
                        x_max = clipper->top_left.x + clipper->size.width;
                        y_max = clipper->top_left.y + clipper->size.height;
                }

                if (first_point->next == NULL) {
                        // Case only one point
                        if (!clipper || is_in_clipper(first_point->point.x, first_point->point.y, x_max, y_max, clipper)) {
                                first_pixel[first_point->point.x + first_point->point.y * size.width] = color_int;
                        }
                } else {
                        // Case with few points
                        int32_t dx, dy, e;
                        while (first_point->next != NULL) {
                                ei_point_t p1 = first_point->point;
                                ei_point_t p2 = first_point->next->point;

                                dx = p2.x - p1.x;
                                dy = p2.y - p1.y;

                                if (dx != 0) {
                                        if (dx > 0) {
                                                if (dy != 0) {
                                                        // 1er cadran
                                                        if (dy > 0) {
                                                                // Vecteur diagonal/oblique 1er octant
                                                                if (dx >= dy) {
                                                                        e = dx;
                                                                        dx = e * 2;
                                                                        dy *= 2;

                                                                        do {
                                                                                ++p1.x;
                                                                                if (!clipper || is_in_clipper(p1.x, p1.y, x_max, y_max, clipper)){
                                                                                        first_pixel[p1.x + p1.y * size.width] = color_int;
                                                                                }

                                                                                e -= dy;
                                                                                if (e < 0) {
                                                                                        ++p1.y;
                                                                                        e += dx;
                                                                                }
                                                                        } while (p1.x != p2.x);
                                                                // Vecteur proche de la verticale 2nd octant
                                                                } else {
                                                                        e = dy;
                                                                        dy = e * 2;
                                                                        dx *= 2;
                                                                        // On trace la ligne
                                                                        do {
                                                                                if (!clipper || is_in_clipper(p1.x, p1.y, x_max, y_max, clipper)){
                                                                                        first_pixel[p1.x + p1.y * size.width] = color_int;
                                                                                }
                                                                                ++p1.y;
                                                                                e -= dx;
                                                                                if (e < 0) {
                                                                                        ++p1.x;
                                                                                        e += dy;
                                                                                }
                                                                        } while (p1.y != p2.y);
                                                                }
                                                        // Vecteur oblique 4me quadrant
                                                        } else {
                                                                // Vecteur oblique/diagonal 8me octant
                                                                if (dx >= -dy) {
                                                                        e = dx;
                                                                        dx = e * 2;
                                                                        dy *= 2;
                                                                        // On trace la ligne
                                                                        do {
                                                                                if (!clipper || is_in_clipper(p1.x, p1.y, x_max, y_max, clipper)){
                                                                                        first_pixel[p1.x + p1.y * size.width] = color_int;
                                                                                }
                                                                                ++p1.x;
                                                                                e += dy;
                                                                                if (e < 0) {
                                                                                        --p1.y;
                                                                                        e += dx;
                                                                                }
                                                                        } while (p1.x != p2.x);
                                                                // Vecteur proche verticale 7me octant
                                                                } else {
                                                                        e = dy;
                                                                        dy = e * 2;
                                                                        dx *= 2;
                                                                        // On trace la ligne
                                                                        do {
                                                                                if (!clipper || is_in_clipper(p1.x, p1.y, x_max, y_max, clipper)){
                                                                                        first_pixel[p1.x + p1.y * size.width] = color_int;
                                                                                }
                                                                                --p1.y;
                                                                                e += dx;
                                                                                if (e > 0) {
                                                                                        ++p1.x;
                                                                                        e += dy;
                                                                                }
                                                                        } while (p1.y != p2.y);
                                                                }
                                                        }
                                                // Vecteur horizontal droite
                                                } else {
                                                        // On trace la ligne
                                                        do {
                                                                if (!clipper || is_in_clipper(p1.x, p1.y, x_max, y_max, clipper)){
                                                                        first_pixel[p1.x + p1.y * size.width] = color_int;
                                                                }
                                                                ++p1.x;
                                                        } while (p1.x != p2.x);
                                                }
                                        // Cas dx < 0
                                        } else {
                                                // Cas dy != 0
                                                if (dy != 0) {
                                                        // On traite ici le 2nd cadran
                                                        if (dy > 0) {

                                                                if (-dx >= dy) {
                                                                        e = dx;
                                                                        dx *= 2;
                                                                        dy *= 2;
                                                                        // On trace la ligne
                                                                        do {
                                                                                if (!clipper || is_in_clipper(p1.x, p1.y, x_max, y_max, clipper)){
                                                                                        first_pixel[p1.x + p1.y * size.width] = color_int;
                                                                                }
                                                                                p1.x--;
                                                                                if ((e += dy) >= 0) {
                                                                                        p1.y++;
                                                                                        e += dx;
                                                                                }
                                                                        } while (p1.x != p2.x);
                                                                }
                                                                // On trace dans le 3eme octant
                                                                else {
                                                                        e = dy;
                                                                        dy *= 2;
                                                                        dx *= 2;
                                                                        do {
                                                                                if (!clipper || is_in_clipper(p1.x, p1.y, x_max, y_max, clipper)){
                                                                                        first_pixel[p1.x + p1.y * size.width] = color_int;
                                                                                }
                                                                                p1.y++;
                                                                                if ((e += dx) <= 0) {
                                                                                        p1.x--;
                                                                                        e += dy;
                                                                                }
                                                                        } while (p1.y != p2.y);
                                                                }
                                                        }
                                                        // Case dy < 0, we write in the 3rd cadran
                                                        else {
                                                                if (dx <= dy) {
                                                                        e = dx;
                                                                        dx *= 2;
                                                                        dy *= 2;
                                                                        do {
                                                                                if (!clipper || is_in_clipper(p1.x, p1.y, x_max, y_max, clipper)){
                                                                                        first_pixel[p1.x + p1.y * size.width] = color_int;
                                                                                }
                                                                                p1.x--;
                                                                                if ((e -= dy) >= 0) {
                                                                                        p1.y--;
                                                                                        e += dx;
                                                                                }
                                                                        } while (p1.x != p2.x);
                                                                } else {
                                                                        e = dy;
                                                                        dy *= 2;
                                                                        dx *= 2;
                                                                        do {
                                                                                if (!clipper || is_in_clipper(p1.x, p1.y, x_max, y_max, clipper)){
                                                                                        first_pixel[p1.x + p1.y * size.width] = color_int;
                                                                                }
                                                                                p1.y--;
                                                                                if ((e -= dx) >= 0) {
                                                                                        p1.x--;
                                                                                        e += dy;
                                                                                }
                                                                        } while (p1.y != p2.y);
                                                                }
                                                        }
                                                } else {
                                                        // Case dy = 0 and dx < 0
                                                        do {
                                                                if (!clipper || is_in_clipper(p1.x, p1.y, x_max, y_max, clipper)){
                                                                        first_pixel[p1.x + p1.y * size.width] = color_int;
                                                                }
                                                                p1.x--;
                                                        } while (p1.x != p2.x);
                                                }
                                        }
                                } else {
                                        dy = p2.y - p1.y;
                                        if (dy != 0) {
                                                // Vecteur vertical croissant
                                                if (dy > 0) {
                                                        // On trace la ligne
                                                        do {
                                                                if (!clipper || is_in_clipper(p1.x, p1.y, x_max, y_max, clipper)){
                                                                        first_pixel[p1.x + p1.y * size.width] = color_int;
                                                                }
                                                                ++p1.y;
                                                        } while (p1.y != p2.y);
                                                } else {
                                                        // On trace la ligne
                                                        do {
                                                                if (!clipper || is_in_clipper(p1.x, p1.y, x_max, y_max, clipper)){
                                                                        first_pixel[p1.x + p1.y * size.width] = color_int;
                                                                }
                                                                --p1.y;
                                                        } while (p1.y != p2.y);
                                                }
                                        }
                                }
                                // On trace le pixel final
                                if (!clipper || is_in_clipper(p1.x, p1.y, x_max, y_max, clipper)){
                                        first_pixel[p1.x + p1.y * size.width] = color_int;
                                }

                                // On passe au point suivant de la liste
                                first_point = first_point->next;
                        }
                }
        }
        hw_surface_unlock(surface);
}


/**
 * \brief	Draws a filled polygon.
 *
 * @param	surface 	Where to draw the polygon. The surface must be *locked* by
 *				\ref hw_surface_lock.
 * @param	first_point 	The head of a linked list of the points of the line. It is either
 *				NULL (i.e. draws nothing), or has more than 2 points. The last point
 *				is implicitly connected to the first point, i.e. polygons are
 *				closed, it is not necessary to repeat the first point.
 * @param	color		The color used to draw the polygon. The alpha channel is managed.
 * @param	clipper		If not NULL, the drawing is restricted within this rectangle.
 */
void			ei_draw_polygon		(ei_surface_t			surface,
                                                            const ei_linked_point_t*	first_point,
                                                            ei_color_t			color,
                                                            const ei_rect_t*		clipper) {
        hw_surface_lock(surface);
        uint32_t *first_pixel = (uint32_t*)hw_surface_get_buffer(surface);

        // Init clipper size
        uint32_t x_max;
        uint32_t y_max;
        if (clipper) {
                x_max = clipper->top_left.x + clipper->size.width;
                y_max = clipper->top_left.y + clipper->size.height;
        }

        // Init TC
        ei_size_t surface_size = hw_surface_get_size(surface);

        // First line where intersect polygon, at first : the bottom of the surface
        int y_first_line = surface_size.height;

        int size_tab = surface_size.height;
        struct side **tc = calloc(size_tab, sizeof(struct side*));
        for (uint32_t i = 0 ; i < size_tab ; ++i) {
                tc[i] = NULL;
        }

        // Fill TC
        while (first_point->next != NULL) {
                ei_point_t p1 = first_point->point;
                ei_point_t p2 = first_point->next->point;

                // Skip when horizontal side
                if (p1.y == p2.y) {
                        first_point = first_point->next;
                        continue;
                }

                // Allocate memory to store a new side structure and fill it
                struct side *new_side = malloc(sizeof(struct side));
                new_side->next = NULL;
                uint32_t indice_tc;

                if (p1.y > p2.y) {
                        new_side->ymax = p1.y;
                        new_side->xymin = p2.x;
                        new_side->dx = p1.x - p2.x;
                        new_side->dy = p1.y - p2.y;
                        indice_tc = p2.y;
                } else {
                        new_side->ymax = p2.y;
                        new_side->xymin = p1.x;
                        new_side->dx = p2.x - p1.x;
                        new_side->dy = p2.y - p1.y;
                        indice_tc = p1.y;

                        // Update of the first and last lines of intersect
                        y_first_line = p1.y < y_first_line ? p1.y : y_first_line;
                }
                new_side->error = 0;
                new_side->inv_slope = ceil((float)new_side->dx/ new_side->dy);

                // Insert the new side in tail of linked-list at indice_tc of TC
                insert(&tc[indice_tc], new_side);

                first_point = first_point->next;
        }

        // Init TCA
        struct side *tca = malloc(sizeof(struct side *));
        tca = NULL;

        // Init color
        uint32_t color_int = ei_map_rgba(surface, color);

        //Algorithm
        uint32_t index_list = 0;
        do {
                // Copy linked list tc[y] into tca
                move(&tc[y_first_line], &tca);

                // Delete side of tca where ymax = y
                delete(&tca, y_first_line);

                // Sort tca by attribute xymin
                insertion_sort(&tca);

                // Change pixels color to fill polygon

                // Get the first pixel which needs to be colored on the x-axis
                int32_t begin_fill = get_xymin(tca, index_list);
                ++index_list;
                while(begin_fill != -1) {
                        // Get the last pixel which needs to be colored on the x-axis
                        int32_t end_fill = get_xymin(tca, index_list);
                        ++index_list;

                        // If the end of fill is -1, that is mean it will be fill by the color to the edge
                        end_fill = end_fill != -1 ? end_fill : surface_size.width;

                        // Color all the concerned pixels
                        for (int32_t i = begin_fill ; i < end_fill ; ++i) {
                                uint32_t pixel_index = i + y_first_line * surface_size.width;
                                if (!clipper || is_in_clipper(i, y_first_line, x_max, y_max, clipper)) {
                                        first_pixel[pixel_index] = color_int;
                                }
                        }

                        // Update the next first pixel which needs to be colored on the x-axis
                        begin_fill = get_xymin(tca, index_list);
                        ++index_list;
                }

                // Change scanline
                ++y_first_line;

                // Update xymin for all nodes in tca
                set_xymin(&tca);
                index_list = 0;
        } while (tca != NULL || !is_empty(tc, size_tab));

        // Free memory
        free(tc);
        free(tca);

        hw_surface_unlock(surface);
}

/**
 * \brief	Draws text by calling \ref hw_text_create_surface.
 *
 * @param	surface 	Where to draw the text. The surface must be *locked* by
 *				\ref hw_surface_lock.
 * @param	where		Coordinates, in the surface, where to anchor the top-left corner of
 *				the rendered text.
 * @param	text		The string of the text. Can't be NULL.
 * @param	font		The font used to render the text. If NULL, the \ref ei_default_font
 *				is used.
 * @param	color		The text color. Can't be NULL. The alpha parameter is not used.
 * @param	clipper		If not NULL, the drawing is restricted within this rectangle.
 */
void ei_draw_text (ei_surface_t surface, const ei_point_t* where, const char* text, ei_font_t font,
                   ei_color_t color, const ei_rect_t* clipper) {

        // Lock the surface
        hw_surface_lock(surface);

        // Parameters of the rectangle where the copy will be done
        ei_size_t *destination_size = malloc(sizeof(ei_size_t));
        hw_text_compute_size(text, font, &(destination_size->width), &(destination_size->height));
        ei_rect_t destination_rect = {*where, *destination_size};

        // Surface which is copied
        if (font == NULL) font = ei_default_font;
        ei_surface_t text_surface = hw_text_create_surface(text, font, color);

        ////// I have to clip before passing text_surface as argument
        // Copy of the text
        ////// IL FAUT ABSOLUMENT QUE ALPHA SOIT EGALE A TRUE ICI
        ei_copy_surface(surface, &destination_rect, text_surface, NULL, EI_TRUE);

        // Doesn't forget to unlock/free the surfaces
        hw_surface_free(text_surface);
        hw_surface_unlock(surface);
}

/**
 * \brief	Fills the surface with the specified color.
 *
 * @param	surface		The surface to be filled. The surface must be *locked* by
 *				\ref hw_surface_lock.
 * @param	color		The color used to fill the surface. If NULL, it means that the
 *				caller want it painted black (opaque).
 * @param	clipper		If not NULL, the drawing is restricted within this rectangle.
 */
void			ei_fill			(ei_surface_t		surface,
                                                            const ei_color_t*	color,
                                                            const ei_rect_t*	clipper) {
        hw_surface_lock(surface);

        uint32_t *first_pixel = (uint32_t*)hw_surface_get_buffer(surface);
        ei_size_t size = hw_surface_get_size(surface);
        uint32_t color_int = ei_map_rgba(surface, *color);

        // Max coordinates of clipper
        uint32_t x_max;
        uint32_t y_max;
        if (clipper) {
                x_max = clipper->top_left.x + clipper->size.width;
                y_max = clipper->top_left.y + clipper->size.height;
        }

        // Put color on each pixels of the surface which are in the clipper
        for (int y = 0; y < size.height; y++){
                for(int x=0; x < size.width; x++){
                        if (!clipper || is_in_clipper(x, y, x_max, y_max, clipper)) {
                                *first_pixel++ = color_int;
                        } else {
                                first_pixel++;
                        }
                }
        }
        hw_surface_unlock(surface);
}

/**
 * \brief	Copies pixels from a source surface to a destination surface.
 *		The source and destination areas of the copy (either the entire surfaces, or
 *		subparts) must have the same size before considering clipping.
 *		Both surfaces must be *locked* by \ref hw_surface_lock.
 *
 * @param	destination	The surface on which to copy pixels.
 * @param	dst_rect	If NULL, the entire destination surface is used. If not NULL,
 *				defines the rectangle on the destination surface where to copy
 *				the pixels.
 * @param	source		The surface from which to copy pixels.
 * @param	src_rect	If NULL, the entire source surface is used. If not NULL, defines the
 *				rectangle on the source surface from which to copy the pixels.
 * @param	alpha		If true, the final pixels are a combination of source and
 *				destination pixels weighted by the source alpha channel and
 *				the transparency of the final pixels is set to opaque.
 *				If false, the final pixels are an exact copy of the source pixels,
 				including the alpha channel.
 *
 * @return			Returns 0 on success, 1 on failure (different sizes between source and destination).
 **/
int	ei_copy_surface	(ei_surface_t destination, const ei_rect_t* dst_rect, ei_surface_t source, const ei_rect_t* src_rect, ei_bool_t alpha){

        // Place of colors. Must be the same for destination and source surfaces.
        int red_place; int green_place;
        int blue_place; int alpha_place;
        hw_surface_get_channel_indices(destination, &red_place, &green_place, &blue_place, &alpha_place);
        alpha_place = 6 - (blue_place + red_place + green_place);

        // Source surface elements
        hw_surface_lock(source);
        uint8_t *src_pixel = hw_surface_get_buffer(source);
        ei_size_t src_size_surface = hw_surface_get_size(source);
        ei_size_t src_size_rect;
        uint32_t sum_src_next_line;

        /////// Le -1 est à surveiller dans le if ici et plus bas
        if (src_rect){
                src_size_rect = src_rect->size;
                src_pixel += (src_rect->top_left.y * src_size_surface.width + src_rect->top_left.x - 1) * 4;
                sum_src_next_line = 4 * (src_size_surface.width - src_size_rect.width);
        } else {
                src_size_rect = src_size_surface;
                sum_src_next_line = 0;
        }

        // Dest surface elements
        hw_surface_lock(destination);
        uint8_t *dst_pixel = hw_surface_get_buffer(destination);
        ei_size_t dst_size_surface = hw_surface_get_size(destination);
        ei_size_t dst_size_rect;
        uint32_t sum_dst_next_line;

        if (dst_rect){
                dst_size_rect = dst_rect->size;
                dst_pixel += (dst_rect->top_left.y * dst_size_surface.width + dst_rect->top_left.x - 1) * 4;
                sum_dst_next_line = 4 * (dst_size_surface.width - dst_size_rect.width);
        } else {
                dst_size_rect = dst_size_surface;
                sum_dst_next_line = 0;
        }

        // Verifies if different sizes
        if (dst_size_rect.width != src_size_rect.width || dst_size_rect.height != src_size_rect.height){
                hw_surface_unlock(source);
                hw_surface_unlock(destination);
                return 1;
        }

        // If alpha is true, makes a combination of source and destination pixels weighted by the source alpha channel
        if (alpha == EI_TRUE){
                for (uint32_t y = 0; y < src_size_rect.height; y++){
                        for (uint32_t x = 0; x < src_size_rect.width; x++){
                                dst_pixel[red_place] = (dst_pixel[red_place] * (255 - src_pixel[alpha_place]) + src_pixel[red_place] * src_pixel[alpha_place]) / 255;
                                dst_pixel[blue_place] = (dst_pixel[blue_place] * (255 - src_pixel[alpha_place]) + src_pixel[blue_place] * src_pixel[alpha_place]) / 255;
                                dst_pixel[green_place] = (dst_pixel[green_place] * (255 - src_pixel[alpha_place]) + src_pixel[green_place] * src_pixel[alpha_place]) / 255;
                                dst_pixel[alpha_place] = 255;
                                src_pixel += 4;
                                dst_pixel += 4;
                        }
                        dst_pixel += sum_dst_next_line;
                        src_pixel += sum_src_next_line;
                }

        // If alpha is false, the final pixels are an exact copy of the source pixels
        } else {
                for (uint32_t y = 0; y < src_size_rect.height; y++){
                        for (uint32_t x = 0; x < src_size_rect.width; x++){
                                dst_pixel[red_place] = src_pixel[red_place];
                                dst_pixel[green_place] = src_pixel[green_place];
                                dst_pixel[blue_place] = src_pixel[blue_place];
                                dst_pixel[alpha_place] = src_pixel[alpha_place];
                                src_pixel += 4;
                                dst_pixel += 4;
                        }
                        dst_pixel += sum_dst_next_line;
                        src_pixel += sum_src_next_line;
                }
        }

        // Doesn't forget to unlock surface
        hw_surface_unlock(source);
        hw_surface_unlock(destination);
        return 0;
}

/**
 * @brief   Draw button
 *
 * @param widget
 * @param surface
 * @param pick_surface
 * @param clipper
 */
void                    ei_draw_button          (ei_widget_t*	        widget,
                                                 ei_surface_t		surface,
                                                 ei_surface_t		pick_surface,
                                                 ei_rect_t*		clipper) {
        ei_button_t *button = (ei_button_t*) widget;

        // Get size and place parameters
        int width_button = button->widget.requested_size.width;
        int height_button = button->widget.requested_size.height;
        int place_x = button->widget.screen_location.top_left.x;
        int place_y = button->widget.screen_location.top_left.y;
        
        // Set size and place for the rectangle used to model the center of the button (all without border) 
        ei_size_t size_middle_button = {width_button - 2*(*button->border_width), height_button - 2*(*button->border_width)};
        ei_point_t place_middle_button = {place_x + (*button->border_width), place_y + (*button->border_width)};
        ei_rect_t middle_rect = ei_rect(place_middle_button, size_middle_button);
        
        // Rectangle used for border
        ei_rect_t border_rect = ei_rect(ei_point(place_x, place_y), ei_size(width_button, height_button));

        ei_color_t base_color = *button->color;

        // Lighten top color
        ei_color_t color_top = *button->color;
        color_top.red = color_top.red <= 205 ? color_top.red += 50 : 255;
        color_top.green = color_top.green <= 205 ? color_top.green += 50 : 255;
        color_top.blue = color_top.blue <= 205 ? color_top.blue += 50 : 255;

        // Darken bottom color
        ei_color_t color_bottom = *button->color;
        color_bottom.red = color_bottom.red >= 50 ? color_bottom.red -= 50 : 0;
        color_bottom.green = color_bottom.green >= 50 ? color_bottom.green -= 50 : 0;
        color_bottom.blue = color_top.blue >= 50 ? color_bottom.blue -= 50 : 0;

        // Set color param in the widget for the center part of the button
        ei_color_t color_middle = base_color;

        // Get all points for button modelization
        ei_linked_point_t *pts_top = rounded_frame(border_rect, *button->corner_radius, TOP);
        ei_linked_point_t *pts_middle = rounded_frame(middle_rect, *button->corner_radius, FULL);
        ei_linked_point_t *pts_bottom = rounded_frame(border_rect, *button->corner_radius, BOTTOM);

        // Display button
        ei_draw_polygon(surface, pts_top, color_top, clipper);
        ei_draw_polygon(surface, pts_bottom, color_bottom, clipper);
        ei_draw_polygon(surface, pts_middle, color_middle, clipper);

        // Display text
        ei_size_t *destination_size = malloc(sizeof(ei_size_t));
        hw_text_compute_size(*button->text, button->text_font, &(destination_size->width), &(destination_size->height));
        int place_text_x = (int) width_button - destination_size->width / 2;
        int place_text_y = (int) height_button - destination_size->height / 2;
        ei_point_t place_text = {place_text_x, place_text_y};
        ei_draw_text(surface, &place_text, *button->text , button->text_font, *button->text_color, clipper);

        // Free memory
        free(destination_size);
}

/**
 * @brief       Draw frame
 * @param widget
 * @param surface
 * @param pick_surface
 * @param clipper
 */
void ei_draw_frame (ei_widget_t* widget,
                    ei_surface_t		surface,
                    ei_surface_t		pick_surface,
                    ei_rect_t*		clipper) {
        ei_frame_t *frame = (ei_frame_t*) widget;

        ei_rect_t rect = ei_rect(frame->widget.screen_location.top_left, frame->widget.screen_location.size);
        ei_linked_point_t *pts_frame = rounded_frame(rect, 0, FULL);
        ei_draw_polygon(surface, pts_frame, *frame->color, clipper);

        if (*frame->border_width == 0) {

//                ei_point_t place_text = {frame->widget.screen_location.top_left.x, frame->widget.screen_location.top_left.y};
//                ei_draw_text(surface, &place_text, *frame->text, *frame->text_font, *frame->text_color, clipper);
        }
}

void ei_draw_top_level (ei_widget_t* widget,
                        ei_surface_t		surface,
                        ei_surface_t		pick_surface,
                        ei_rect_t*		clipper) {

}

