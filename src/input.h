#pragma once

#include "include.h"

typedef struct {
    bool start;
    bool select;
    bool a;
    bool b;
    bool up;
    bool down;
    bool left;
    bool right;
} inputs;

void input_init();
bool input_button_sel();
bool input_dir_sel();
void input_set_sel(u8 val);
inputs *input_get_state();
u8 input_get_output();
