#include "viewerraw.h"
#include "engine/ptextparser.h"
#include "QGLViewer/manipulatedCameraFrame.h"
#include "GL/glut.h"

#include <QFile>
#include <QMouseEvent>
#include <QMessageBox>
#include <iostream>
#include <QDir>

using namespace qglviewer;
using namespace std;

ViewerRaw::ViewerRaw(int _type, QWidget* _parent, const QGLWidget* _share_widget)
    : QGLViewer(_parent, _share_widget)
{
    if (_type < 3)
    {
      // Move camera according to viewer type (on X, Y or Z axis)
      camera()->setPosition(Vec((_type==0)? 1.0 : 0.0, (_type==1)? 1.0 : 0.0, (_type==2)? 1.0 : 0.0));
      camera()->lookAt(sceneCenter());

      camera()->setType(Camera::ORTHOGRAPHIC);
      camera()->showEntireScene();

      // Forbid rotation
      WorldConstraint* constraint = new WorldConstraint();
      constraint->setRotationConstraintType(AxisPlaneConstraint::FORBIDDEN);
      camera()->frame()->setConstraint(constraint);
    }
}

ViewerRaw::ViewerRaw(QString _csv_filename, int _type, QWidget* _parent, const QGLWidget* _share_widget)
    : QGLViewer(_parent, _share_widget)
{
    qstr_csv_fn = _csv_filename;
    if (_type < 3)
    {
      // Move camera according to viewer type (on X, Y or Z axis)
      camera()->setPosition(Vec((_type==0)? 1.0 : 0.0, (_type==1)? 1.0 : 0.0, (_type==2)? 1.0 : 0.0));
      camera()->lookAt(sceneCenter());

      camera()->setType(Camera::ORTHOGRAPHIC);
      camera()->showEntireScene();

      // Forbid rotation
      WorldConstraint* constraint = new WorldConstraint();
      constraint->setRotationConstraintType(AxisPlaneConstraint::FORBIDDEN);
      camera()->frame()->setConstraint(constraint);
    }
}

ViewerRaw::ViewerRaw(QString _csv_filename, QString _id_user, QString _date, int _type, QWidget* _parent, const QGLWidget* _share_widget)
    : QGLViewer(_parent, _share_widget)
{
    qstr_csv_fn = _csv_filename;
    if (_type < 3)
    {
      // Move camera according to viewer type (on X, Y or Z axis)
      camera()->setPosition(Vec((_type==0)? 1.0 : 0.0, (_type==1)? 1.0 : 0.0, (_type==2)? 1.0 : 0.0));
      camera()->lookAt(sceneCenter());

      camera()->setType(Camera::ORTHOGRAPHIC);
      camera()->showEntireScene();

      // Forbid rotation
      WorldConstraint* constraint = new WorldConstraint();
      constraint->setRotationConstraintType(AxisPlaneConstraint::FORBIDDEN);
      camera()->frame()->setConstraint(constraint);
    }
    hash_save = new QHash<QString,int>(); //create hashtab img to  line in file to save inf
    id = _id_user;
    date_test = _date;
    test_name = _csv_filename;
}

ViewerRaw::~ViewerRaw()
{
    glDeleteTextures(NB_OUTPUT_IMG, texture_names);
    delete [] texture_names;
    saveStateToFile();
}

void ViewerRaw::init()
{
    QColor bg_color(0, 0, 0);
//    QVector<PImgData>* my_imgdata;
//    QImage my_img;
//    QStringList qsl_fn;
//    QVector<QImage> qvec_img;

    setSceneRadius(10);
    showEntireScene();
    setBackgroundColor(bg_color);


    box_length   = 0.1f;
    box_width    = 3.0f;
    box_height   = 2.0f;
    title_border = 1.0f;
    highlighting_thickness = 0.125f;

    if (!qstr_csv_fn.toStdString().empty())
        load_image_list(qstr_csv_fn);

    // Default initialization goes here (including the declaration of a possible manipulatedFrame).
    QString my_statefilename(".viewerraw.xml");
    setStateFileName(my_statefilename);
    if (!restoreStateFromFile())
        showEntireScene();  // Previous state cannot be restored: fit camera to scene.
                            // Specific initialization that overrides file savings goes here.

    name_drawn      = true;
    nb_column       = 3;
    snapshot_spacing= 0.05f;


    float width=box_width*3;
    float height=box_height*(27/3);
    qglviewer::Camera* cam=camera();
    float fovy =cam->fieldOfView();
    double ycam = 0;
    double pos_x = (width/2.0)*1.6/tan(fovy/2);
    double pos_z = (height/2.0)*1.6/tan(fovy/2);
    ycam = (std::fmax(pos_x,pos_z));
    cam->setPosition(qglviewer::Vec(width/2.0,ycam,height/2.0));
    cam->lookAt(sceneCenter());
    cam->setUpVector(qglviewer::Vec(0.0,1.0,0.0));
    //cam->setType(qglviewer::Camera::PERSPECTIVE);
    //pMainViewer->updateGL();
    qglviewer::Vec pos_vec2= cam->position();

    setCamera(cam);
}
void ViewerRaw::setNbColumn(int nb)
{
    nb_column=nb;
    draw();
    float width=box_width*nb;
    float height=box_height*(27/nb);
    qglviewer::Camera* cam=camera();
    float fovy =cam->fieldOfView();
    double ycam = 0;
    double pos_x = (width/2.0)*1.6/tan(fovy/2);
    double pos_z = (height/2.0)*1.6/tan(fovy/2);
    ycam = (std::fmax(pos_x,pos_z));
    cam->setPosition(qglviewer::Vec(width/2.0,ycam,height/2.0));
    cam->lookAt(sceneCenter());
    cam->setUpVector(qglviewer::Vec(0.0,1.0,0.0));
    //cam->setType(qglviewer::Camera::PERSPECTIVE);
    //pMainViewer->updateGL();
    qglviewer::Vec pos_vec2= cam->position();

    setCamera(cam);

}

