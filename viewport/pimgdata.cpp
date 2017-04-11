#include "pimgdata.h"

PImgData::PImgData()
{
    name_gl = -1;
    texture_name = 0;
    group_id = 0;
}

PImgData::PImgData(GLuint* _texture_name, int _name_gl, std::string _filename, std::string _grammar_fn, QImage _image, int _group_id)
    : texture_name(_texture_name), name_gl(_name_gl), filename(_filename), grammar_filename(_grammar_fn), image(_image), group_id(_group_id)
{
}

//PImgData::PImgData(int _name_gl, GLuint* _texture_name, std::string _filename, QImage _image, int _group_id)
//    :name_gl(_name_gl), texture_name(_texture_name), filename(_filename), image(_image), group_id(_group_id)
//{
//}

PImgData& PImgData::operator=(const PImgData& _imgdata)
{
    name_gl         = _imgdata.getNameGl();
    texture_name    = _imgdata.getTextureName();
    filename        = _imgdata.getFilename();
    grammar_filename= _imgdata.getGrammarFilename();
    image           = _imgdata.getImage();
    group_id        = _imgdata.getGroupId();
}
