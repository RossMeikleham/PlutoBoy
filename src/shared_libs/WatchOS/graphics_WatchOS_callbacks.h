#ifndef GRAPHICS_WATCHOS_CALLBACKS_H
#define GRAPHICS_WATCHOS_CALLBACKS_H

struct graphics_callbacks {
    void (* _Nonnull display_screen)(const void* _Nonnull slf, uint32_t * _Nonnull pixels);
};


void setup_graphics_callbacks(const void* _Nonnull slf, const struct graphics_callbacks * _Nonnull graphics_callbacks);


#endif
