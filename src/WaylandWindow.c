//printf will be removed after stable work of code

#include <wayland-client.h>
#include <wlr-foreign-toplevel-management-unstable-v1-client-protocol.h>
#include <stdio.h>

struct State {
    struct zwlr_foreign_toplevel_manager_v1 *toplevel_manager;
};

static void handle_toplevel(void *data,
    struct zwlr_foreign_toplevel_handle_v1 *toplevel) {
    printf("New toplevel window found\n");
}

static void handle_finished(void *data,
    struct zwlr_foreign_toplevel_handle_v1 *toplevel) {
    printf("Toplevel window closed\n");
}

static const struct zwlr_foreign_toplevel_handle_v1_listener toplevel_listener = {
    .handle = handle_toplevel,
    .closed = handle_finished,
};

static void registry_global(void *data, struct wl_registry *registry,
    uint32_t name, const char *interface, uint32_t version) {
    struct State *state = data;

    if (strcmp(interface, zwlr_foreign_toplevel_manager_v1_interface.name) == 0) {
        state->toplevel_manager = wl_registry_bind(registry, name,
            &zwlr_foreign_toplevel_manager_v1_interface, version);
    }
}

int main() {
    struct wl_display *display = wl_display_connect(NULL);
    struct wl_registry *registry = wl_display_get_registry(display);
    
    struct State state = {0};
    static struct wl_registry_listener registry_listener = {
        .global = registry_global,
    };
    
    wl_registry_add_listener(registry, &registry_listener, &state);
    wl_display_roundtrip(display);

    if (!state.toplevel_manager) {
        fprintf(stderr, "Compositor doesn't support foreign toplevel\n");
        return 1;
    }

    struct zwlr_foreign_toplevel_manager_v1 *manager = state.toplevel_manager;
    // Здесь должна быть реализация работы с окнами через менеджер
    
    wl_display_roundtrip(display);
    wl_display_disconnect(display);
    return 0;
}
