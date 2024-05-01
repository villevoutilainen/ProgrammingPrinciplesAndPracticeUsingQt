#include "PPP/Graph.h"
#include "PPP/Window.h"
#include "PPP/Image_private.h"


namespace Graph_lib {

Shape::~Shape()
{
    if (parent_window) {
        parent_window->detach(*this);
    }
}

void Shape::draw(Painter& painter) const
{
    painter.save();
    painter.set_line_style(style());
    // Color must be set after line style so that
    // setting an invisible color can set the line style to "no pen".
    painter.set_color(color());
    painter.set_fill_color(fill_color());
    draw_specifics(painter);
    painter.restore();
}

void Shape::redraw()
{
    if (parent_window)
        parent_window->draw();
}
// does two lines (p1,p2) and (p3,p4) intersect?
// if se return the distance of the intersect point as distances from p1
inline pair<double,double> line_intersect(Point p1, Point p2, Point p3, Point p4, bool& parallel) 
{
    double x1 = p1.x;
    double x2 = p2.x;
	double x3 = p3.x;
	double x4 = p4.x;
	double y1 = p1.y;
	double y2 = p2.y;
	double y3 = p3.y;
	double y4 = p4.y;

	double denom = ((y4 - y3)*(x2-x1) - (x4-x3)*(y2-y1));
	if (denom == 0){
		parallel= true;
		return pair<double,double>(0,0);
	}
	parallel = false;
	return pair<double,double>( ((x4-x3)*(y1-y3) - (y4-y3)*(x1-x3))/denom,
								((x2-x1)*(y1-y3) - (y2-y1)*(x1-x3))/denom);
}


//intersection between two line segments
//Returns true if the two segments intersect,
//in which case intersection is set to the point of intersection
bool line_segment_intersect(Point p1, Point p2, Point p3, Point p4, Point& intersection){
   bool parallel;
   pair<double,double> u = line_intersect(p1,p2,p3,p4,parallel);
   if (parallel || u.first < 0 || u.first > 1 || u.second < 0 || u.second > 1) return false;
   intersection.x = p1.x + u.first*(p2.x - p1.x);
   intersection.y = p1.y + u.first*(p2.y - p1.y);
   return true;
} 

void Polygon::add(Point p)
{
	int np = number_of_points();

	if (1<np) {	// check that thenew line isn't parallel to the previous one
		if (p==point(np-1)) error("polygon point equal to previous point");
		bool parallel;
		line_intersect(point(np-1),p,point(np-2),point(np-1),parallel);
		if (parallel)
			error("two polygon points lie in a straight line");
	}

	for (int i = 1; i<np-1; ++i) {	// check that new segment doesn't interset and old point
        Point ignored_point{0,0};
        if (line_segment_intersect(point(np-1),p,point(i-1),point(i),ignored_point))
			error("intersect in polygon");
	}
	

	Closed_polyline::add(p);
}


void Polygon::draw_specifics(Painter& painter) const
{
    if (number_of_points() < 3) error("less than 3 points in a Polygon");
    Closed_polyline::draw_specifics(painter);
}

void Open_polyline::draw_specifics(Painter& painter) const
{
    if (color().visibility() && 1<number_of_points())	// draw sole pixel?
        for (int i=1; i<number_of_points(); ++i)
            painter.draw_line(point(i-1), point(i));
}

void Closed_polyline::draw_specifics(Painter& painter) const
{
    painter.draw_polygon(*this);
}
void Shape::move(int dx, int dy)
{
	for (unsigned int i = 0; i<points.size(); ++i) {
		points[i].x+=dx;
		points[i].y+=dy;
	}
    redraw();
}

void Lines::draw_specifics(Painter& painter) const
{
    if (number_of_points()%2==1) error("odd number of points in set of lines");
	if (color().visibility())
		for (int i=1; i<number_of_points(); i+=2)
            painter.draw_line(point(i-1) ,point(i));
}

void Text::draw_specifics(Painter& painter) const
{
    painter.set_font(font());
    painter.set_font_size(font_size());
    painter.draw_text(point(0), lab);
}

Function::Function(std::function<double(double)> f, double r1, double r2, Point xy, int count, double xscale, double yscale)
// graph f(x) for x in [r1:r2) using count line segments with (0,0) displayed at xy
// x coordinates are scaled by xscale and y coordinates scaled by yscale
{
	if (r2-r1<=0) error("bad graphing range");
	if (count<=0) error("non-positive graphing count");
	double dist = (r2-r1)/count;
	double r = r1;
    for (int i = 0; i<count; ++i) {
        add(Point{xy.x+int(r*xscale),xy.y-int(f(r)*yscale)});
		r += dist;
	}
}
void Line::draw_specifics(Painter& painter) const
{
    painter.draw_line(point(0), point(1));
}

void Rectangle::draw_specifics(Painter& painter) const
{
    painter.draw_rectangle(point(0), w, h);
}


Axis::Axis(Orientation d, Point xy, int length, int n, string lab)
    :label(Point{0,0},lab),
    line(xy, d==x ?
                   Point{xy.x+length, xy.y} :
                   Point{xy.x, xy.y-length})
{
	if (length<0) error("bad axis length");
	switch (d){
	case Axis::x:
        {
			if (1<n) {
				int dist = length/n;
				int x = xy.x+dist;
                for (int i = 0; i<n; ++i) {
                    notches.add(Point{x,xy.y},Point{x,xy.y-5});
				x += dist;
			}
		}
		// label under the line
		label.move(length/3,xy.y+20);
		break;
	}
	case Axis::y:
        {
			if (1<n) {
			int dist = length/n;
			int y = xy.y-dist;
            for (int i = 0; i<n; ++i) {
                notches.add(Point{xy.x,y},Point{xy.x+5,y});
				y -= dist;
			}
		}
		// label at top
		label.move(xy.x-10,xy.y-length-10);
		break;
	}
	case Axis::z:
		error("z axis not implemented");
	}
}

void Axis::draw_specifics(Painter& painter) const
{
    line.draw(painter);	// the line
    notches.draw(painter);	// the notches may have a different color from the line
    label.draw(painter);	// the label may have a different color from the line
}


void Axis::set_color(Color c)
{
	Shape::set_color(c);
	notches.set_color(c);
	label.set_color(c);
    line.set_color(c);
    redraw();
}

void Axis::move(int dx, int dy)
{
	Shape::move(dx,dy);
	notches.move(dx,dy);
	label.move(dx,dy);
    redraw();
}

void Circle::draw_specifics(Painter& painter) const
{
    painter.draw_ellipse(center(), r, r);
}


void Ellipse::draw_specifics(Painter& painter) const
{
    painter.draw_ellipse(center(), w, h);
}

void Arc::draw_specifics(Painter& painter) const
{
    painter.draw_arc(point(0), w*2, h*2, start_angle*16, degrees*16);
}

void Pie::draw_specifics(Painter& painter) const
{
    painter.draw_pie(point(0), w*2, h*2, start_angle*16, degrees*16);
}

void draw_mark(Painter& painter, Point xy, char c)
{
    string m(1,c);
    painter.draw_centered_text(xy, m);
}

void Marked_polyline::draw_specifics(Painter& painter) const
{
    Open_polyline::draw_specifics(painter);

    painter.set_line_style(style());
    painter.set_color(m_color);
    painter.set_font(font());
    painter.set_font_size(font_size());
    for (int i=0; i<number_of_points(); ++i)
        draw_mark(painter, point(i),mark[i%mark.size()]);
}

bool can_open(const string& s)
            // check if a file named s exists and can be opened for reading
{
	ifstream ff(s.c_str());
    return static_cast<bool>(ff);
}



// somewhat overelaborate constructor
// because errors related to image files can be such a pain to debug
Image::Image(Point xy, string s)
    :w(0), h(0), fn(xy,""), impl(std::make_unique<ImagePrivate>())
{
	add(xy);

	if (!can_open(s)) {
		fn.set_label("cannot open \""+s+'\"');
		return;
	}
    impl->load(s);
}

Image::~Image() {}

void Image::draw_specifics(Painter& painter) const
{
    if (fn.label()!="") fn.draw_specifics(painter);

    if (w&&h) {
        Point p = point(0);
        p.x += cx;
        p.y += cy;
        painter.draw_image(point(0), p, w, h, *this);
    }
	else
        painter.draw_image(point(0), *this);
}

void Image::scale(int ww, int hh, bool keep_aspect_ratio)
{
    impl->scale(ww, hh, keep_aspect_ratio);
    redraw();
}

void Out_box::put(int i)
{
    std::ostringstream os;
    os << i;
    data.set_label(os.str());
    redraw();
}

void Out_box::put(const std::string& s)
{
    data.set_label(s);
    redraw();
}

void Out_box::draw_specifics(Painter& painter) const
{
    Vector_ref<const Text> texts;
    texts.push_back(label);
    texts.push_back(data);
    if (orientation == horizontal) {
        painter.draw_text_line(point(0), texts);
    } else {
        painter.draw_text_column(point(0), texts);
    }
}

} // Graph
