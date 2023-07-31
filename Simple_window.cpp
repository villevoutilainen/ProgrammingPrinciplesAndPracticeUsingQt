
//
// This is a GUI support code to the chapters 12-16 of the book
// "Programming -- Principles and Practice Using C++" by Bjarne Stroustrup
//

#include "PPP/Simple_window.h"
#include <QEventLoop>
#include <QTimer>
#include <PPP/GUI_private.h>
//------------------------------------------------------------------------------

struct Simple_windowPrivate
{
    QEventLoop nested_loop;
    QTimer timer{&nested_loop};
};

Simple_window::Simple_window(Point xy, int w, int h, const string& title) :
    Window(xy,w,h,title),
    next_button(Point{w-70,0}, 70, 20, "Next", []{}),
    impl(std::make_unique<Simple_windowPrivate>())
{
    attach(next_button);
    next_button.do_it = [&] {impl->nested_loop.exit();};
}

Simple_window::~Simple_window() {}

//------------------------------------------------------------------------------

void Simple_window::wait_for_button()
// modified event loop:
// handle all events (as per default), quit when button_pushed becomes true
// this allows graphics without control inversion
{
    impl->nested_loop.exec();
}


void Simple_window::windowClosed()
{
    // We need to do this with a QObject that is owned by the nested
    // loop, that's why we create a timer that is owned by it.
    // Then, we can tell that timer to exit the loop. Telling it to
    // exit from outside won't do it, but this will.
    QObject::connect(&impl->timer, &QTimer::timeout,
                     [this] {impl->nested_loop.quit();});
    impl->timer.start(0);
}

//------------------------------------------------------------------------------
