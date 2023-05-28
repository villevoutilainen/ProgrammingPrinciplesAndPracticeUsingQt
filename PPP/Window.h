#ifndef WINDOW_GUARD
#define WINDOW_GUARD 1

#include <PPP/std_lib_facilities.h>

#include <PPP/Point.h>
//#include "GUI.h"

namespace Graph_lib {

class Shape;	// "forward declare" Shape
struct Color;
struct Line_style;
class Font;
struct Image;

class Widget;

class PainterPrivate;
class Painter
{
public:
    Painter(std::unique_ptr<PainterPrivate>&& pp);
    ~Painter();
    Painter(const Painter&) = delete;
    Painter& operator=(const Painter&) = delete;

    void draw_rectangle(const Point& p1, int w, int h);
    void draw_line(const Point& p1, const Point& p2);
    void draw_polygon(const Shape& s);
    void draw_text(const Point& p1, const std::string text);
    void draw_ellipse(const Point& p1, int r, int r2);
    void draw_image(const Point& p1, const Image& img);
    void draw_image(const Point& p1, const Point& p2, int w, int h, const Image& img);
    void set_font_size(int s);
    void set_font(Font f);
private:
    void save();
    void restore();
    void set_color(Color color);
    void set_line_style(Line_style style);
    void set_fill_color(Color color);
    friend class Shape;
private:
    std::unique_ptr<PainterPrivate> impl;
};

class ApplicationPrivate;
class Application
{
public:
    Application();
    ~Application();
    void gui_main();
    void quit();
private:
    std::unique_ptr<ApplicationPrivate> impl;
};

class WindowPrivate;
class Window {
public: 
	Window(int w, int h, const string& title );			// let the system pick the location
	Window(Point xy, int w, int h, const string& title );	// top left corner in xy
    virtual ~Window();

	int x_max() const { return w; }
	int y_max() const { return h; }

    void resize(int ww, int hh) { w=ww, h=hh; }

    void set_label(const string& s);

	void attach(Shape& s);
	void attach(Widget& w);

	void detach(Shape& s);	// remove s from shapes 
	void detach(Widget& w);	// remove w from window (deactivate callbacks)

	void put_on_top(Shape& p);	// put p on top of other shapes

    WindowPrivate& get_impl() const;
	void draw();
    void close();

private:
      int w,h;					// window size
      unique_ptr<WindowPrivate> impl;
      void init();
};

int gui_main();	// invoke GUI library's main event loop

inline int x_max() { return 1024; }	// width of screen in pixels
inline int y_max() { return 768; }	// height of screen in pixels

}
#endif
