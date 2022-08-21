#include "input.h"
#include <string.h>

typedef struct {
    bool button_sel;
    bool dir_sel;
    inputs controller;
} input_context;

static input_context ctx = {0};

bool input_button_sel()
{
    return ctx.button_sel;
}

bool input_dir_sel()
{
    return ctx.dir_sel;
}

void input_set_sel(u8 val)
{
    ctx.button_sel = val & 0x20;
    ctx.dir_sel = val & 0x10;
}

inputs *input_get_state()
{
    return &ctx.controller;
}

u8 input_get_output()
{
    u8 output = 0xCF;

    if (!input_button_sel()) {
        if (input_get_state()->start) {
            output &= ~(1 << 3);
        } else if (input_get_state()->select) {
            output &= ~(1 << 2);
        } else if (input_get_state()->a) {
            output &= ~(1 << 0);
        } else if (input_get_state()->b) {
            output &= ~(1 << 1);
        }
    }

    if (!input_dir_sel()) {
        if (input_get_state()->left) {
            output &= ~(1 << 1);
        } else if (input_get_state()->right) {
            output &= ~(1 << 0);
        } else if (input_get_state()->up) {
            output &= ~(1 << 2);
        } else if (input_get_state()->down) {
            output &= ~(1 << 3);
        }
    }

    return output;
}
