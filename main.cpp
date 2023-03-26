#include "Window.h"
#include "Graph.h"

using namespace Graph_lib;
int main(int argc, char *argv[])
{
    Application app;
    Window w1(Point(20, 20), 400, 200, "Shazam!");
    Window w2(Point(40, 40), 400, 400, "Woohoo!");
    Rectangle r(Point(10, 10), Point(100, 100));
    r.set_style(Line_style::dashdot);
    r.set_color(Color::red);
    Line line1(Point(10, 110), Point(100, 220));
    line1.set_color(Color::blue);
    line1.set_style(Line_style::dot);
    Line line2(Point(10, 220), Point(100, 110));
    line2.set_color(Color::dark_green);
    w2.attach(r);
    w2.attach(line1);
    w2.attach(line2);
    app.gui_main();
}
