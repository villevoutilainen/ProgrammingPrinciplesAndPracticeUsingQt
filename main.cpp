#include "Window.h"
#include "Graph.h"

using namespace Graph_lib;
int main(int argc, char *argv[])
{
    Application app;
    Window w1(Point(20, 20), 600, 400, "Shazam!");
    Window w2(Point(40, 40), 400, 400, "Woohoo!");
    Window w3(Point(40, 40), 400, 400, "More window!");
    Rectangle r(Point(10, 10), Point(100, 100));
    r.set_style(Line_style::dashdot);
    r.set_color(Color::red);
    r.set_fill_color(Color::yellow);
    Line line1(Point(10, 110), Point(100, 220));
    line1.set_color(Color::blue);
    line1.set_style(Line_style::dot);
    Line line2(Point(10, 220), Point(100, 110));
    line2.set_color(Color::dark_green);
    Open_polyline open_poly;
    open_poly.add(Point(110, 110));
    open_poly.add(Point(115, 115));
    open_poly.add(Point(115, 120));
    open_poly.add(Point(120, 120));
    open_poly.add(Point(120, 125));
    open_poly.add(Point(125, 125));
    Closed_polyline closed_poly;
    closed_poly.add(Point(130, 130));
    closed_poly.add(Point(130, 150));
    closed_poly.add(Point(150, 150));
    closed_poly.add(Point(150, 180));
    closed_poly.set_fill_color(Color::cyan);
    Polygon poly;
    poly.add(Point(130, 200));
    poly.add(Point(130, 300));
    poly.add(Point(150, 350));
    poly.add(Point(180, 300));
    Text text(Point(150, 20), "Well hello there!");
    text.set_font_size(20);
    text.set_font(Font::symbol);
    Function fun(std::sin, 0, 100, Point(150, 60), 1000);
    Circle cir(Point(250, 180), 40);
    cir.set_fill_color(Color::dark_red);
    Ellipse ell(Point(320, 260), 80, 40);
    ell.set_fill_color(Color::dark_blue);
    Image img(Point(0, 0), "/home/ville/Downloads/seance.jpg");
    img.set_mask(Point(0, 0), 200, 200);
    Lines lines{Point(0, 0), Point(20, 20), Point(30, 30), Point(40, 10)};
    w2.attach(r);
    w2.attach(line1);
    w2.attach(line2);
    w2.attach(open_poly);
    w2.attach(closed_poly);
    w2.attach(poly);
    w2.attach(text);
    w2.attach(fun);
    w2.attach(cir);
    w2.attach(ell);
    w1.attach(img);
    w3.attach(lines);
    app.gui_main();
}
