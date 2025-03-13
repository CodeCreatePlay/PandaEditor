#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <keyboardButton.h>

enum GameViewStyle {
    CENTER,
    BOTTOM_LEFT,
    BOTTOM_RIGHT,
    TOP_LEFT,
    TOP_RIGHT,
};

constexpr int GAME_RAW_EVT_IDX  = 0;

constexpr int MAIN_TASK_SORT    = 0;
constexpr int MARQUEE_TASK_SORT = 1;

constexpr int ENGINE_DR_3D_SORT = 0;
constexpr int ENGINE_DR_2D_SORT = 10;
constexpr int GAME_DR_3D_SORT   = 20;
constexpr int GAME_DR_2D_SORT   = 30;

const int ALT_KEY_IDX  = KeyboardButton::alt().get_index();
const int CTRL_KEY_IDX = KeyboardButton::control().get_index();

#endif // CONSTANTS_H
