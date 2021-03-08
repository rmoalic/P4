
#ifndef include_layout
#define include_layout 1

#include <stdbool.h>
#include <assert.h>
#include <SDL.h>
#include "list.h"

typedef struct margin_box {
    int top;
    int bottom;
    int left;
    int right;
} Margin;

struct size {
    int width;
    int height;
};

typedef struct widget {
    SDL_Texture* texture;
    struct size texture_size;
    struct size minimum_size;
    bool keep_ratio;
    double ratio;
    void** custom;
    size_t custom_s;
    
    void (*update)(SDL_Renderer*, struct widget*);
    void (*capture_mouse_input)(SDL_Event, struct widget*);
    void (*capture_keyboard_input)(SDL_Event, struct widget*);
} Widget;

typedef struct flow_layout {
    Margin margin;
    list widgets;
} FlowLayout;

typedef enum container_type {
    WIDGET,
    FLOW_LAYOUT,
} ContainerType;

typedef struct container {
    ContainerType type;
    union {
        Widget* widget;
        FlowLayout* flow_layout;
    };
} Container;

Widget* init_widget(SDL_Texture* texture, struct size t_size, bool keep_ratio);
FlowLayout* init_flow_layout();
void draw_container(Container c, SDL_Renderer* ren, int x, int y);
void layout_add_widget(FlowLayout* layout, Widget* w);

#endif
