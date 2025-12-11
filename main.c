#include "rendering.h"

int main(void)
{
    struct window win;

    if (window_create(&win, 600, 600) != 0)
        return -1;

    window_set_color(&win, 0.1f, 0.2f, 0.3f);

    return window_exec(&win);
}
