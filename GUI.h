
//
// This is a GUI support code to the chapters 12-16 of the book
// "Programming -- Principles and Practice Using C++" by Bjarne Stroustrup
//

#ifndef GUI_GUARD
#define GUI_GUARD

#include "Window.h"
#include "Graph.h"

namespace Graph_lib {

//------------------------------------------------------------------------------

    using Callback = std::function<void()>;

//------------------------------------------------------------------------------

    class WidgetPrivate;
    class Widget {
    // Widget is a handle to an Fl_widget - it is *not* an Fl_widget
    // We try to keep our interface classes at arm's length from FLTK
    public:
        Widget(Point xy, int w, int h, const string& s, Callback cb);

        virtual void move(int dx,int dy) { hide(); /*pw->position(loc.x+=dx, loc.y+=dy);*/ show(); }
        virtual void hide() { /*pw->hide();*/ }
        virtual void show() { /*pw->show();*/ }
        virtual void attach(Window&) = 0;

        Point loc;
        int width;
        int height;
        string label;
        Callback do_it;

        virtual ~Widget();

        WidgetPrivate& get_impl() const { return *impl; }
    protected:
        Window* own;    // every Widget belongs to a Window
        //Fl_Widget* pw;  // connection to the FLTK Widget
    private:
        Widget& operator=(const Widget&); // don't copy Widgets
        Widget(const Widget&);
        std::unique_ptr<WidgetPrivate> impl;
    };

//------------------------------------------------------------------------------

    struct Button : Widget {
        Button(Point xy, int w, int h, const string& label, Callback cb);

        void attach(Window&);
    };

//------------------------------------------------------------------------------

    struct In_box : Widget {
        In_box(Point xy, int w, int h, const string& s)
            :Widget(xy,w,h,s,0) { }
        int get_int();
        string get_string();

        void attach(Window& win);
    private:
        Window* window;
    };

//------------------------------------------------------------------------------

    struct Out_box : Widget {
        Out_box(Point xy, int w, int h, const string& s)
            :Widget(xy,w,h,s,0) { }
        void put(int);
        void put(const string&);

        void attach(Window& win);
    private:
        Window* window;
    };

//------------------------------------------------------------------------------

    struct Menu : Widget {
        enum Kind { horizontal, vertical };
        Menu(Point xy, int w, int h, Kind kk, const string& label);

        Vector_ref<Button> selection;
        Kind k;
        int offset;
        int attach(Button& b);      // Menu does not delete &b
        int attach(Button* p);      // Menu deletes p

        void show()                 // show all buttons
        {
            for (unsigned int i = 0; i<selection.size(); ++i)
                selection[i].show();
        }
        void hide()                 // hide all buttons
        {
            for (unsigned int i = 0; i<selection.size(); ++i) 
                selection[i].hide(); 
        }
        void move(int dx, int dy)   // move all buttons
        {
            for (unsigned int i = 0; i<selection.size(); ++i) 
                selection[i].move(dx,dy);
        }

        void attach(Window& win);    // attach all buttons
    };

//------------------------------------------------------------------------------

} // of namespace Graph_lib

#endif // GUI_GUARD
