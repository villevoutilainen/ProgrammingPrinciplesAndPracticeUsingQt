
#ifndef GRAPH_GUARD
#define GRAPH_GUARD 1

#include <PPP/Point.h>
#include <PPP/std_lib_facilities.h>


namespace Graph_lib {
// defense against ill-behaved Linux macros:
#undef major
#undef minor

struct Color {
	enum Color_type {
        red, blue, green,
        yellow, white, black,
        magenta, cyan, dark_red,
        dark_green, dark_yellow, dark_blue,
        dark_magenta, dark_cyan,
        palette_index,
        rgb
	};
	enum Transparency { invisible = 0, visible=255 };

    Color(Color_type cc) :c(cc), ct(cc), v(visible) { }
    Color(Color_type cc, Transparency vv) :c(cc), ct(cc), v((unsigned char)vv) { }
    Color(int cc) :c(cc), ct(Color_type::palette_index), v(visible) { }
    Color(Transparency vv) :c(), ct(Color_type::black), v((unsigned char)vv) { }
    Color(int redc, int greenc, int bluec) : c(), ct(Color_type::rgb),
        r(redc), g(greenc), b(bluec), v(visible) {}

    int as_int() const { return c; }
    int red_component() {return r;}
    int green_component() {return g;}
    int blue_component() {return b;}
    Color_type type() {return ct;}
    char visibility() const { return v; }
    void set_visibility(Transparency vv) { v=(unsigned char)vv; }
private:
    int c;
    Color_type ct;
    int r, g, b;
    unsigned char v;	// 0 or 1 for now
};

inline bool operator<(Color a, Color b)
{
    if (a.type() != Color::palette_index || b.type() != Color::palette_index)
        return a.type() < b.type();
    return a.as_int() < b.as_int();
}

struct Line_style {
	enum Line_style_type {
        solid,				// -------
        dash,				// - - - -
        dot,					// .......
        dashdot,			// - . - .
        dashdotdot,	// -..-..
        none
    };
    Line_style() {}
    Line_style(Line_style_type ss) :s(ss) { }
	Line_style(Line_style_type lst, int ww) :s(lst), w(ww) { }

	int width() const { return w; }
	int style() const { return s; }
private:
    int s = Line_style::solid;
    int w = 1;
};

inline bool operator<(Line_style a, Line_style b)
{
    return a.style() < b.style();
}

class Font {
public:
	enum Font_type {
        helvetica,
        helvetica_bold,
        helvetica_italic,
        helvetica_bold_italic,
        courier,
        courier_bold,
        courier_italic,
        courier_bold_italic,
        times,
        times_bold,
        times_italic,
        times_bold_italic,
        symbol,
        screen,
        screen_bold,
        zapf_dingbats
	};

	Font(Font_type ff) :f(ff) { }

	int as_int() const { return f; }
private:
    int f;
};

inline bool operator<(Font a, Font b)
{
    return a.as_int() < b.as_int();
}

template<class T> class Vector_ref {
    vector<T*> v;
    vector<T*> owned;
public:
	Vector_ref() {}

	Vector_ref(T* a, T* b=0, T* c=0, T* d=0)
	{
			if (a) push_back(a);
			if (b) push_back(b);
			if (c) push_back(c);
			if (d) push_back(d);
	}

    ~Vector_ref() { for (unsigned int i=0; i<owned.size(); ++i) delete owned[i]; }

	void push_back(T& s) { v.push_back(&s); }
	void push_back(T* p) { v.push_back(p); owned.push_back(p); }

	// ???void erase(???)

	T& operator[](int i) { return *v[i]; }
	const T& operator[](int i) const { return *v[i]; }
    int size() const { return (int)v.size(); }
};

typedef double Fct(double);


class Painter;
class Window;
class Shape  {	// deals with color and style, and holds sequence of lines
protected:
	Shape() { }
    Shape(std::initializer_list<Point> lst);  // add() the Points to this Shape

//	Shape() : lcolor(fl_color()),
//		ls(0),
//		fcolor(Color::invisible) { }