void ViewerRaw::compute_vertices(GLfloat _out_vertices[8][3], GLfloat _out_normals[6][3], GLint _out_faces[6][4], GLfloat length, GLfloat width, GLfloat height)
{
    GLfloat normals_tmp[6][3] = {  /* Normals for the 6 faces of a cube. */
          {0.0, 1.0, 0.0}, {0.0, 0.0, -1.0}, {0.0, -1.0, 0.0},
          {0.0, 0.0, 1.0}, {1.0, 0.0, 0.0},  {-1.0, 0.0, 0.0} };
    GLint faces_tmp[6][4] = {  /* Vertex indices for the 6 faces of a cube. */
          {0, 1, 2, 3}, {3, 2, 6, 7}, {7, 6, 5, 4},
          {4, 5, 1, 0}, {5, 6, 2, 1}, {7, 4, 0, 3} };

    _out_vertices[0][0] = 0;        _out_vertices[0][1] = height; _out_vertices[0][2] = width;
    _out_vertices[1][0] = length;   _out_vertices[1][1] = height; _out_vertices[1][2] = width;
    _out_vertices[2][0] = length;   _out_vertices[2][1] = height; _out_vertices[2][2] = 0;
    _out_vertices[3][0] = 0;        _out_vertices[3][1] = height; _out_vertices[3][2] = 0;
    _out_vertices[4][0] = 0;        _out_vertices[4][1] = 0;      _out_vertices[4][2] = width;
    _out_vertices[5][0] = length;   _out_vertices[5][1] = 0;      _out_vertices[5][2] = width;
    _out_vertices[6][0] = length;   _out_vertices[6][1] = 0;      _out_vertices[6][2] = 0;
    _out_vertices[7][0] = 0;        _out_vertices[7][1] = 0;      _out_vertices[7][2] = 0;

    for (int i=0; i<6; i++)
        for (int j=0; j<3; j++)
            _out_normals[i][j] = normals_tmp[i][j];

    for (int i=0; i<6; i++)
        for (int j=0; j<4; j++)
            _out_faces[i][j] = faces_tmp[i][j];
}

void ViewerRaw::draw()
{
    for (size_t i=0; i<qvecvec_imgdata.size(); i++)
    {
        draw_group(qvecvec_imgdata.at(i), i);
    }
}

void ViewerRaw::drawWithNames()
{
    draw();
}

void ViewerRaw::draw_box(GLfloat _length, GLfloat _width, GLfloat _height)
{
    GLfloat vertices[8][3];
    GLfloat normals[6][3];
    GLint faces[6][4];

    compute_vertices(vertices, normals, faces, _length, _width, _height);
    for (int i = 0; i < 6; i++)
    {
        glBegin(GL_QUADS);
        glNormal3fv(&normals[i][0]);
        glVertex3fv(&vertices[faces[i][0]][0]);
        glVertex3fv(&vertices[faces[i][1]][0]);
        glVertex3fv(&vertices[faces[i][2]][0]);
        glVertex3fv(&vertices[faces[i][3]][0]);
        glEnd();
    }
}

void ViewerRaw::draw_box(GLuint _texture_name, GLint _texture_face, GLfloat _length, GLfloat _width, GLfloat _height, bool _label_drawn)
{
    GLfloat vertices[8][3];
    GLfloat normals[6][3];
    GLint faces[6][4];
    std::string str_label;

    // Get the label string
    if (_label_drawn==true)
    {
        for (QVector< QVector<PImgData>* >::iterator iter1 = qvecvec_imgdata.begin(); iter1!=qvecvec_imgdata.end(); iter1++)
        {
            QVector<PImgData>* vec_tmp = (*iter1);
            for (QVector<PImgData>::iterator iter2 = vec_tmp->begin(); iter2 != vec_tmp->end(); iter2++)
            {
                if ((*(*iter2).getTextureName()) == _texture_name)
                {
                    str_label = std::string((*iter2).getFilename());
                    char my_separator = (str_label.find('/')==string::npos?'\\':'/');
                    size_t my_last = str_label.find_last_of(my_separator);
                    if (my_last!=std::string::npos)
                    {
                        str_label = str_label.substr(my_last+1);
                        str_label = str_label.substr(str_label.size()/2);
                        str_label = std::string("...")+str_label;
                    }
                    break;
                }
            }
        }
    }

    compute_vertices(vertices, normals, faces, _length, _width, _height);
    glBindTexture(GL_TEXTURE_2D, _texture_name);

    for (int i = 0; i < 6; i++)
    {
        if (i == _texture_face)
        {
            if (_label_drawn)
            {
                QString qstr_label(str_label.c_str());
                glPushMatrix();
                glPushAttrib(GL_CURRENT_BIT);
                glColor4f(1.0f,0.0f,0.0f,1.0f);
                glTranslatef(0.0f, 1.0f, 0.0f);
                glTranslatef(0.0f, 0.0f, title_border*0.375f);
                glScalef(0.25f,0.25f,0.25f);
                text_3d(qstr_label);
                glPopAttrib();
                glPopMatrix();
            }
            glBegin(GL_QUADS);
            glNormal3fv(&normals[i][0]);
            glTexCoord2f(0.0f, 0.0f);glVertex3fv(&vertices[faces[i][0]][0]);
            glTexCoord2f(1.0f, 0.0f);glVertex3fv(&vertices[faces[i][1]][0]);
            glTexCoord2f(1.0f, 1.0f);glVertex3fv(&vertices[faces[i][2]][0]);
            glTexCoord2f(0.0f, 1.0f);glVertex3fv(&vertices[faces[i][3]][0]);
            glEnd();
        }
        else
        {
            glBegin(GL_QUADS);
            glNormal3fv(&normals[i][0]);
            glVertex3fv(&vertices[faces[i][0]][0]);
            glVertex3fv(&vertices[faces[i][1]][0]);
            glVertex3fv(&vertices[faces[i][2]][0]);
            glVertex3fv(&vertices[faces[i][3]][0]);
            glEnd();
        }
    }
}

