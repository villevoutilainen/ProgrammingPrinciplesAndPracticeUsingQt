
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
        virtual void attach(Window&) {}

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

    };

//------------------------------------------------------------------------------

    struct In_box : Widget {
        In_box(Point xy, int w, int h, const string& s, Callback cb = {});
        int get_int();
        string get_string();
        int get_int_keep_open();
        string get_string_keep_open();
        void attach(Window& win) override;
        void dismiss();
        void hide() override;
        void show() override;
        void hide_buttons();
        void show_buttons();
        enum State {idle, accepted, rejected};
        State last_result();
        void clear_last_result();
        string last_string_value();
        int last_int_value();
        struct ResultData {
            State state = idle;
            string last_string;
            int last_int = 0;
        };

    private:
        ResultData result;
        bool waiting = false;
    };


//------------------------------------------------------------------------------

    struct Menu : Widget {
        enum Kind { horizontal, vertical };
        Menu(Point xy, int w, int h, Kind kk, const string& label);

        using Widget::attach;
        int attach(Button& b);      // Menu does not delete &b
        int attach(unique_ptr<Button> p);      // Menu destroys p

        void show()                 // show all buttons
        {
            for (auto&& x : selection)
                x->show();
        }
        void hide()                 // hide all buttons
        {
            for (auto&& x : selection)
                x->hide();
        }
        void move(int dx, int dy)   // move all buttons
        {
            for (auto&& x : selection)
                x->move(dx,dy);
        }

    private:
        Vector_ref<Button> selection;
        Kind k;
        int offset;
        void layoutButtons(Button& b);
        void layoutMenu();
    };

//------------------------------------------------------------------------------

} // of namespace Graph_lib

#endif // GUI_GUARD
