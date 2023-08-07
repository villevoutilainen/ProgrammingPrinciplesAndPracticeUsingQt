#include "PPP/Window.h"
#include "PPP/Graph.h"
#include "PPP/Image_private.h"
#include <QApplication>
#include <QPainter>
#include <QVector>
#include <QWidget>
#include "PPP/GUI.h"
#include "PPP/GUI_private.h"

namespace Graph_lib {

class PainterPrivate
{
public:
    QPainter* painter;
    QPen pen;
    QBrush brush;
    QFont font;
};

void Painter::draw_rectangle(const Point& p1, int w, int h)
{
    impl->painter->setPen(impl->pen);
    impl->painter->setBrush(impl->brush);
    impl->painter->drawRect(p1.x, p1.y, w, h);
}

void Painter::draw_line(const Point& p1, const Point& p2)
{
    impl->painter->setPen(impl->pen);
    impl->painter->drawLine(p1.x, p1.y, p2.x, p2.y);
}

void Painter::draw_polygon(const Shape& s)
{
    impl->painter->setPen(impl->pen);
    impl->painter->setBrush(impl->brush);
    QVector<QPoint> points;
    for (int i = 0; i < s.number_of_points(); ++i) {
        Point p = s.point(i);
        points.push_back(QPoint(p.x, p.y));
    }
    impl->painter->drawPolygon(points.data(), points.size());
}

void Painter::draw_text(const Point& p1, const std::string text)
{
    impl->painter->setPen(impl->pen);
    impl->painter->setFont(impl->font);
    impl->painter->drawText(QPoint(p1.x, p1.y), QString::fromStdString(text));
}

void Painter::draw_ellipse(const Point& p1, int r, int r2)
{
    impl->painter->setPen(impl->pen);
    impl->painter->setBrush(impl->brush);
    impl->painter->drawEllipse(QPoint(p1.x, p1.y), r, r2);
}

void Painter::draw_image(const Point& p1, const Image& img)
{
    impl->painter->drawImage(QPoint(p1.x, p1.y), img.get_impl().image);
}

void Painter::draw_image(const Point& p1, const Point& p2, int w, int h, const Image& img)
{
    impl->painter->drawImage(QPoint(p1.x, p1.y), img.get_impl().image, QRect(p2.x, p2.y, w, h));
}

void Painter::save()
{
    impl->painter->save();
}

void Painter::restore()
{
    impl->painter->restore();
}

static QColor map_color(Color color)
{
    static const QMap<Color, QColor> color_map = {
        {Color::red, QColorConstants::Red},
        {Color::blue, QColorConstants::Blue},
        {Color::green, QColorConstants::Green},
        {Color::yellow, QColorConstants::Yellow},
        {Color::white, QColorConstants::White},
        {Color::black, QColorConstants::Black},
        {Color::magenta, QColorConstants::Magenta},
        {Color::cyan, QColorConstants::Cyan},
        {Color::dark_red, QColorConstants::DarkRed},
        {Color::dark_green, QColorConstants::DarkGreen},
        {Color::dark_yellow, QColorConstants::DarkYellow},
        {Color::dark_blue, QColorConstants::DarkBlue},
        {Color::dark_magenta, QColorConstants::DarkMagenta},
        {Color::dark_cyan, QColorConstants::DarkCyan},
    };
    if (color.type() == Color::palette_index)
        return mapPaletteColor(color.as_int());
    if (color.type() == Color::rgb)
        return QColor(color.red_component(), color.green_component(), color.blue_component());
    return color_map[color];
}

void Painter::set_color(Color color)
{
    if (color.visibility() != Color::invisible) {
        impl->pen.setColor(map_color(color));
    } else {
        impl->pen.setStyle(Qt::NoPen);
    }

}

void Painter::set_fill_color(Color color)
{
    if (color.visibility() != Color::invisible) {
        impl->brush.setColor(map_color(color));
        impl->brush.setStyle(Qt::SolidPattern);
    } else {
        impl->brush.setStyle(Qt::NoBrush);
    }
}

void Painter::set_line_style(Line_style style)
{
    static const QMap<Line_style, Qt::PenStyle> line_style_map = {
        {Line_style::none, Qt::NoPen},
        {Line_style::solid, Qt::SolidLine},
        {Line_style::dash, Qt::DashLine},
        {Line_style::dot, Qt::DotLine},
        {Line_style::dashdot, Qt::DashDotLine},
        {Line_style::dashdotdot, Qt::DashDotDotLine}
    };
    impl->pen.setStyle(line_style_map[style]);
    impl->pen.setWidth(style.width());
}

void Painter::set_font_size(int s)
{
    impl->font.setPointSize(s);
}

void Painter::set_font(Font f)
{
    static QMap<Font, QFont> fontMap = {
        {Font::helvetica, QFont("Helvetica", 14) },
        {Font::helvetica_bold, QFont("Helvetica", 14, QFont::Bold)},
        {Font::helvetica_italic, QFont("Helvetica", 14, QFont::Normal, true)},
        {Font::helvetica_bold_italic, QFont("Helvetica", 14, QFont::Bold, true)},
        {Font::courier, QFont("Courier", 14) },
        {Font::courier_bold, QFont("Courier", 14, QFont::Bold)},
        {Font::courier_italic, QFont("Courier", 14, QFont::Normal, true)},
        {Font::courier_bold_italic, QFont("Courier", 14, QFont::Bold, true)},
        {Font::times, QFont("Times", 14) },
        {Font::times_bold, QFont("Times", 14, QFont::Bold)},
        {Font::times_italic, QFont("Times", 14, QFont::Normal, true)},
        {Font::times_bold_italic, QFont("Times", 14, QFont::Bold, true)},
        {Font::symbol, QFont("Symbol", 14)},
        {Font::screen, QFont("Screen", 14)},
        {Font::screen_bold, QFont("Screen", 14, QFont::Bold)},
        {Font::zapf_dingbats, QFont("Zapf Dingbats", 14)}
    };
    QFont painter_font = fontMap[f];
    impl->font = painter_font;
}

void ImagePrivate::load(const std::string& s)
{
    image.load(QString::fromStdString(s));
}

void ImagePrivate::scale(int ww, int hh, bool keep_aspect_ratio)
{
    image = image.scaled(QSize(ww, hh), keep_aspect_ratio ? Qt::KeepAspectRatio : Qt::IgnoreAspectRatio);
}

class ApplicationPrivate
{
public:
    ApplicationPrivate() : app(argc, nullptr) {}
    void gui_main() { app.exec(); }
    void quit() { app.quit(); }
private:
    int argc = 0;
    QApplication app;
};

Application::Application() : impl(std::make_unique<ApplicationPrivate>())
{

}

Application::~Application()
{

}

void Application::gui_main()
{
    impl->gui_main();
}

void Application::quit()
{
    impl->quit();
}

Painter::Painter(std::unique_ptr<PainterPrivate>&& pp)
    : impl(std::move(pp))
{
}

Painter::~Painter() {}

void WindowPrivate::paintEvent(QPaintEvent*/*event*/)
{
    std::unique_ptr<PainterPrivate> priv =
            std::make_unique<PainterPrivate>();
    QPainter painter(this);
    priv->painter = &painter;
    Painter shape_painter(std::move(priv));
    for (auto&& shape : shapes) {
        shape->draw(shape_painter);
    }
}

void WindowPrivate::closeEvent(QCloseEvent*/*event*/)
{
    QObject::connect(&timer, &QTimer::timeout,
                     [this] {nested_loop.quit();});
    timer.start(0);

    wnd->windowClosed();
}

void WindowPrivate::wait_for_button(Button* button)
{
    stored_button = button;
    stored_callback = stored_button->do_it;
    stored_button->do_it = [&] {nested_loop.exit();};
    nested_loop.exec();
    stored_button->do_it = stored_callback;
    stored_button->do_it();

}

Window::Window(int ww, int hh, const string& title)
    : w(ww), h(hh), impl(std::make_unique<WindowPrivate>(this))
{
    impl->setGeometry(0, 0, ww, hh);
    impl->setWindowTitle(QString::fromStdString(title));
    impl->show();
	init();
}

Window::Window(Point xy, int ww, int hh, const string& title)
    : w(ww), h(hh), impl(std::make_unique<WindowPrivate>(this))
{
    impl->setGeometry(xy.x, xy.y, ww, hh);
    impl->setWindowTitle(QString::fromStdString(title));
    impl->show();
    init();
}

Window::~Window()
{

}

void Window::set_label(const string &s)
{
    impl->setWindowTitle(QString::fromStdString(s));
}

void Window::init()
{
   //resizable(this);
   //show();
} 

//---------------------------------------------------- 

void Window::draw()
{
    impl->repaint();
    //Fl_Window::draw();
    //for (unsigned int i=0; i<shapes.size(); ++i) shapes[i]->draw();
}

void Window::attach(Widget& ww)
{
    QWidget* widget = ww.get_impl().widget;
    if (widget) {
        widget->setParent(impl.get());
        widget->move(ww.loc.x, ww.loc.y);
        widget->setMinimumWidth(ww.width);
        widget->setMinimumHeight(ww.height);
        widget->show();
    }
    ww.attach(*this);
    //begin();			// FTLK: begin attaching new Fl_Wigets to this window
    //	w.attach(*this);	// let the Widget create its Fl_Wigits
    //end();				// FTLK: stop attaching new Fl_Wigets to this window
}

void Window::detach(Widget& ww)
{
    QWidget* widget = ww.get_impl().widget;
    widget->setParent(nullptr);
    widget->hide();
}

void Window::attach(Shape& s)
{
    impl->attach(s);
//		s.attached = this;
    s.set_window(this);
}
void Window::detach(Shape& s)
{
    impl->detach(s);
    s.set_window(nullptr);
}


void Window::put_on_top(Shape& p) {
    impl->put_on_top(p);
    draw();
}

void Window::close()
{
    impl->close();
}

WindowPrivate& Window::get_impl() const
{
    return *impl;
}

int gui_main()
{
    int argc=0;
    QApplication a(argc, nullptr);
    return a.exec();
}

} // Graph