int ViewerRaw::load_image_list(QString file_basename, QString _extension,
                               QStringList& _qsl_filename, QVector<QImage> &_qvec_img)
{
    int value=0;
    float x;
    QString tmp;
    PTextParser tp;

    _qsl_filename.clear();

    for (int i=0; i<NB_OUTPUT_IMG; i++)
    {
        x = static_cast<float>(i);
        tmp = file_basename;

        tmp+="_";
        tmp+=tp.float_to_stdstr(x).c_str();
        tmp+=".";
        tmp+=_extension;
        _qsl_filename << tmp;
        QFile f(tmp);
        if (f.exists())
        {
            _qvec_img.push_back(QGLWidget::convertToGLFormat(QImage(tmp)));
            value++;
        }
    }
    if (!_qvec_img.empty())
    {
        GLfloat ratio = 0.007;
        box_width = _qvec_img.back().width()*ratio;
        box_height = _qvec_img.back().height()*ratio;
        box_length = 0.1f;
    }

    return value;
}

//int ViewerRaw::load_image_list(QString _csv_filename,
//                    QVector<QImage>& _qvec_img)
//{
//    int value = 0;
//    QFile my_file;
//    QStringList my_vals;
//    QString my_data;
//    QVector< QVector<QString> > qvecvec_datas;
//    QVector<QString> qvec_str_tmp;
//    int csv_col = 4;
//    int i, j;

//    if (my_file.open(QIODevice::ReadOnly))
//    {
//        _qvec_img.clear();
//        //file opened successfully
//        QString my_data;
//        my_data = my_file.readAll();
//        my_data.replace('\n', ',');
//        my_vals = my_data.split(',');
//        my_file.close();
//        i=0;
//        while (i<my_vals.size())
//        {
//            qvec_str_tmp.clear();
//            for (j=0; (j<csv_col)&&(i<my_vals.size()); j++)
//            {
//                qvec_str_tmp.push_back(my_vals.at(i));
//                i++;
//            }
//            qvecvec_datas.push_back(qvec_str_tmp);
//        }
//        for (i=0; i<qvecvec_datas.size(); i++)
//        {
//            if (qvecvec_datas.at(i).at(3).toStdString().find(".png")!=std::string::npos)
//            {
//                QString qstr_tmp = qvecvec_datas.at(i).at(3);
//                QFile f(qstr_tmp);
//                if (f.exists())
//                {
//                    _qvec_img.push_back(QGLWidget::convertToGLFormat(QImage(qstr_tmp)));
//                    value++;
//                }
//            }
//        }
//    }
//    if (!_qvec_img.empty())
//    {
//        GLfloat ratio = 0.007;
//        box_width = _qvec_img.back().width()*ratio;
//        box_height = _qvec_img.back().height()*ratio;
//        box_length = 0.1f;
//    }
//    return value;
//}ttpv

