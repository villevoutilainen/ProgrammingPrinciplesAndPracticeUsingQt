#include "PPP/GUI.h"
#include "PPP/GUI_private.h"
#include "PPP/Window.h"
#include "PPP/std_lib_facilities.h"
#include <sstream>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>

#include <QInputDialog>
#include <QKeyEvent>
#include <QMessageBox>

using namespace Graph_lib;


Widget::Widget(Point xy, int w, int h, const string& s, Callback cb)
    : loc(xy), width(w), height(h), label(s), do_it(cb), impl(std::make_unique<WidgetPrivate>())
{}

Widget::~Widget()
{

}

void Widget::move(int dx,int dy)
{
    QRect geom = impl->widget->geometry();
    geom.moveLeft(geom.left() + dx);
    geom.moveTop(geom.top() + dy);
    impl->widget->setGeometry(geom);
    loc.x += dx;
    loc.y += dy;
}

void Widget::hide() {impl->widget->setVisible(false);}
void Widget::show() {impl->widget->setVisible(true);}

Button::Button(Point xy, int w, int h, const string& label, Callback cb)
    : Widget(xy,w,h,label,cb)
{
    WidgetPrivate& w_impl = get_impl();
    QPushButton* button = new QPushButton();
    w_impl.widget.reset(button);
    button->setText(QString::fromStdString(label));
    QObject::connect(button, &QPushButton::clicked, [this]{ do_it(); });
}

class InputDialog : public QInputDialog
{
public:
    InputDialog(QWidget* parent = nullptr) : QInputDialog(parent) {}
    void done(int result) override
    {
        if (result == QDialog::Rejected) {
            if (close_on_accept) {
                QInputDialog::done(result);
            } else {
                QInputDialog::rejected();
            }
        } else if (result == QDialog::Accepted) {
            if (close_on_accept) {
                QInputDialog::done(result);
            } else {
                QInputDialog::accepted();
            }
        }
    }
    void keyPressEvent(QKeyEvent *e) override {
        if ((options() | QInputDialog::NoButtons)) {
            if (e->key() == Qt::Key_Return) {
                done(QDialog::Accepted);
            } else if (e->key() == Qt::Key_Escape) {
                done(QDialog::Rejected);
            }
        } else {
            QInputDialog::keyPressEvent(e);
        }
    }
    void set_close_on_accept(bool val) {close_on_accept = val;}
private:
    bool close_on_accept = false;
};

In_box::In_box(Point xy, int w, int h, const string& s, Callback cb)
    :Widget(xy,w,h,s,0)
{
    WidgetPrivate& w_impl = get_impl();
    InputDialog* dialog = new InputDialog();
    dialog->setVisible(false);
    w_impl.widget.reset(dialog);
    do_it = cb;
}

void setup_input(InputDialog* dialog, const std::string& label,
                 bool close_on_accept,
                 QInputDialog::InputMode inputMode,
                 Callback& do_it,
                 In_box::ResultData& result)
{
    dialog->setWindowTitle(QString::fromStdString(label));
    dialog->setLabelText(QString::fromStdString(label));
    dialog->set_close_on_accept(close_on_accept);
    dialog->setInputMode(inputMode);
    dialog->setModal(false);
    QObject::connect(dialog, &QInputDialog::accepted,
        [dialog, &do_it, &result] {
            result.state = Graph_lib::In_box::accepted;
            if (dialog->inputMode() == QInputDialog::IntInput) {
                result.last_int = dialog->intValue();
                dialog->setIntValue(0);
            } else if ((dialog->inputMode() == QInputDialog::TextInput)) {
                result.last_string = dialog->textValue().toStdString();
                dialog->setTextValue("");
            }
            if (do_it) {
                do_it();
            }
    });
    QObject::connect(dialog, &QInputDialog::rejected,
        [dialog, &do_it, &result] {
            result.state = Graph_lib::In_box::rejected;
            if (dialog->inputMode() == QInputDialog::TextInput) {
                dialog->setTextValue("");
            } else if (dialog->inputMode() == QInputDialog::IntInput) {
                dialog->setIntValue(0);
            }
            if (do_it) {
                do_it();
            }
    });

}

void wait_for_input(InputDialog* dialog, In_box::State& state)
{
    QEventLoop nested_loop;
    QTimer timer(&nested_loop);
    timer.setSingleShot(true);
    QObject::connect(&timer, &QTimer::timeout,
                     [&] {nested_loop.quit();});
    auto conn1 = QObject::connect(dialog, &QInputDialog::accepted,
        [&] {
                state = In_box::accepted;
                timer.start(0);
    });
    auto conn2 = QObject::connect(dialog, &QInputDialog::rejected,
        [&] {
                state = In_box::rejected;
                timer.start(0);
    });
    nested_loop.exec();
    QObject::disconnect(conn1);
    QObject::disconnect(conn2);
}

void exec_input(InputDialog* dialog, In_box::State& state)
{
    auto conn1 = QObject::connect(dialog, &QInputDialog::accepted,
        [&] {
                state = In_box::accepted;
    });
    auto conn2 = QObject::connect(dialog, &QInputDialog::rejected,
        [&] {
                state = In_box::rejected;
    });
    dialog->exec();
    QObject::disconnect(conn1);
    QObject::disconnect(conn2);
}