    void add(Point p){ points.push_back(p); redraw();}
    void set_point(int i, Point p) { points[i] = p; redraw();}
public:
    void draw(Painter& painter) const;					// deal with color and draw_lines
protected:
    virtual void draw_lines(Painter& painter) const;	// simply draw the appropriate lines
public:
	virtual void move(int dx, int dy);	// move the shape +=dx and +=dy

    void set_color(Color col) { lcolor = col; redraw();}
	Color color() const { return lcolor; }

    void set_style(Line_style sty) { ls = sty; redraw();}
	Line_style style() const { return ls; }

    void set_fill_color(Color col) { fcolor = col; redraw();}
	Color fill_color() const { return fcolor; }

	Point point(int i) const { return points[i]; }
	int number_of_points() const { return int(points.size()); }
    void set_window(Window* win) { parent_window = win;}
	virtual ~Shape() { }
	/*
	struct Window* attached;
	Shape(const Shape& a)
		:attached(a.attached), points(a.points), line_color(a.line_color), ls(a.ls)
	{
		if (a.attached)error("attempt to copy attached shape");
	}
	*/
	Shape(const Shape&) = delete;
	Shape& operator=(const Shape&) = delete;
protected:
    void redraw();
    Window* parent_window = nullptr;
private:
    vector<Point> points;	// not used by all shapes
    Color lcolor {Color::black};
    Line_style ls;
	Color fcolor {Color::invisible};

//	Shape(const Shape&);
//	Shape& operator=(const Shape&);
};

struct Function : Shape {
	// the function parameters are not stored
	Function(Fct f, double r1, double r2, Point orig, int count = 100, double xscale = 25, double yscale = 25);
	//Function(Point orig, Fct f, double r1, double r2, int count, double xscale = 1, double yscale = 1);	
};

struct Fill {
	Fill() :no_fill(true), fcolor(0) { }
	Fill(Color c) :no_fill(false), fcolor(c) { }

    void set_fill_color(Color col) { fcolor = col; }
	Color fill_color() { return fcolor; }
protected:
	bool no_fill;
	Color fcolor;
};

struct Line : Shape {
	Line(Point p1, Point p2) { add(p1); add(p2); }
};

struct Rectangle : Shape {

	Rectangle(Point xy, int ww, int hh) :w{ ww }, h{ hh }
	{
		if (h<=0 || w<=0) error("Bad rectangle: non-positive side");
		add(xy);
	}
	Rectangle(Point x, Point y) :w{ y.x - x.x }, h{ y.y - x.y }
	{
		if (h<=0 || w<=0) error("Bad rectangle: first point is not top left");
		add(x);
	}
    void draw_lines(Painter& painter) const override;

//	void set_fill_color(Color col) { fcolor = col; }
//	Color fill_color() { return fcolor; }

	int height() const { return h; }
	int width() const { return w; }
private:
    int w;			// width
    int h;			// height
//	Color fcolor;	// fill color; 0 means "no fill"
};

bool intersect(Point p1, Point p2, Point p3, Point p4);


struct Open_polyline : Shape {	// open sequence of lines
    using Shape::Shape;
    Open_polyline(std::initializer_list<Point> p) : Shape(p) {}
    void add(Point p) { Shape::add(p); redraw();}
    void draw_lines(Painter& painter) const override;
};

struct Closed_polyline : Open_polyline {	// closed sequence of lines
	using Open_polyline::Open_polyline;
    void draw_lines(Painter& painter) const override;
	
//	void add(Point p) { Shape::add(p); }
};


struct Polygon : Closed_polyline {	// closed sequence of non-intersecting lines
	using Closed_polyline::Closed_polyline;
	void add(Point p);
    void draw_lines(Painter& painter) const override;
};

struct Lines : Shape {	// indepentdent lines
	Lines() {}
	Lines(initializer_list<Point> lst) : Shape{lst} { if (lst.size() % 2) error("odd number of points for Lines"); }
    void draw_lines(Painter& painter) const override;
    void add(Point p1, Point p2) { Shape::add(p1); Shape::add(p2); redraw();}
};

struct Text : Shape {
	// the point is the bottom left of the first letter
	Text(Point x, const string& s) : lab{ s } { add(x); }

