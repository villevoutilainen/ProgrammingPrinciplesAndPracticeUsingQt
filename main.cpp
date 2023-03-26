#include "Window.h"
#include "Graph.h"

using namespace Graph_lib;
int main(int argc, char *argv[])
{
    Application app;
    Window w1(Point(20, 20), 400, 200, "Shazam!");
    Window w2(Point(40, 40), 400, 200, "Woohoo!");
    Rectangle r(Point(10, 10), Point(100, 100));
    w2.attach(r);
    app.gui_main();
}
