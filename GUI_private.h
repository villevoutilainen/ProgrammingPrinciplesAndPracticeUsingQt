
//
// This is a GUI support code to the chapters 12-16 of the book
// "Programming -- Principles and Practice Using C++" by Bjarne Stroustrup
//

#ifndef GUI_PRIVATE_GUARD
#define GUI_PRIVATE_GUARD

#include "Window.h"
#include "Graph.h"

#include <QWidget>

namespace Graph_lib {

    struct WidgetPrivate
    {
        QWidget* widget;
    };

} // of namespace Graph_lib

#endif // GUI_GUARD