int ViewerRaw::load_image_list(QString _csv_filename)
{
    int value = 0;
    QFile my_file(_csv_filename);
    QStringList my_vals, qsl_fn, qsl_grammar_fn;
    QString my_data;
    QVector< QVector<QString> > qvecvec_datas;
    QVector<QString> qvec_str_tmp;
    QVector<QImage> qvec_glimg;
    //QVector<QImage> qvec_img;
    QVector<PImgData>* my_imgdata;
    QImage my_img;
    int csv_col = 4;
    int i, j;
    //save file name to create later file to save information
    test_name=_csv_filename;
    if (my_file.open(QIODevice::ReadOnly))
    {
        //file opened successfully
        QString my_data;
        my_data = my_file.readAll();
        my_data.replace('\n', ',');
        my_vals = my_data.split(',');
        while (my_vals.back()=="")
            my_vals.pop_back();
        my_file.close();
        i=0;
        while (i<my_vals.size())
        {
            qvec_str_tmp.clear();
            for (j=0; (j<csv_col)&&(i<my_vals.size()); j++)
            {
                qvec_str_tmp.push_back(my_vals.at(i));
                i++;
            }
            qvecvec_datas.push_back(qvec_str_tmp);
        }

        for (i=0; i<qvecvec_datas.size(); i++)
        {
            if (qvecvec_datas.at(i).at(3).toStdString().find(".png")!=std::string::npos)
            {
                QString qstr_fntmp = qvecvec_datas.at(i).at(3);
                QString qstr_grammartmp = qvecvec_datas.at(i).at(1);
                QString path_tmp(Procedural::file_path(_csv_filename.toStdString()).c_str());
                QString path_separator(path_tmp.toStdString().find('/')==std::string::npos?"\\":"/");
                //QString work_path(QDir::currentPath());
                QDir::setCurrent(path_tmp);
                qstr_fntmp=path_tmp+path_separator+qstr_fntmp;
                qstr_grammartmp=path_tmp+path_separator+qstr_grammartmp;
                QFile f(qstr_fntmp);
                QFile fgrammar(qstr_grammartmp);
                //std::cout << std::endl << "Current Path : " << work_path.toStdString();
                //if (f.open(QIODevice::ReadOnly))
                if (f.exists()&&fgrammar.exists())
                {
                    qvec_glimg.push_back(QGLWidget::convertToGLFormat(QImage(qstr_fntmp)));
                    //qvec_img.push_back(QImage(qstr_fntmp));
                    qsl_fn << qstr_fntmp;
                    qsl_grammar_fn << qstr_grammartmp;
                    value++;
                } else std::cout << std::endl << "Failed to open \'" << qstr_fntmp.toStdString() << "\'";
            }
        }
        qvecvec_imgdata.clear();
    }
    else QMessageBox::warning(this, "Loading Status", "Failed to load file");
    if (!qvec_glimg.empty())
    {
        GLfloat ratio = 0.007;
        box_width = qvec_glimg.back().width()*ratio;
        box_height = qvec_glimg.back().height()*ratio;
        box_length = 0.1f;

        texture_names = new GLuint[NB_OUTPUT_IMG];
        glGenTextures(NB_OUTPUT_IMG, texture_names);

        my_imgdata = new QVector<PImgData>();


        QString tmp;
        int ind_ins = test_name.lastIndexOf("/");
        test_name= test_name.insert(ind_ins+1, "Resultats_Test/"+id+"_"+date_test+"_");
        ind_ins = test_name.lastIndexOf("/");
        QString dir_res =test_name.mid(0,ind_ins);
        if (!QDir(dir_res).exists())
        {
           QDir().mkdir(dir_res);
        }


        QFile* file_save = new QFile(test_name);
        std::cout << "Open save file :"+test_name.toStdString()+"\n"  << std::endl;


        if(file_save->open(QIODevice::ReadWrite | QIODevice::Truncate)) {
            //std::cout << "file open\n"  << std::endl;

            QString line;
            QString fn_main = qstr_csv_fn;
            int k = fn_main.lastIndexOf("/");
            fn_main=fn_main.mid(k+1);

            line ="Test:,"+ fn_main +"\n";
            file_save->write(line.toLocal8Bit());

            line ="User:"+id+",Date:"+ date_test +"\n";
            file_save->write(line.toLocal8Bit());

            line ="\n";
            file_save->write(line.toLocal8Bit());

            line ="Image,Groupe_number,Main_img_Group\n";
            file_save->write(line.toLocal8Bit());
            tmp = file_save->readAll();
            line="\n";
            int nbr_line=tmp.count(line)+4;

            for (int i=0; i<qvec_glimg.size(); i++)
            {
                my_img = qvec_glimg.at(i);
                glBindTexture(GL_TEXTURE_2D, texture_names[i]);
                int retvalue =
                        gluBuild2DMipmaps(GL_TEXTURE_2D,     // texture to specify
                                          GL_RGBA,           // internal texture storage format
                                          my_img.width(),     // texture width
                                          my_img.height(),    // texture height
                                          GL_RGBA,           // pixel format
                                          GL_UNSIGNED_BYTE,  // color component format
                                          my_img.bits());     // pointer to texture image
                //QGLWidget::convertToGLFormat(qvec_img.at(i)).bits());
                //Debug
                switch (retvalue)
                {
                case GLU_INVALID_VALUE:std::cout<<std::endl<<"GLU_INVALID_VALUE returned";break;
                case GLU_INVALID_ENUM:std::cout<<std::endl<<"GLU_INVALID_ENUM returned";break;
                case GLU_INVALID_OPERATION:std::cout<<std::endl<<"GLU_INVALID_OPERATION returned";break;
                }

                my_imgdata->push_back(PImgData(&texture_names[i], i, qsl_fn.at(i).toStdString(), qsl_grammar_fn.at(i).toStdString(), qvec_glimg.at(i), 0));

                QString n;
                n.setNum(nbr_line);
                //std::cout << "file open avant H"+n.toStdString()+"\n"  << std::endl;

                QString fn =qsl_fn.at(i);
                int k = fn.lastIndexOf("/");
                fn=fn.mid(k+1);
                //std::cout << "fn =" + fn.toStdString()+"\n"  << std::endl;

                //ajout de limage a la hashtable et ecriture dans file_save
                hash_save->insert(fn,nbr_line+i) ;
                //std::cout << "file open ap H\n"  << std::endl;

                //std::cout <<"add"+ fn.toStdString()+ tmp.setNum(nbr_line+i+1).toStdString()+"\n"<< std::endl;

                // add img in file_save in group 0
                QString group =",0,\n";
                QString to_write= fn;
                to_write +=group;
                // std::cout <<"WRITE :"+to_write.toStdString()+"\n"<< std::endl;

                file_save->write(to_write.toLocal8Bit());

            }
            file_save->close();

            qvecvec_imgdata.push_back(my_imgdata);
            for (int i=1; i<=9; i++)
            {
                qvecvec_imgdata.push_back(new QVector<PImgData>());
            }

            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR); // Linear Filtering
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // Linear Filtering
        }
        else QMessageBox::warning(this, "Loading Status", "Failed to load file to save information");
    }
    return value;
}