int In_box::get_int()
{
    InputDialog* dialog = static_cast<InputDialog*>(get_impl().widget.get());
    if (waiting) {
        dialog->reject();
        result.state = rejected;
        return 0;
    }
    result.state = idle;
    setup_input(dialog, label, true, QInputDialog::IntInput,
                do_it, result);
    waiting = true;
    exec_input(dialog, result.state);
    waiting = false;
    return dialog->intValue();
}

int In_box::get_int_keep_open()
{
    InputDialog* dialog = static_cast<InputDialog*>(get_impl().widget.get());
    if (waiting) {
        dialog->reject();
        result.state = rejected;
        return 0;
    }
    result.state = idle;
    setup_input(dialog, label, false, QInputDialog::IntInput,
                do_it, result);
    dialog->show();
    waiting = true;
    wait_for_input(dialog, result.state);
    waiting = false;
    return dialog->intValue();
}

string In_box::get_string()
{
    InputDialog* dialog = static_cast<InputDialog*>(get_impl().widget.get());
    if (waiting) {
        dialog->reject();
        result.state = rejected;
        return "";
    }
    result.state = idle;
    setup_input(dialog, label, true, QInputDialog::TextInput,
                do_it, result);
    waiting = true;
    exec_input(dialog, result.state);
    waiting = false;
    return dialog->textValue().toStdString();
}

string In_box::get_string_keep_open()
{
    InputDialog* dialog = static_cast<InputDialog*>(get_impl().widget.get());
    if (waiting) {
        dialog->reject();
        result.state = rejected;
        return "";
    }
    result.state = idle;
    setup_input(dialog, label, false, QInputDialog::TextInput,
                do_it, result);
    dialog->show();
    waiting = true;
    wait_for_input(dialog, result.state);
    waiting = false;
    return dialog->textValue().toStdString();
}

void In_box::attach(Window& win)
{
    InputDialog* dialog = static_cast<InputDialog*>(get_impl().widget.get());
    dialog->hide();
    QObject::connect(&win.get_impl(), &WindowPrivate::windowClosed,
                     [=] {dialog->reject();});
}

void In_box::dismiss()
{
    InputDialog* dialog = static_cast<InputDialog*>(get_impl().widget.get());
    dialog->reject();
    result.state = rejected;
}

void In_box::hide()
{
    InputDialog* dialog = static_cast<InputDialog*>(get_impl().widget.get());
    dialog->hide();
}

void In_box::show()
{
    InputDialog* dialog = static_cast<InputDialog*>(get_impl().widget.get());
    setup_input(dialog, label, false, QInputDialog::TextInput,
                do_it, result);
    dialog->show();
}

void In_box::hide_buttons()
{
    InputDialog* dialog = static_cast<InputDialog*>(get_impl().widget.get());
    dialog->setOption(QInputDialog::NoButtons, true);
}

void In_box::show_buttons()
{
    InputDialog* dialog = static_cast<InputDialog*>(get_impl().widget.get());
    dialog->setOption(QInputDialog::NoButtons, false);
}

In_box::State In_box::last_result()
{
     return result.state;
}

void In_box::clear_last_result()
{
     result.state = idle;
     result.last_string = "";
     result.last_int = 0;
}

std::string In_box::last_string_value()
{
     return result.last_string;
}

int In_box::last_int_value()
{
     return result.last_int;
}

Menu::Menu(Point xy, int w, int h, Kind kk, const string& label)
    : Widget(xy,w,h,label,0), k(kk), offset(0)
{
    WidgetPrivate& w_impl = get_impl();
    QWidget* widget = new QWidget();
    w_impl.widget.reset(widget);
    if (k == Menu::horizontal) {
        QHBoxLayout* layout = new QHBoxLayout();
        layout->setContentsMargins(0,0,0,0);
        layout->setSpacing(0);
        widget->setLayout(layout);
    } else {
        QVBoxLayout* layout = new QVBoxLayout();
        layout->setContentsMargins(0,0,0,0);
        layout->setSpacing(0);
        widget->setLayout(layout);
    }

}

void Menu::layoutButtons(Button& b)
{
    b.width = width;
    b.height = height;

    switch(k) {
    case horizontal:
        b.loc = Point{loc.x+offset,loc.y};
        offset+= b.width;
        break;
    case vertical:
        b.loc = Point{loc.x,loc.y+offset};
        offset+= b.height;
        break;
    }
    b.get_impl().widget->setMaximumHeight(height);
    b.get_impl().widget->setMaximumWidth(width);
    get_impl().widget->layout()->addWidget(b.get_impl().widget.get());
}

void Menu::layoutMenu()
{
    if (k == vertical) {
        get_impl().widget->setMinimumHeight(height * selection.size());
    } else {
        get_impl().widget->setMinimumWidth(width * selection.size());
    }
}

int Menu::attach(Button& b)
{
    layoutButtons(b);
    selection.push_back(b);
    layoutMenu();
    return int(selection.size()-1);
}

int Menu::attach(unique_ptr<Button> p)
{
    layoutButtons(*p);
    selection.push_back(std::move(p));
    layoutMenu();
    return int(selection.size()-1);
}
