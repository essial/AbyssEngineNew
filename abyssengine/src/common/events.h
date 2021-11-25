#ifndef ABYSS_EVENTS_H
#define ABYSS_EVENTS_H

namespace AbyssEngine {

enum class eMouseButton { Left, Right, Middle };
enum class eMouseEventType { Move, Button };

struct MouseMoveEvent {
    int X;
    int Y;
};

struct MouseButtonEvent {
    eMouseButton Button;
    bool IsPressed;
};

struct MouseEventInfo {
    eMouseEventType EventType;
    union Event {
        MouseMoveEvent MoveEvent;
        MouseButtonEvent ButtonEvent;
    };
};

}

#endif // ABYSS_EVENTS_H
