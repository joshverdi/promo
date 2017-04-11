#ifndef PIMGDATA_H
#define PIMGDATA_H

#include <string>
#include <QImage>
#include "GL/gl.h"

class PImgData
{
    int name_gl;
    GLuint* texture_name;
    std::string filename;
    std::string grammar_filename;
    QImage image;
    int group_id;    
    bool is_mainimg ;

public:
    PImgData();
//    PImgData(PImgData& _imgdt);
    PImgData(GLuint* _texture_name, int _name_gl, std::string _filename, std::string _grammar_fn, QImage _image, int _group_id);
//    PImgData(int _name_gl, GLuint* _texture_name, std::string _filename, QImage _image, int _group_id);
    int getNameGl() const {return name_gl;}
    void setNameGl(int _name_gl){name_gl = _name_gl;}
    std::string getGrammarFilename() const {return grammar_filename;}
    GLuint* getTextureName() const {return texture_name;}
    void setTextureName(GLuint* _texture_name){texture_name = _texture_name;}
    std::string  getFilename() const {return filename;}
    int getGroupId() const {return group_id;}
    void setGroupId(int _group_id){group_id = _group_id;}
    QImage getImage() const {return image;}
    void setImage(QImage& _image){image = _image;}
    PImgData& operator=(const PImgData& _imgdata);
    bool isMainImg() const {return is_mainimg;}
    void setMainImg(bool _mainimg){is_mainimg = _mainimg;}

};

#endif // PIMGDATA_H
