#ifndef PTEXT3D_H
#define PTEXT3D_H

#include <QOpenGLFunctions>
#include <QString>
#include <QFont>
#include <QFontMetricsF>

class PText3D
{
public:
    PText3D(QString _text);
    void initfont(QFont & f, float thickness); // set up a font and specify the "thickness"
    void print();    // print it in 3D!

private:
    void buildglyph(GLuint b, int c);   // create one displaylist for character "c"
    QFont * font;
    QFontMetricsF *fm;
    QString text;
    float glyphthickness;
    GLuint base;      // the "base" of our displaylists
};

#endif // PTEXT3D_H
