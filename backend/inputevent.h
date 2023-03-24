#pragma once

#include <QObject>

class InputEvent : public QObject {
    Q_OBJECT

public:
    enum Key {
        Up,
        Down,
        Right,
        Left,
        Ok,
        Back,
    };

    Q_ENUM(Key)

    enum Type {
        Press, /* Press event, emitted after debounce */
        Release, /* Release event, emitted after debounce */
        Short, /* Short event, emitted after InputTypeRelease done withing INPUT_LONG_PRESS interval */
        Long, /* Long event, emitted after INPUT_LONG_PRESS interval, asynchronous to InputTypeRelease  */
        Repeat, /* Repeat event, emitted with INPUT_REPEAT_PRESS period after InputTypeLong event */
    };

    Q_ENUM(Type)
};
