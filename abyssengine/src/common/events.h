#ifndef ABYSS_EVENTS_H
#define ABYSS_EVENTS_H

#include <variant>

namespace AbyssEngine {

enum class eMouseButton { Left, Right, Middle };

struct MouseMoveEvent {
    int X;
    int Y;
};

struct MouseButtonEvent {
    eMouseButton Button;
    bool IsPressed;
};

using MouseEvent = std::variant<MouseMoveEvent, MouseButtonEvent>;

}

#endif // ABYSS_EVENTS_H
