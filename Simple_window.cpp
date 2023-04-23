
//
// This is a GUI support code to the chapters 12-16 of the book
// "Programming -- Principles and Practice Using C++" by Bjarne Stroustrup
//

#include "PPP/Simple_window.h"
#include <QEventLoop>
//------------------------------------------------------------------------------

Simple_window::Simple_window(Point xy, int w, int h, const string& title) :
    Window(xy,w,h,title),
    next_button(Point(w-70,0), 70, 20, "Next", []{})
{
    attach(next_button);
}

//------------------------------------------------------------------------------

void Simple_window::wait_for_button()
// modified event loop:
// handle all events (as per default), quit when button_pushed becomes true
// this allows graphics without control inversion
{
    QEventLoop nested_loop;
    next_button.do_it = [&] {nested_loop.exit();};
    nested_loop.exec();
    next_button.do_it = []{};
}


//------------------------------------------------------------------------------
