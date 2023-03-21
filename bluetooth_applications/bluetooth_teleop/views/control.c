
#include <furi_hal.h>
#include <furi.h>
#include "control.h"
#include "bluetooth_teleop_icons.h"

struct Control {
    View* view;
};

typedef struct {
    bool left_pressed;
    bool right_pressed;
    bool up_pressed;
    bool down_pressed;
    bool ok_pressed;
    uint8_t cmd;
    uint8_t speed;
} ControlModel;

static void control_draw_callback(Canvas* canvas, void* ctx) {
    furi_assert(ctx);
    ControlModel* model = ctx;

    canvas_draw_icon(canvas, 0, 64, &I_circles_64x64);

    canvas_draw_icon(
        canvas,
        3,
        89,
        model->left_pressed ? &I_arrow_left_filled_14x15 : &I_arrow_left_empty_14x15);
    canvas_draw_icon(
        canvas,
        46,
        89,
        model->right_pressed ? &I_arrow_right_filled_14x15 : &I_arrow_right_empty_14x15);
    canvas_draw_icon(
        canvas, 25, 67, model->up_pressed ? &I_arrow_up_filled_14x15 : &I_arrow_up_empty_14x15);
    canvas_draw_icon(
        canvas,
        25,
        110,
        model->down_pressed ? &I_arrow_down_filled_14x15 : &I_arrow_down_empty_14x15);
    canvas_draw_icon(canvas, 25, 89, model->ok_pressed ? &I_ok_filled_14x14 : &I_ok_empty_14x14);

    char speed_str[18];
    canvas_set_font(canvas, FontSecondary);
    snprintf(speed_str, 18, "Speed: %u", model->speed + 1);
    canvas_draw_str_aligned(canvas, 32, 10, AlignCenter, AlignTop, speed_str);
}

static void control_process_press(Control* control, InputEvent* event) {
    with_view_model(
        control->view,
        ControlModel * model,
        {
            if(event->key == InputKeyLeft) {
                model->left_pressed = true;
                model->cmd |= 1 << 7;
            } else if(event->key == InputKeyRight) {
                model->right_pressed = true;
                model->cmd |= 1 << 6;
            } else if(event->key == InputKeyUp) {
                model->up_pressed = true;
                model->cmd |= 1 << 4;
            } else if(event->key == InputKeyDown) {
                model->down_pressed = true;
                model->cmd |= 1 << 5;
            } else if(event->key == InputKeyOk) {
                model->ok_pressed = true;
                model->speed = (model->speed + 1) % 5;
                model->cmd = (model->speed + 1) & 0x0F;
            }
        },
        true);
}

static void control_process_release(Control* control, InputEvent* event) {
    with_view_model(
        control->view,
        ControlModel * model,
        {
            if(event->key == InputKeyLeft) {
                model->left_pressed = false;
                model->cmd &= ~(1 << 7);
            } else if(event->key == InputKeyRight) {
                model->right_pressed = false;
                model->cmd &= ~(1 << 6);
            } else if(event->key == InputKeyUp) {
                model->up_pressed = false;
                model->cmd &= ~(1 << 4);
            } else if(event->key == InputKeyDown) {
                model->down_pressed = false;
                model->cmd &= ~(1 << 5);
            } else if(event->key == InputKeyOk) {
                model->ok_pressed = false;
            }
        },
        true);
}

uint8_t control_get_cmd(Control* control) {
    furi_assert(control);
    uint8_t data;
    with_view_model(
        control->view, ControlModel * model, { data = model->cmd; }, true);
    return data;
}

static bool control_input_callback(InputEvent* event, void* ctx) {
    furi_assert(ctx);
    Control* control = ctx;

    if(event->type == InputTypePress) {
        control_process_press(control, event);
    } else if(event->type == InputTypeRelease) {
        control_process_release(control, event);
    }

    return 0;
}

Control* control_alloc() {
    Control* control = malloc(sizeof(Control));

    control->view = view_alloc();
    view_set_context(control->view, control);
    view_allocate_model(control->view, ViewModelTypeLocking, sizeof(ControlModel));

    view_set_orientation(control->view, ViewOrientationVertical);
    view_set_draw_callback(control->view, control_draw_callback);
    view_set_input_callback(control->view, control_input_callback);

    with_view_model(
        control->view,
        ControlModel * model,
        {
            model->cmd = (model->speed + 1) & 0x0F;
            model->speed = 0;
        },
        true);

    return control;
}

void control_free(Control* control) {
    furi_assert(control);
    view_free(control->view);
    free(control);
}

View* control_get_view(Control* control) {
    furi_assert(control);
    return control->view;
}
