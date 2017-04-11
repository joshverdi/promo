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
    PText3D(QString _text, GLuint _base);
    ~PText3D();
    void initfont(QFont & f, float thickness); // set up a font and specify the "thickness"
    void print();    // print it in 3D!
    inline QString gettext() const {return text;}
    inline void setcolor(GLfloat* _color){for (int i=0;i<4;i++) color[i]=_color[i];}
    inline GLfloat* getcolor() {return color;}
    inline bool operator==(const PText3D& _t3d){return (text.compare(_t3d.gettext())==0)?true:false;}
    inline QFont* getfont() const {return font;}
    inline bool equalcolor(GLfloat* _color){if ( (color[0]==_color[0])&&(color[1]==_color[1])&&(color[2]==_color[2])&&(color[3]==_color[3])) return true;return false;}
private:
    void buildglyph(GLuint b, int c);   // create one displaylist for character "c"
    QFont *font;
    QFontMetricsF *fm;
    QString text;
    float glyphthickness;
    GLuint base;      // the "base" of our displaylists
    GLfloat* color;
};

#endif // PTEXT3D_H
