#include "matrix.hpp"
#include "image.hpp"
bool SScUCMatrix::save(const QString &filename, const char* format)
{
    SScImage im(*this);
    return im.save(filename,format);
}