void ViewerRaw::postSelection(const QPoint& _point)
{
    bool found;
    camera()->convertClickToLine(_point, origin, direction);
    selected_point = camera()->pointUnderPixel(_point, found);
    selected_point -= 0.01f*direction;
    highlight_idx = selectedName();
//    if (highlight_idx==-1)
//        vec_selection.clear();
}

void ViewerRaw::draw_border(int _size, QString _str_group_name)
{
    QFont my_font;
    GLfloat color_tmp[4];
    GLfloat my_length = (box_width+snapshot_spacing*box_width)*nb_column+snapshot_spacing*box_width;
    GLfloat my_width = (box_height+snapshot_spacing*box_height)*((int)((_size/nb_column)+((_size%nb_column==0)?0:1))+snapshot_spacing*box_height);
    GLfloat my_height = 0.25f;

    GLfloat my_bar = 1.0f;
    my_width+=my_bar;

    GLfloat my_snapshot_spacing = snapshot_spacing*box_width;
    glGetFloatv(GL_CURRENT_COLOR,color_tmp);
    glPushMatrix();
    glColor4f(1.0f,1.0f,0.0f,1.0f);

    //text_glut(_str_group_name);
    text_3d(_str_group_name);

    glTranslatef(0.0f, -1.0f, 0.0f);
    glTranslatef(0.0f, 0.0f, -title_border);
    glTranslatef(-my_snapshot_spacing, 0.0f, -my_snapshot_spacing);

    glColor4f(0.0f,1.0f,0.0f,1.0f);
    draw_box(my_length, my_width, my_height);
    glTranslatef(0.0f, 1.0f, 0.0f);
    glColor4f(color_tmp[0], color_tmp[1], color_tmp[2], color_tmp[3]);
    glPopMatrix();
}

void ViewerRaw::draw_group(const QVector<PImgData>* _qvec_imgdata, unsigned int _group_id)
{
    glDisable(GL_LIGHTING);
    GLint my_face = 0;
    GLfloat my_length = box_width;
    GLfloat my_width = box_height;
    GLfloat my_height = 0.1f;

    if (name_drawn)
        glInitNames();

    //glPushMatrix();
    glPushName(GROUP_NAME_OFFSET);
    draw_border(_qvec_imgdata->size(), (_group_id==0)?QString("Main"):QString("Group %1").arg(_group_id));

    glPopName();

    // This next push is supposed to substitute the translation process
    // which occured in every column completed
    glPushMatrix();

    for (int i=0; i<_qvec_imgdata->size(); i++)
    {
        //The next instruction is planned to replace the above, with a concept of push/pop
        if ((i%nb_column)==0)
        {
            glPopMatrix();
            glTranslatef(0.0f, 0.0f, (i==0)?0.0f:my_width+0.05*my_width);
            glPushMatrix();
        }
        else
        {
            glTranslatef(my_length+0.05*my_length, 0.0f, 0.0f);
        }


        if (name_drawn)
            glPushName(_qvec_imgdata->at(i).getNameGl());
        if(name_drawn&&(is_selected_main(_qvec_imgdata->at(i).getNameGl())))
        {
            GLfloat color_tmp[4];
            glTranslatef(0.0f, -0.25f, 0.0f);
            glGetFloatv(GL_CURRENT_COLOR,color_tmp);
            glColor4f(0.0,0.0,1.0,1.0);
            glTranslatef(-highlighting_thickness, 0.0, -highlighting_thickness);
            draw_box(my_length+highlighting_thickness, my_width+highlighting_thickness, my_height+highlighting_thickness);
//            glTranslatef(0.0625f, 0.0, 0.0625f);
            glTranslatef(highlighting_thickness, 0.0, highlighting_thickness);
            glColor4f(color_tmp[0], color_tmp[1], color_tmp[2], color_tmp[3]);
            glTranslatef(0.0f, 0.25f, 0.0f);
        }

        if (name_drawn&&(is_selected(_qvec_imgdata->at(i).getNameGl())))
        {
            GLfloat color_tmp[4];
            glTranslatef(0.0f, -0.25f, 0.0f);
            glGetFloatv(GL_CURRENT_COLOR,color_tmp);
            glColor4f(1.0,0.0,0.0,1.0);
            glTranslatef(-highlighting_thickness/2, 0.0, -highlighting_thickness/2);
            draw_box(my_length+highlighting_thickness, my_width+highlighting_thickness, my_height+highlighting_thickness);
//            glTranslatef(0.0625f, 0.0, 0.0625f);
            glTranslatef(highlighting_thickness/2, 0.0, highlighting_thickness/2);
            glColor4f(color_tmp[0], color_tmp[1], color_tmp[2], color_tmp[3]);
            glTranslatef(0.0f, 0.25f, 0.0f);
        }
        glEnable(GL_TEXTURE_2D);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        draw_box(*_qvec_imgdata->at(i).getTextureName(), my_face, my_length, my_width, my_height, false);
        glDisable(GL_TEXTURE_2D);


        if (name_drawn)
            glPopName();
    }

    // Please check the paired Push instruction for more information
    glPopMatrix();

    glEnable(GL_LIGHTING);
    glTranslatef(0.0f, 0.0f, _qvec_imgdata->size()?2*my_width:my_width);
}

