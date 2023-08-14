
//
// This is a GUI support code to the chapters 12-16 of the book
// "Programming -- Principles and Practice Using C++" by Bjarne Stroustrup
//

#ifndef GUI_GUARD
#define GUI_GUARD

#include <PPP/Window.h>
#include <PPP/Graph.h>

namespace Graph_lib {

//------------------------------------------------------------------------------

    using Callback = std::function<void()>;

//------------------------------------------------------------------------------

    struct WidgetPrivate;
    class Widget {
    // Widget is a handle to a QWidget - it is *not* a QWidget
    // We keep our interface classes at arm's length from Qt
    public:
        Widget(Point xy, int w, int h, const string& s, Callback cb);

        virtual void move(int dx,int dy);
        virtual void hide();
        virtual void show();
        virtual void attach(Window&) = 0;

        Point loc;
        int width;
        int height;
        string label;
        Callback do_it;

        virtual ~Widget();

        WidgetPrivate& get_impl() const { return *impl; }
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
        In_box(Point xy, int w, int h, const string& s);
        int get_int();
        string get_string();
        int get_int_keep_open();
        string get_string_keep_open();
        void attach(Window& win);
        void close();
        enum State {idle, accepted, rejected};
        State last_result() {return state;}
    private:
        Window* window;
        State state = idle;
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
        int attach(Button* p);      // Menu does not delete p

        void show()                 // show all buttons
        {
            for (int i = 0; i<selection.size(); ++i)
                selection[i].show();
        }
        void hide()                 // hide all buttons
        {
            for (int i = 0; i<selection.size(); ++i)
                selection[i].hide(); 
        }
        void move(int dx, int dy)   // move all buttons
        {
            for (int i = 0; i<selection.size(); ++i)
                selection[i].move(dx,dy);
        }

        void attach(Window& win);    // attach all buttons
    private:
        void layoutButtons(Button& b);
        void layoutMenu();
    };

//------------------------------------------------------------------------------

} // of namespace Graph_lib

#endif // GUI_GUARD
