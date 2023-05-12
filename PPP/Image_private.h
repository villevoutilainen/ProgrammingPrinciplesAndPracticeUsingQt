
#ifndef IMAGE_PRIVATE_GUARD
#define IMAGE_PRIVATE_GUARD 1

#include <QImage>

namespace Graph_lib {

class ImagePrivate
{
public:
    QImage image;
    void load(const std::string& s);
    void scale(int w, int h, bool keep_aspect_ratio = true);
};

}
#endif

