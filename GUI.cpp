#include "PPP/GUI.h"
#include "PPP/GUI_private.h"
#include "PPP/Window.h"
#include "PPP/std_lib_facilities.h"
#include <sstream>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>

#include <QInputDialog>
#include <QMessageBox>

using namespace Graph_lib;


Widget::Widget(Point xy, int w, int h, const string& s, Callback cb)
    : loc(xy), width(w), height(h), label(s), do_it(cb), impl(std::make_unique<WidgetPrivate>())
{}

Widget::~Widget()
{

}

Button::Button(Point xy, int w, int h, const string& label, Callback cb)
    : Widget(xy,w,h,label,cb)
{
    WidgetPrivate& impl = get_impl();
    QPushButton* button = new QPushButton();
    impl.widget = button;
    button->setText(QString::fromStdString(label));
    QObject::connect(button, &QPushButton::clicked, [this]{ do_it(); });
}

void Button::attach(Window& win)
	{
        win.attach(*this);
    }

int In_box::get_int()
{
    return QInputDialog::getInt(&window->get_impl(), QString::fromStdString(label),
                         QString::fromStdString(label));
}

string In_box::get_string()
{
    return QInputDialog::getText(&window->get_impl(), QString::fromStdString(label),
                         QString::fromStdString(label)).toStdString();
}

void In_box::attach(Window& win)
{
    window = &win;
}

void Out_box::put(int i)
{
    std::ostringstream os;
    os << i;
    QString data = QString::fromStdString(os.str());
    data.resize(data.size() + label.size()*2, QChar(' ')); // hack to make the messagebox wider
    QMessageBox::information(&window->get_impl(), QString::fromStdString(label), data);
}

void Out_box::put(const string& s)
{
    QString data = QString::fromStdString(s);
    data.resize(data.size() + label.size()*2, QChar(' ')); // hack to make the messagebox wider
    QMessageBox::information(&window->get_impl(), QString::fromStdString(label),
                             data);
}

void Out_box::attach(Window& win)
{
    window = &win;
}

Menu::Menu(Point xy, int w, int h, Kind kk, const string& label)
    : Widget(xy,w,h,label,0), k(kk), offset(0)
{
    WidgetPrivate& impl = get_impl();
    QWidget* widget = new QPushButton();
    impl.widget = widget;
    if (k == Menu::horizontal) {
        widget->setLayout(new QHBoxLayout());
    } else {
        widget->setLayout(new QVBoxLayout());
    }

}

void Menu::attach(Window& w)
{
    w.attach(*this);
}


int Menu::attach(Button& b)
{
	b.width = width;
	b.height = height;

	switch(k) {
	case horizontal:
		b.loc = Point(loc.x+offset,loc.y);
		offset+=b.width;
		break;
	case vertical:
		b.loc = Point(loc.x,loc.y+offset);
		offset+=b.height;
		break;
	}
    WidgetPrivate& impl = get_impl();
    impl.widget->layout()->addWidget(b.get_impl().widget);
	selection.push_back(&b);
	return int(selection.size()-1);
}

int Menu::attach(Button* p)
{
//	owned.push_back(p);
	return attach(*p);
}