void ViewerRaw::text_glut(QString& _text)
{
    GLfloat my_scale = 0.01f;
//    GLdouble savelinewidth;  // Save current line width for later restoring
//    glGetDoublev(GL_LINE_WIDTH, &savelinewidth);
//    glLineWidth(100.0);
    glPushMatrix();
    glRotatef(90,-1.0f,0.0f,0.0f);
    glTranslatef(0.0f, title_border*0.1f, 0.0f);
    glScalef(my_scale,my_scale,my_scale);
    char *p;
    for (p=const_cast<char*>(_text.toStdString().c_str()); *p; p++)
        glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);
    glPopMatrix();
//    glLineWidth(savelinewidth);  // Restore line width
}

void ViewerRaw::text_3d(QString& _text)
{
    PText3D t3d(_text);
    QFont dfont("Comic Sans MS", 1);
    QFontMetrics fm(dfont);
    t3d.initfont(dfont,0.1f);
    glTranslatef(0.0f, 0.0f, -title_border*0.2f);
    t3d.print();
    glTranslatef(0.0f, 0.0f, title_border*0.2f);
}

void ViewerRaw::mousePressEvent(QMouseEvent* e)
{
    if ((e->button() == Qt::RightButton) && ((e->modifiers() == Qt::NoButton)||(e->modifiers() == Qt::ControlModifier)))
    {
        select(e);

        if (highlight_idx!=-1)
        {
            if (is_selected(highlight_idx))
                contextual->exec(e->globalPos());
            else
            {
                add_selection(highlight_idx, e->modifiers() == Qt::ControlModifier?false:true);
            }
        }
        else QGLViewer::mousePressEvent(e);
    }
    else if ((e->button() == Qt::LeftButton) && ((e->modifiers() == Qt::NoButton)||(e->modifiers() == Qt::ControlModifier)))
    {
        select(e);
        add_selection(highlight_idx, e->modifiers() == Qt::ControlModifier?false:true);
    }
    else
    {
        QGLViewer::mousePressEvent(e);
    }
}

void ViewerRaw::move_to(unsigned int _group_id)
{
    int src_grpid, dest_grpid;
    PImgData my_imgdt;
    QVector<PImgData> *vec_dest_imgdt, *vec_src_imgdt;
    try
    {
        my_imgdt = select_imgdt(highlight_idx);
        src_grpid = my_imgdt.getGroupId();
        dest_grpid = _group_id;


        if (    (dest_grpid>=0)&&(dest_grpid<qvecvec_imgdata.size())
                &&(src_grpid>=0)&&(src_grpid<qvecvec_imgdata.size())
           )
        {
            for (QVector<PImgData>::iterator iter = qvecvec_imgdata.at(src_grpid)->begin(); iter!=qvecvec_imgdata.at(src_grpid)->end(); iter++)
            {
                if ((*iter).getNameGl() == my_imgdt.getNameGl())
                {
                    qvecvec_imgdata.at(src_grpid)->erase(iter);
                    break;
                }
            }
            my_imgdt.setGroupId(dest_grpid);
            qvecvec_imgdata.at(dest_grpid)->push_back(my_imgdt);
        }
        else if (dest_grpid==qvecvec_imgdata.size())
        {
            vec_src_imgdt  = qvecvec_imgdata.at(src_grpid);
            for (QVector<PImgData>::iterator iter = vec_src_imgdt->begin(); iter!=vec_src_imgdt->end(); iter++)
            {
                if ((*iter).getNameGl() == my_imgdt.getNameGl())
                {
                    vec_src_imgdt->erase(iter);
                    break;
                }
            }
            my_imgdt.setGroupId(dest_grpid);
            vec_dest_imgdt = qvecvec_imgdata.at(dest_grpid);
            vec_dest_imgdt->push_back(my_imgdt);
        }
    }
    catch(...)
    {
        QMessageBox::warning(this, "Moving Failure", "Failed to fetch to specified group!" );
    }
    updateGL();
}