    void draw_lines(Painter& painter) const override;

    void set_label(const string& s) { lab = s; redraw();}
	string label() const { return lab; }

    void set_font(Font f) { fnt = f; redraw();}
	Font font() const { return Font(fnt); }

    void set_font_size(int s) { fnt_sz = s; redraw();}
	int font_size() const { return fnt_sz; }
private:
	string lab;	// label
    Font fnt{ Font::courier };
    int fnt_sz{ 14 };	// at least 14 point
};


struct Axis : Shape {
	// representation left public
	enum Orientation { x, y, z };
	Axis(Orientation d, Point xy, int length, int nummber_of_notches=0, string label = "");

    void draw_lines(Painter& painter) const override;
    void move(int dx, int dy) override;

	void set_color(Color c);

	Text label;
	Lines notches;
//	Orientation orin;
//	int notches;
};

struct Circle : Shape {
	Circle(Point p, int rr)	// center and radius
	:r{ rr } {
		add(Point{ p.x - r, p.y - r });
	}

    void draw_lines(Painter& painter) const override;

	Point center() const { return { point(0).x + r, point(0).y + r }; }

    void set_radius(int rr) { r=rr; redraw();}
	int radius() const { return r; }
private:
	int r;
};


struct Ellipse : Shape {
	Ellipse(Point p, int ww, int hh)	// center, min, and max distance from center
	:w{ ww }, h{ hh } {
		add(Point{ p.x - ww, p.y - hh });
	}

    void draw_lines(Painter& painter) const override;

	Point center() const { return{ point(0).x + w, point(0).y + h }; }
	Point focus1() const { return{ center().x + int(sqrt(double(w*w - h*h))), center().y }; }
    Point focus2() const { return{ center().x - int(sqrt(double(w*w - h*h))), center().y }; }

    void set_major(int ww) { w=ww; redraw();}
    int major() const { return w; }
    void set_minor(int hh) { h=hh; redraw();}
	int minor() const { return h; }
private:
	int w;
	int h;
};
/*
struct Mark : Text {
	static const int dw = 4;
	static const int dh = 4;
	Mark(Point xy, char c) : Text(Point(xy.x-dw, xy.y+dh),string(1,c)) {}
};
*/

struct Marked_polyline : Open_polyline {
    Marked_polyline(const string& m) :mark(m) { }
    Marked_polyline(const string& m, initializer_list<Point> lst)
        : Open_polyline{ lst }, mark{ m }
    {
        if (m == "")
            mark = "*";
    }
    void draw_lines(Painter& painter) const override;
protected:
    void hide_lines(bool hide = true) {lines_hidden = hide;}
private:
	string mark;
    bool lines_hidden = false;
};

struct Marks : Marked_polyline {
    Marks(const string& m) :Marked_polyline(m)
    { hide_lines(); }
    Marks(const string& m, initializer_list<Point> lst)
        : Marked_polyline{ m,lst }
    {
        hide_lines();
    }
};

struct Mark : Marks {
	Mark(Point xy, char c) : Marks(string(1,c)) {add(xy); }
};

/*

struct Marks : Shape {
	Marks(char m) : mark(string(1,m)) { }
	void add(Point p) { Shape::add(p); }
	void draw_lines() const;
private:
	string mark;
};
*/

class ImagePrivate;
struct Image : Shape {
    Image(Point xy, string s);
    ~Image();
    void draw_lines(Painter& painter) const override;
    void set_mask(Point xy, int ww, int hh) { w=ww; h=hh; cx=xy.x; cy=xy.y; redraw();}
    void move(int dx,int dy) override { Shape::move(dx,dy); redraw(); /*p->draw(point(0).x,point(0).y);*/ }
    ImagePrivate& get_impl() const {return *impl;}
    void scale(int ww, int hh, bool keep_aspect_ratio = true);
private:
	int w,h,cx,cy; // define "masking box" within image relative to position (cx,cy)
	Text fn;
    std::unique_ptr<ImagePrivate> impl;
};

}
#endif

