#include <stdbool.h>
#include <assert.h>
#include <SDL.h>
#include "list.h"
#include "layout.h"

Widget* init_widget(SDL_Texture* texture, struct size t_size, bool keep_ratio) {
    Widget tmp = {.texture = texture, 
                  .texture_size = t_size, 
                  .ratio = t_size.width / (t_size.height * 1.0),
                  .keep_ratio = keep_ratio};
    Widget* widget = malloc(sizeof(Widget));
    memcpy(widget, &tmp, sizeof(Widget));
    
    return widget;
}

FlowLayout* init_flow_layout() {
    FlowLayout* ret = malloc(sizeof(FlowLayout));
    
    ret->margin.bottom = 0;
    ret->margin.left = 0;
    ret->margin.right = 0;
    ret->margin.top = 0;
    list_init(&ret->widgets);
    
    return ret;
}

void layout_add_widget(FlowLayout* layout, Widget* w) {
    list_append(&layout->widgets, w, sizeof(Widget));
}

void draw_widget(Widget* widget, SDL_Renderer* ren, int x, int y) {
    SDL_Rect r;
    
    SDL_SetRenderTarget(ren, widget->texture);
    SDL_RenderClear(ren);
    
    widget->update(ren, widget);

    r.x = x;
    r.y = y;
    r.w = widget->texture_size.width;
    r.h = widget->texture_size.height;

    SDL_SetRenderTarget(ren, NULL);
    SDL_RenderCopy(ren, widget->texture, NULL, &r);
}

void draw_layout(FlowLayout* layout, SDL_Renderer* ren, int x, int y) {
    struct list_element* curr = layout->widgets;
    int cx = x;
    int cy = y;
    
    while (curr != NULL) {
        Widget* w = (Widget*)curr->element;
        draw_widget(w, ren, cx, cy);
        cy = w->texture_size.height;

        curr = curr->next;
    }
}

void draw_container(Container c, SDL_Renderer* ren, int x, int y) {
    switch (c.type) {
        case WIDGET: {
            draw_widget(c.widget, ren, x, y);
        } break;
        case FLOW_LAYOUT: {
            draw_layout(c.flow_layout, ren, x, y);
        } break;
    }
}