void ViewerRaw::multimove_to(unsigned int _group_id)
{
    int src_grpid, dest_grpid;
    PImgData my_imgdt;
    std::vector<PImgData*> vec_pimgdt;
    std::vector<PImgData> vec_imgdt;
    QVector<PImgData> *vec_dest_imgdt, *vec_src_imgdt;
    try
    {
        dest_grpid = _group_id;
        vec_pimgdt = multiselect_imgdt(vec_selection);

        for (std::vector<PImgData*>::iterator iter_pimgdt = vec_pimgdt.begin(); iter_pimgdt!=vec_pimgdt.end(); iter_pimgdt++)
            vec_imgdt.push_back(**iter_pimgdt);

        for (std::vector<PImgData>::iterator iter_imgdt = vec_imgdt.begin(); iter_imgdt!=vec_imgdt.end(); iter_imgdt++)
        {
            src_grpid = (*iter_imgdt).getGroupId();

            if (    (dest_grpid>=0)&&(dest_grpid<qvecvec_imgdata.size())
                    &&(src_grpid>=0)&&(src_grpid<qvecvec_imgdata.size())
               )
            {
                for (QVector<PImgData>::iterator iter = qvecvec_imgdata.at(src_grpid)->begin(); iter!=qvecvec_imgdata.at(src_grpid)->end(); iter++)
                {
                    if ((*iter).getNameGl() == (*iter_imgdt).getNameGl())
                    {
                        my_imgdt = (*iter);
                        my_imgdt.setGroupId(dest_grpid);
                        my_imgdt.setMainImg(false);
                        if (is_selected_main(my_imgdt.getNameGl()))
                        {
                            vec_main_img.erase(std::find(vec_main_img.begin(), vec_main_img.end(), my_imgdt.getNameGl()));
                        }
                        qvecvec_imgdata.at(src_grpid)->erase(iter);
                        qvecvec_imgdata.at(dest_grpid)->push_back(my_imgdt);
                        break;
                    }
                }
            }

            // change id group of the image in save file
            QString fn = QString(my_imgdt.getFilename().c_str());
            int k = fn.lastIndexOf("/");
            QString img_name=fn.mid(k+1);

            //std::cout <<"Before saving image :"+ img_name.toStdString()+ "\n"<< std::endl;

            QFile::rename(test_name,"tmp.csv");
            QFile* file_save = new QFile(test_name);
            QFile* file_tmp = new QFile("tmp.csv");

            if(file_save->open(QIODevice::WriteOnly | QIODevice::Truncate) && file_tmp->open(QIODevice::ReadOnly)){

                int n_line = hash_save->value(img_name);

                QString line;
                line.setNum(n_line);
                //std::cout <<"num de ligne:"+line.toStdString()<< std::endl;
                for(int i=0; i<n_line;i++){
                    file_save->write(file_tmp->readLine());
                }

                unsigned int nb_bytesr=0;
                char tmp[img_name.length()];
                while (nb_bytesr!=img_name.length()+1){
                      nb_bytesr+=file_tmp->read(tmp,img_name.length()+1-nb_bytesr);
                      QString nb;
                      nb.setNum(nb_bytesr);

                      //std::cout <<nb.toStdString()<< std::endl;

                }

                file_tmp->readLine();
                //std::cout <<"Lecture de la ligne selectionné = " << tmp << std::endl;
                QString num;
                num.setNum(_group_id);

                line=tmp;
                line.resize(img_name.length()+1);
                line+=num;
                //line=line.remove(QChar(''));
                line+=",\n";
                //std::cout <<line.toStdString()<< std::endl;
                file_save->write(line.toLocal8Bit());



                file_save->write(file_tmp->readAll());
                if(file_tmp->remove())
                    {}
                else
                    QMessageBox::warning(this, "Loading Status", "Failed to delete file tmp");
                file_save->close();
                file_tmp->close();
                std::cout <<"Saved"<< std::endl;

            }
            else QMessageBox::warning(this, "Loading Status", "Failed to load file to save information");

        }
    }
    catch(...)
    {
        QMessageBox::warning(this, "Moving Failure", "Failed to fetch to specified group!" );
    }
    updateGL();
}


