
//
// This is a GUI support code to the chapters 12-16 of the book
// "Programming -- Principles and Practice Using C++" by Bjarne Stroustrup
//

#ifndef GUI_PRIVATE_GUARD
#define GUI_PRIVATE_GUARD

#include <PPP/Graph.h>

#include <QWidget>

namespace Graph_lib {

    struct WidgetPrivate
    {
        QWidget* widget;
    };

    class WindowPrivate : public QWidget
    {
    public:
        void attach(Shape& s)
        {
            shapes.push_back(&s);
        }
        void detach(Shape& s)
        {
            for (unsigned int i = shapes.size(); 0<i; --i)	// guess last attached will be first released
                if (shapes[i-1]==&s)
                    shapes.erase(shapes.begin()+(i-1));//&shapes[i-1]);
        }
    private:
        void paintEvent(QPaintEvent *event) override;
        vector<Shape*> shapes;	// shapes attached to window
        int w,h;					// window size
    };

} // of namespace Graph_lib

#endif // GUI_GUARD