void ViewerRaw::main_tree()
{
    int src_grpid;
    PImgData* my_imgdt,old_img;
    std::vector<PImgData*> vec_imgdt;
    QVector<PImgData> *vec_dest_imgdt, *vec_src_imgdt;
    bool is_old= false ;
    try
    {
        vec_imgdt = multiselect_imgdt(vec_selection);
        if (vec_imgdt.size()>1)
            QMessageBox::warning(this, "MAin tree failure ", "Failed multiselection not allowed" );
        else {
            my_imgdt=*vec_imgdt.begin();
            src_grpid = my_imgdt->getGroupId();
            if ( (src_grpid>0)&&(src_grpid<qvecvec_imgdata.size()) )
                {
                    for (QVector<PImgData>::iterator iter = qvecvec_imgdata.at(src_grpid)->begin(); iter!=qvecvec_imgdata.at(src_grpid)->end(); iter++)
                    {
                        if ((*iter).getNameGl() == my_imgdt->getNameGl())
                        {
                            (*iter).setMainImg(true);
                            add_main_img(my_imgdt->getNameGl());
                        }
                        else {
                            if((*iter).isMainImg())
                            {
                                is_old = true;
                                old_img = *iter;
                                vec_main_img.erase(std::find(vec_main_img.begin(), vec_main_img.end(), (*iter).getNameGl()));
                            }
                            (*iter).setMainImg(false);
                        }
                    }


                // change main img in save file
                QString fn = QString(my_imgdt->getFilename().c_str());
                int k = fn.lastIndexOf("/");
                QString img_name=fn.mid(k+1);

                std::cout <<"Before saving image :"+ img_name.toStdString()+"\n"<< std::endl;

                QFile::rename(test_name,"tmp.csv");
                QFile* file_save = new QFile(test_name);
                QFile* file_tmp = new QFile("tmp.csv");

                if(file_save->open(QIODevice::WriteOnly | QIODevice::Truncate) && file_tmp->open(QIODevice::ReadOnly)){

                    int n_line = hash_save->value(img_name);

                    QString line;
                    line.setNum(n_line);
                    //std::cout <<"num de ligne:"+line.toStdString()<< std::endl;
                    for(int i=0; i<n_line;i++){
                        file_save->write(file_tmp->readLine());
                    }
                    QString num;
                    num.setNum(src_grpid);

                    line=file_tmp->readLine();
                    //std::cout <<"Lecture de la ligne selectionné = " << tmp << std::endl;
                    line.resize(line.lastIndexOf(',')+1);
                    line += "main_tree_of_group_" + num;
                    line +="\n";
                    std::cout <<"Lecture de la ligne selectionné = " << line.toStdString() << std::endl;
                    file_save->write(line.toLocal8Bit());
                    file_save->write(file_tmp->readAll());
                    if(file_tmp->remove())
                        {}
                    else
                        QMessageBox::warning(this, "Loading Status", "Failed to delete file tmp");
                    file_save->close();
                    file_tmp->close();
                }
                else QMessageBox::warning(this, "Loading Status", "Failed to load file to save information");

                if(is_old)
                {
                    QString old_fn = QString(old_img.getFilename().c_str());
                    int old_k = old_fn.lastIndexOf("/");
                    QString old_img_name=old_fn.mid(k+1);
                    std::cout <<"Before saving old image :"+ old_img_name.toStdString()+"\n"<< std::endl;

                    QFile::rename(test_name,"tmp.csv");
                    file_save = new QFile(test_name);
                    file_tmp = new QFile("tmp.csv");

                    if(file_save->open(QIODevice::WriteOnly | QIODevice::Truncate) && file_tmp->open(QIODevice::ReadOnly)){

                        int n_line = hash_save->value(old_img_name);

                        QString line;
                        line.setNum(n_line);
                        //std::cout <<"num de ligne:"+line.toStdString()<< std::endl;
                        for(int i=0; i<n_line;i++){
                            file_save->write(file_tmp->readLine());
                        }
                        line=file_tmp->readLine();
                        //std::cout <<"Lecture de la ligne selectionné = " << tmp << std::endl;
                        line.resize(line.lastIndexOf(',')+1);
                        line += "";
                        line+="\n";
                        std::cout <<"Lecture de la ligne selectionné = " << line.toStdString() << std::endl;
                        file_save->write(line.toLocal8Bit());
                        file_save->write(file_tmp->readAll());
                        if(file_tmp->remove())
                        {}
                        else
                            QMessageBox::warning(this, "Loading Status", "Failed to delete file tmp");
                        file_save->close();
                        file_tmp->close();


                    }
                    else QMessageBox::warning(this, "Loading Status", "Failed to load file to save information");

                }
                std::cout <<"end saving"<< std::endl;
            }
            else QMessageBox::warning(this, "MAin tree failure ", "Failed group not allowed" );
        }
    }
    catch(...)
    {

    }
    updateGL();
}



PImgData& ViewerRaw::select_imgdt(unsigned int _gl_id)
{
    for (QVector< QVector<PImgData>* >::iterator iter1 = qvecvec_imgdata.begin(); iter1 != qvecvec_imgdata.end(); iter1++)
    {
        for (QVector<PImgData>::iterator iter2 = (*iter1)->begin(); iter2 != (*iter1)->end(); iter2++)
        {
            if ((*iter2).getNameGl() == _gl_id)
                return (*iter2);
        }
    }
    return qvecvec_imgdata.front()->front();
}

std::vector<PImgData*> ViewerRaw::multiselect_imgdt(std::vector<int> _vec_slctn)
{
    std::vector<PImgData*> value;
    for (QVector< QVector<PImgData>* >::iterator iter1 = qvecvec_imgdata.begin(); iter1 != qvecvec_imgdata.end(); iter1++)
    {
        for (QVector<PImgData>::iterator iter2 = (*iter1)->begin(); iter2 != (*iter1)->end(); iter2++)
        {
            if (is_selected((*iter2).getNameGl()))
                value.push_back(&*iter2);
        }
    }
    return value;
}

bool ViewerRaw::is_selected(int _elt_idx)
{
    return std::find(vec_selection.begin(), vec_selection.end(), _elt_idx)!=vec_selection.end()?true:false;
}

bool ViewerRaw::is_selected_main(int _elt_idx)
{
    return std::find(vec_main_img.begin(), vec_main_img.end(), _elt_idx)!=vec_main_img.end()?true:false;
}

void ViewerRaw::add_selection(int _idx, bool _reset)
{
    std::vector<int>::iterator iter_found;
    if (_reset==true)
        vec_selection.clear();
    if (_idx>=0)
    {
        iter_found = std::find(vec_selection.begin(), vec_selection.end(), _idx);
        if (iter_found!=vec_selection.end())
            vec_selection.erase(iter_found);
        else vec_selection.push_back(_idx);
    }
}
