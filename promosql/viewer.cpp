#include "viewer.h"
#include "putils.h"
#include "ptext3d.h"
#include "pboundingbox.h"
#include "main.h"
#include "QGLViewer/manipulatedCameraFrame.h"
#include <QStringListIterator>
#include <fstream>
#include <boost/algorithm/string.hpp>

using namespace qglviewer;
using namespace std;

Viewer::Viewer()
{
}

Viewer::Viewer(int type, QWidget* parent, const QGLWidget* shareWidget, Qt::WindowFlags flags)
    : QGLViewer(parent, shareWidget, flags)
{
    if (type < 3)
    {
      // Move camera according to viewer type (on X, Y or Z axis)
      camera()->setPosition(Vec((type==0)? 1.0 : 0.0, (type==1)? 1.0 : 0.0, (type==2)? 1.0 : 0.0));
      camera()->lookAt(sceneCenter());

      camera()->setType(Camera::ORTHOGRAPHIC);
      camera()->showEntireScene();

      // Forbid rotation
      WorldConstraint* constraint = new WorldConstraint();
      constraint->setRotationConstraintType(AxisPlaneConstraint::FORBIDDEN);
      camera()->frame()->setConstraint(constraint);
    }
}

Viewer::~Viewer()
{
    for (std::vector<PText3D*>::iterator iter = vec_t3d.begin(); iter!=vec_t3d.end(); iter++)
        delete (*iter);
            
}

void Viewer::draw()
{
    GLfloat textcolor[4]={1.0f,1.0f,0.0f,1.0f};

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glDisable(GL_LIGHTING);
    update_orientation();
    if (      (box_orientation!=REAL_PTS)
            &&(box_orientation!=REAL_GAP)
            &&(is_allqlfrs_drawn==false)
            &&(vec_obb.empty())
              )
        draw_imgwire();
    if (is_allqlfrs_drawn==false && vec_obb.empty())
    {
        for (int i=0; i<vecs_imgid.size(); i++)
        {
            QString qs = QString::number(i+1);

            glPushMatrix();
            glTranslatef((-0.45f)*idxr_fontsize*qs.size(), (-0.5f)*idxr_fontsize, 0.0f);

            glTranslatef(vecs_imgid[i][0], vecs_imgid[i][1], vecs_imgid[i][2]);
            glRotatef(90.0f, 1.0f, 0.0f, 0.0f);

            drawtext(qs, idxr_fontsize);
            glPopMatrix();
            drawsphere(vecs_imgid[i], idxr_radius);
        }
    }

    if (is_realtarget_drawn==true)
    {
        GLfloat sphrcolor[4] = {0.0f, 1.0f, 0.0f, 0.25f};
        GLfloat dgtcolor[4] = {0.0f, 0.0f, 1.0f, 0.25f};
        GLfloat linecolor[4] = {1.0f, 1.0f, 1.0f, 0.5f};
        GLfloat linecolor_res[4] = {0.99f, 0.01f, 0.6f, 0.9f};
        GLfloat linecolor_res2[4] = {0.01f, 0.99f, 0.6f, 0.9f};
        GLfloat linecolor_res3[4] = {0.99f, 0.39f, 0.01f, 0.9f};
        GLfloat linecolor_resman[4] = {0.995f, 0.0f, 0.0f, 0.9f};
        GLfloat dist_tmp0 = 0, dist_tmp1 = 0, dist_tmp2 = 0, dist_tmp3 = 0;
        for (int i=0; i<vecs_absoluteimgid.size(); i++)
        {
            QString qs = QString::number(i+1);

            glPushMatrix();
            glTranslatef((-0.45f)*idxr_fontsize*qs.size(), (-0.5f)*idxr_fontsize, 0.0f);

            glTranslatef(vecs_absoluteimgid[i][0], vecs_absoluteimgid[i][1], vecs_absoluteimgid[i][2]);
            glRotatef(90.0f, 1.0f, 0.0f, 0.0f);

            drawtext(qs, dgtcolor, idxr_fontsize);
            glPopMatrix();
            drawsphere(vecs_absoluteimgid[i], idxr_radius, sphrcolor);

            if (vecs_absoluteimgid.size()==vecs_imgid.size())
            {
                drawdashedline(vecs_absoluteimgid[i], vecs_imgid[i], linecolor);
                dist_tmp0 += euclidist(vecs_absoluteimgid[i], vecs_imgid[i]);
            }
            if (vec_absolresampld1.size()==IMAGE_NB&&dash_method==DASH_METHOD1)
            {
                drawdashedline(vecs_absoluteimgid[vec_absolresampld1[i]], vecs_imgid[i], linecolor_res);
                dist_tmp1 += euclidist(vecs_absoluteimgid[vec_absolresampld1[i]], vecs_imgid[i]);
            }
            if (vec_absolresampld2.size()==IMAGE_NB&&dash_method==DASH_METHOD2)
            {
                drawdashedline(vecs_absoluteimgid[i], vecs_imgid[vec_absolresampld2[i]], linecolor_res2);
                dist_tmp2 += euclidist(vecs_absoluteimgid[i], vecs_imgid[vec_absolresampld2[i]]);
            }
            if (vec_absolresampld3.size()==IMAGE_NB&&dash_method==DASH_METHOD3)
            {
                drawdashedline(vecs_imgid[i], vecs_absoluteimgid[vec_absolresampld3[i]], linecolor_res3);
                dist_tmp3 += euclidist(vecs_absoluteimgid[i], vecs_imgid[vec_absolresampld3[i]]);
            }
            if (vec_absolresampld_man.size()==IMAGE_NB&&dash_method==DASH_MANUAL)
            {
                drawdashedline(vecs_imgid[i], vecs_absoluteimgid[vec_absolresampld_man[i]-1], linecolor_resman);
            }
        }
        std::cout << std::endl << "dist0 = " << dist_tmp0 << "\t"<< "dist1 = " << dist_tmp1 << "\tdist2 = " << dist_tmp2 << "\tdist3 = " << dist_tmp3;
    }

    if (is_scattered_drawn==true)
    {
        GLfloat sphrcolor[4] = {0.4f, 0.0f, 0.8f, 0.5f};
        for (int i=0; i<vecs_scatterplot.size(); i++)
        {
            drawsphere(vecs_scatterplot[i], idxr_radius-0.5, sphrcolor);
        }
    }

    if (is_qualifier_drawn==true)
        draw_qlfrs();
    if (is_allqlfrs_drawn==true)
        draw_allqlfrs();

    draw_usefn();
    draw_diff();

    if (!vec_obb.empty() && is_obb_drawn==true)
    {
        GLfloat sphrcolor[4] = {0.4f, 0.0f, 0.8f, 0.5f};
        std::vector< std::vector< std::vector<GLfloat> > >::iterator iter_obbclouds;
        std::vector< std::vector<float> >::iterator iter_rgb;
        std::vector<PBoundingBox>::iterator iter_obb;
        std::vector<bool>::iterator iter_displayobb;
        std::vector< std::vector<float> > vecs_rgb = ::rgb_colorgenerator(vec_obb.size());
        iter_rgb = vecs_rgb.begin();
        iter_obb = vec_obb.begin();
        iter_obbclouds  = vecs_obb_clouds.begin();
        iter_displayobb = vec_obbdisplayed.begin();
        while (iter_obb != vec_obb.end() && iter_rgb != vecs_rgb.end() && iter_displayobb != vec_obbdisplayed.end())
        {
            if ((*iter_rgb).size()<3)
                continue;
            if ((*iter_displayobb)==true)
            {
                draw_obb(*iter_obb, (*iter_rgb)[0], (*iter_rgb)[1], (*iter_rgb)[2]);
                if (vecs_obb_clouds.size() == vec_obb.size())
                {
                    if (iter_obbclouds != vecs_obb_clouds.end())
                    {
                        for (std::vector< std::vector<GLfloat> >::iterator iter_pts = (*iter_obbclouds).begin(); iter_pts != (*iter_obbclouds).end(); iter_pts++)
                            drawsphere(*iter_pts, idxr_radius-1.5, (*iter_rgb)[0], (*iter_rgb)[1], (*iter_rgb)[2]);
                    }
                }
            }
            iter_obb++;
            iter_rgb++;
            iter_displayobb++;
            iter_obbclouds++;
        }
    }

    if (!vec_obb_resampled.empty() && is_obbresampled_drawn == true)
    {
        GLfloat sphrcolor[4] = {0.4f, 0.0f, 0.8f, 0.5f};
        std::vector< std::vector< std::vector<GLfloat> > >::iterator iter_obbclouds;
        std::vector< std::vector<float> >::iterator iter_rgb;
        std::vector<PBoundingBox>::iterator iter_obb;
        std::vector<bool>::iterator iter_displayobb;
        std::vector< std::vector<float> > vecs_rgb = ::rgb_colorgenerator(vec_obb.size());
        iter_rgb = vecs_rgb.begin();
        iter_obb = vec_obb_resampled.begin();
        iter_obbclouds  = vecs_obb_cloudsresampled.begin();
        iter_displayobb = vec_obbdisplayed.begin();
        while (iter_obb != vec_obb_resampled.end() && iter_rgb != vecs_rgb.end() && iter_displayobb != vec_obbdisplayed.end())
        {
            if ((*iter_rgb).size()<3)
                continue;
            if ((*iter_displayobb)==true)
            {
                draw_obb(*iter_obb, (*iter_rgb)[0], (*iter_rgb)[1], (*iter_rgb)[2]);
                if (vecs_obb_cloudsresampled.size() == vec_obb_resampled.size())
                {
                    if (iter_obbclouds != vecs_obb_cloudsresampled.end())
                    {
                        for (std::vector< std::vector<GLfloat> >::iterator iter_pts = (*iter_obbclouds).begin(); iter_pts != (*iter_obbclouds).end(); iter_pts++)
                            drawsphere(*iter_pts, idxr_radius-1.5, (*iter_rgb)[0], (*iter_rgb)[1], (*iter_rgb)[2]);
                    }
                }
            }
            iter_obb++;
            iter_rgb++;
            iter_displayobb++;
            iter_obbclouds++;
        }
    }

    glPopMatrix();
}

void Viewer::init()
{
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    setGridIsDrawn(true);
    setAxisIsDrawn(true);
    setSceneRadius(120+50+10);
    showEntireScene();

    idxr_radius = 6.0f;
    idxr_fontsize = 5;
    is_realtarget_drawn = false;
    is_qualifier_drawn = true;
    is_scattered_drawn = false;
    is_allqlfrs_drawn  = false;


    init_extremums();
    box_orientation = STD_ORIENT;
    dash_method = DISABLE_DASHD;
    init_imgids(); // Please add your custom function BELOW, and NOT ABOVE the function
    //init_imgt3ds();

    bgcolor_default = backgroundColor();
    is_obb_drawn = true;
    is_obbresampled_drawn = false;
}

void Viewer::init_extremums()
{
    step = 100.0f;
    xmin=ymin=zmin=-step;
    xmax=ymax=zmax= step;
}

//void Viewer::init_extremums(std::vector<float>& _vec_extremum)
//{
//    if (_vec_extremum.size()>=6)
//    {
//        absolutexmin=_vec_extremum[0];absoluteymin=_vec_extremum[1];absolutezmin=_vec_extremum[2];
//        absolutexmax=_vec_extremum[3];absoluteymax=_vec_extremum[4];absolutezmax=_vec_extremum[5];
//    }
//}


//float Viewer::scalepca(float _x)
//{
//    return (_x*(absolutexmax-absolutexmin)/step);
//}

void Viewer::init_imgids(bool _draw_real)
{
    std::vector<GLfloat> vec_tmp;

    if (_draw_real==true)
    {
        xmin = absolutexmin;
        ymin = absoluteymin;
        zmin = absolutezmin;
        xmax = absolutexmax;
        ymax = absoluteymax;
        zmax = absolutezmax;
    }
    else
    {
        init_extremums();
    }
    GLfloat xmid = 0.5*(xmin+xmax);
    GLfloat ymid = 0.5*(ymin+ymax);
    GLfloat zmid = 0.5*(zmin+zmax);

    vecs_imgid.clear();
    vec_tmp.push_back(xmin);vec_tmp.push_back(ymin);vec_tmp.push_back(zmin);vecs_imgid.push_back(vec_tmp);vec_tmp.clear();
    vec_tmp.push_back(xmin);vec_tmp.push_back(ymid);vec_tmp.push_back(zmin);vecs_imgid.push_back(vec_tmp);vec_tmp.clear();
    vec_tmp.push_back(xmin);vec_tmp.push_back(ymax);vec_tmp.push_back(zmin);vecs_imgid.push_back(vec_tmp);vec_tmp.clear();

    vec_tmp.push_back(xmin);vec_tmp.push_back(ymin);vec_tmp.push_back(zmid);vecs_imgid.push_back(vec_tmp);vec_tmp.clear();
    vec_tmp.push_back(xmin);vec_tmp.push_back(ymid);vec_tmp.push_back(zmid);vecs_imgid.push_back(vec_tmp);vec_tmp.clear();
    vec_tmp.push_back(xmin);vec_tmp.push_back(ymax);vec_tmp.push_back(zmid);vecs_imgid.push_back(vec_tmp);vec_tmp.clear();

    vec_tmp.push_back(xmin);vec_tmp.push_back(ymin);vec_tmp.push_back(zmax);vecs_imgid.push_back(vec_tmp);vec_tmp.clear();
    vec_tmp.push_back(xmin);vec_tmp.push_back(ymid);vec_tmp.push_back(zmax);vecs_imgid.push_back(vec_tmp);vec_tmp.clear();
    vec_tmp.push_back(xmin);vec_tmp.push_back(ymax);vec_tmp.push_back(zmax);vecs_imgid.push_back(vec_tmp);vec_tmp.clear();

    vec_tmp.push_back(xmid);vec_tmp.push_back(ymin);vec_tmp.push_back(zmin);vecs_imgid.push_back(vec_tmp);vec_tmp.clear();
    vec_tmp.push_back(xmid);vec_tmp.push_back(ymid);vec_tmp.push_back(zmin);vecs_imgid.push_back(vec_tmp);vec_tmp.clear();
    vec_tmp.push_back(xmid);vec_tmp.push_back(ymax);vec_tmp.push_back(zmin);vecs_imgid.push_back(vec_tmp);vec_tmp.clear();

    vec_tmp.push_back(xmid);vec_tmp.push_back(ymin);vec_tmp.push_back(zmid);vecs_imgid.push_back(vec_tmp);vec_tmp.clear();
    vec_tmp.push_back(xmid);vec_tmp.push_back(ymid);vec_tmp.push_back(zmid);vecs_imgid.push_back(vec_tmp);vec_tmp.clear();
    vec_tmp.push_back(xmid);vec_tmp.push_back(ymax);vec_tmp.push_back(zmid);vecs_imgid.push_back(vec_tmp);vec_tmp.clear();

    vec_tmp.push_back(xmid);vec_tmp.push_back(ymin);vec_tmp.push_back(zmax);vecs_imgid.push_back(vec_tmp);vec_tmp.clear();
    vec_tmp.push_back(xmid);vec_tmp.push_back(ymid);vec_tmp.push_back(zmax);vecs_imgid.push_back(vec_tmp);vec_tmp.clear();
    vec_tmp.push_back(xmid);vec_tmp.push_back(ymax);vec_tmp.push_back(zmax);vecs_imgid.push_back(vec_tmp);vec_tmp.clear();

    vec_tmp.push_back(xmax);vec_tmp.push_back(ymin);vec_tmp.push_back(zmin);vecs_imgid.push_back(vec_tmp);vec_tmp.clear();
    vec_tmp.push_back(xmax);vec_tmp.push_back(ymid);vec_tmp.push_back(zmin);vecs_imgid.push_back(vec_tmp);vec_tmp.clear();
    vec_tmp.push_back(xmax);vec_tmp.push_back(ymax);vec_tmp.push_back(zmin);vecs_imgid.push_back(vec_tmp);vec_tmp.clear();

    vec_tmp.push_back(xmax);vec_tmp.push_back(ymin);vec_tmp.push_back(zmid);vecs_imgid.push_back(vec_tmp);vec_tmp.clear();
    vec_tmp.push_back(xmax);vec_tmp.push_back(ymid);vec_tmp.push_back(zmid);vecs_imgid.push_back(vec_tmp);vec_tmp.clear();
    vec_tmp.push_back(xmax);vec_tmp.push_back(ymax);vec_tmp.push_back(zmid);vecs_imgid.push_back(vec_tmp);vec_tmp.clear();

    vec_tmp.push_back(xmax);vec_tmp.push_back(ymin);vec_tmp.push_back(zmax);vecs_imgid.push_back(vec_tmp);vec_tmp.clear();
    vec_tmp.push_back(xmax);vec_tmp.push_back(ymid);vec_tmp.push_back(zmax);vecs_imgid.push_back(vec_tmp);vec_tmp.clear();
    vec_tmp.push_back(xmax);vec_tmp.push_back(ymax);vec_tmp.push_back(zmax);vecs_imgid.push_back(vec_tmp);vec_tmp.clear();

    if (_draw_real==false)
        vecs_std_imgid = vecs_imgid;
}

void Viewer::drawline(std::vector<GLfloat>& _pt1,std::vector<GLfloat>& _pt2)
{
    glPushAttrib(GL_CURRENT_BIT);
    glColor3f(1.0,0.0,0.0);

    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex3f(_pt1[0], _pt1[1], _pt1[2]);
    glVertex3f(_pt2[0], _pt2[1], _pt2[2]);
    glEnd();

    glPopAttrib();
}

void Viewer::drawline(std::vector<GLfloat>& _pt1,std::vector<GLfloat>& _pt2, GLfloat* _color)
{
    glPushAttrib(GL_CURRENT_BIT);
    glColor4fv(_color);

    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex3f(_pt1[0], _pt1[1], _pt1[2]);
    glVertex3f(_pt2[0], _pt2[1], _pt2[2]);
    glEnd();

    glPopAttrib();
}

void Viewer::drawdashedline(std::vector<GLfloat>& _pt1,std::vector<GLfloat>& _pt2, GLfloat* _color)
{
    glPushAttrib(GL_CURRENT_BIT);
    glColor4fv(_color);

    glEnable (GL_LINE_STIPPLE);
    glLineStipple (1, 0x0101);  /*  dotted  */
    glBegin(GL_LINES);
    glVertex3f(_pt1[0], _pt1[1], _pt1[2]);
    glVertex3f(_pt2[0], _pt2[1], _pt2[2]);
    glEnd();
    glDisable (GL_LINE_STIPPLE);

    glPopAttrib();
}

void Viewer::drawbox(std::vector< std::vector<GLfloat> >& _corners)
{
    if (_corners.size()!=8)
    {
        std::cout << std::endl << "Drawing Error: Requesting to draw box with an incorrect number of vertices.";
    }
    else
    {
        drawline(_corners[0], _corners[1]);
        drawline(_corners[1], _corners[2]);
        drawline(_corners[2], _corners[3]);
        drawline(_corners[3], _corners[0]);

        drawline(_corners[4], _corners[5]);
        drawline(_corners[5], _corners[6]);
        drawline(_corners[6], _corners[7]);
        drawline(_corners[7], _corners[4]);

        drawline(_corners[0], _corners[4]);
        drawline(_corners[1], _corners[5]);
        drawline(_corners[2], _corners[6]);
        drawline(_corners[3], _corners[7]);
    }
}

void Viewer::drawpt3d(std::vector<float>& _vec3)
{
    if (_vec3.size()==3 || _vec3.size()==2)
    {
        glPushAttrib(GL_CURRENT_BIT);

        glColor3f(0.117, 0.564, 1.0);
        glPointSize(6);
        glBegin(GL_POINTS);
        (_vec3.size()==3)?glVertex3f(_vec3[0], _vec3[1], _vec3[2]):glVertex3f(_vec3[0], _vec3[1], 0);
        glEnd();

        glPopAttrib();
    }
}

void Viewer::drawtext(QString& _text, int _size)
{
    int font_size=_size;
    std::vector<PText3D*>::iterator iter;
    for (iter = vec_t3d.begin(); iter!=vec_t3d.end(); iter++)
    {
        if (((*iter)->gettext().compare(_text)==0)&&((*iter)->getfont()->pointSize() == font_size) )
            break;
    }
    if (iter==vec_t3d.end())
    {
        PText3D* t3d = new PText3D(_text);
        QFont dfont("Comic Sans MS", font_size);
        QFontMetrics fm(dfont);
        t3d->initfont(dfont,1.0f);

        vec_t3d.push_back(t3d);
    }
    else
    {
        glTranslatef(0.0f, 0.5f, 0.0f);
        (*iter)->print();
        glTranslatef(0.0f, -0.5f, 0.0f);
    }
}

void Viewer::drawtext(QString& _text, GLfloat* _color, int _size)
{
    int font_size=_size;
    std::vector<PText3D*>::iterator iter;
    for (iter = vec_t3d.begin(); iter!=vec_t3d.end(); iter++)
    {
        if ( ((*iter)->gettext().compare(_text)==0)
             &&((*iter)->getfont()->pointSize() == font_size)
             &&((*iter)->equalcolor(_color)==true))
            break;
    }
    if (iter==vec_t3d.end())
    {
        PText3D* t3d = new PText3D(_text);
        t3d->setcolor(_color);
        QFont dfont("Comic Sans MS", font_size);
        QFontMetrics fm(dfont);
        t3d->initfont(dfont,1.0f);

        vec_t3d.push_back(t3d);
    }
    else
    {
        glTranslatef(0.0f, 0.5f, 0.0f);
        (*iter)->print();
        glTranslatef(0.0f, -0.5f, 0.0f);
    }
}

void Viewer::draw_imgid(unsigned int _idx)
{
    int font_size=5;
    if (_idx<vecs_imgid.size())
    {
        QString qs = QString::number(_idx+1);
        glPushMatrix();
        glTranslatef((-0.5f)*font_size*qs.size(), (-0.5f)*font_size, 0.0f);
        glTranslatef(vecs_imgid[_idx][0], vecs_imgid[_idx][1], vecs_imgid[_idx][2]);
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        drawtext(qs, font_size);
        glPopMatrix();
    }
}

void Viewer::draw_imgwire(bool _draw_inside)
{
    std::vector< std::vector<GLfloat> > vecs_corners;
    glPushAttrib(GL_CURRENT_BIT);
    glColor3f(1.0f, 0.0f, 0.0f);
    if (vecs_imgid.size()>=27)
    {
        vecs_corners.push_back(vecs_imgid[2]);
        vecs_corners.push_back(vecs_imgid[8]);
        vecs_corners.push_back(vecs_imgid[26]);
        vecs_corners.push_back(vecs_imgid[20]);

        vecs_corners.push_back(vecs_imgid[0]);
        vecs_corners.push_back(vecs_imgid[6]);
        vecs_corners.push_back(vecs_imgid[24]);
        vecs_corners.push_back(vecs_imgid[18]);

        drawbox(vecs_corners);
    }

    if (_draw_inside)
    {
        vecs_corners.clear();

        vecs_corners.push_back(vecs_imgid[5]);
        vecs_corners.push_back(vecs_imgid[8]);
        vecs_corners.push_back(vecs_imgid[17]);
        vecs_corners.push_back(vecs_imgid[14]);

        vecs_corners.push_back(vecs_imgid[4]);
        vecs_corners.push_back(vecs_imgid[7]);
        vecs_corners.push_back(vecs_imgid[16]);
        vecs_corners.push_back(vecs_imgid[13]);
        drawbox(vecs_corners);

        vecs_corners.clear();

        vecs_corners.push_back(vecs_imgid[14]);
        vecs_corners.push_back(vecs_imgid[17]);
        vecs_corners.push_back(vecs_imgid[26]);
        vecs_corners.push_back(vecs_imgid[23]);

        vecs_corners.push_back(vecs_imgid[13]);
        vecs_corners.push_back(vecs_imgid[16]);
        vecs_corners.push_back(vecs_imgid[25]);
        vecs_corners.push_back(vecs_imgid[22]);
        drawbox(vecs_corners);

        vecs_corners.clear();

        vecs_corners.push_back(vecs_imgid[4]);
        vecs_corners.push_back(vecs_imgid[7]);
        vecs_corners.push_back(vecs_imgid[16]);
        vecs_corners.push_back(vecs_imgid[13]);

        vecs_corners.push_back(vecs_imgid[3]);
        vecs_corners.push_back(vecs_imgid[6]);
        vecs_corners.push_back(vecs_imgid[15]);
        vecs_corners.push_back(vecs_imgid[12]);
        drawbox(vecs_corners);

        vecs_corners.clear();

        vecs_corners.push_back(vecs_imgid[13]);
        vecs_corners.push_back(vecs_imgid[16]);
        vecs_corners.push_back(vecs_imgid[25]);
        vecs_corners.push_back(vecs_imgid[22]);

        vecs_corners.push_back(vecs_imgid[12]);
        vecs_corners.push_back(vecs_imgid[15]);
        vecs_corners.push_back(vecs_imgid[24]);
        vecs_corners.push_back(vecs_imgid[21]);
        drawbox(vecs_corners);

        vecs_corners.clear();

        vecs_corners.push_back(vecs_imgid[2]);
        vecs_corners.push_back(vecs_imgid[5]);
        vecs_corners.push_back(vecs_imgid[14]);
        vecs_corners.push_back(vecs_imgid[11]);

        vecs_corners.push_back(vecs_imgid[1]);
        vecs_corners.push_back(vecs_imgid[4]);
        vecs_corners.push_back(vecs_imgid[13]);
        vecs_corners.push_back(vecs_imgid[10]);
        drawbox(vecs_corners);

        vecs_corners.clear();

        vecs_corners.push_back(vecs_imgid[11]);
        vecs_corners.push_back(vecs_imgid[14]);
        vecs_corners.push_back(vecs_imgid[23]);
        vecs_corners.push_back(vecs_imgid[20]);

        vecs_corners.push_back(vecs_imgid[10]);
        vecs_corners.push_back(vecs_imgid[13]);
        vecs_corners.push_back(vecs_imgid[22]);
        vecs_corners.push_back(vecs_imgid[19]);
        drawbox(vecs_corners);

        vecs_corners.clear();

        vecs_corners.push_back(vecs_imgid[1]);
        vecs_corners.push_back(vecs_imgid[4]);
        vecs_corners.push_back(vecs_imgid[13]);
        vecs_corners.push_back(vecs_imgid[10]);

        vecs_corners.push_back(vecs_imgid[0]);
        vecs_corners.push_back(vecs_imgid[3]);
        vecs_corners.push_back(vecs_imgid[12]);
        vecs_corners.push_back(vecs_imgid[9]);
        drawbox(vecs_corners);

        vecs_corners.clear();

        vecs_corners.push_back(vecs_imgid[10]);
        vecs_corners.push_back(vecs_imgid[13]);
        vecs_corners.push_back(vecs_imgid[22]);
        vecs_corners.push_back(vecs_imgid[19]);

        vecs_corners.push_back(vecs_imgid[9]);
        vecs_corners.push_back(vecs_imgid[12]);
        vecs_corners.push_back(vecs_imgid[21]);
        vecs_corners.push_back(vecs_imgid[18]);
        drawbox(vecs_corners);
    }
    glPopAttrib();
}

void Viewer::drawsphere(std::vector<GLfloat>& _center, GLfloat _radius)
{
    GLint slices = 30;
    GLint stacks = 30;
    //GLfloat sphrcolor[4] = {0.87f, 0.08f, 0.32f, 0.30f};
    GLfloat sphrcolor[4] = {1.0f, 0.0f, 0.0f, 0.25f};
    GLUquadric* quad;
    quad=gluNewQuadric();
    gluQuadricNormals(quad, GL_SMOOTH);

    glPushAttrib(GL_CURRENT_BIT);
    glColor4fv(sphrcolor);
    if (_center.size()>=3)
    {
        glPushMatrix();
        glTranslatef(_center[0], _center[1], _center[2]);
        gluSphere(quad, _radius, slices, stacks);
        glPopMatrix();
    }
    glPopAttrib();
    gluDeleteQuadric(quad);
}

void Viewer::drawsphere(std::vector<GLfloat>& _center, GLfloat _radius, GLfloat* _color)
{
    GLint slices = 30;
    GLint stacks = 30;
    //GLfloat sphrcolor[4] = {0.87f, 0.08f, 0.32f, 0.30f};
    GLUquadric* quad;
    quad=gluNewQuadric();
    gluQuadricNormals(quad, GL_SMOOTH);

    glPushAttrib(GL_CURRENT_BIT);
    glColor4fv(_color);
    if (_center.size()>=3)
    {
        glPushMatrix();
        glTranslatef(_center[0], _center[1], _center[2]);
        gluSphere(quad, _radius, slices, stacks);
        glPopMatrix();
    }
    glPopAttrib();
    gluDeleteQuadric(quad);
}

void Viewer::drawsphere(std::vector<GLfloat>& _center, GLfloat _radius, GLfloat _red, GLfloat _green, GLfloat _blue)
{
    GLint slices = 30;
    GLint stacks = 30;
    //GLfloat sphrcolor[4] = {0.87f, 0.08f, 0.32f, 0.30f};
    GLUquadric* quad;
    quad=gluNewQuadric();
    gluQuadricNormals(quad, GL_SMOOTH);

    glPushAttrib(GL_CURRENT_BIT);
    glColor3f(_red, _green, _blue);
    if (_center.size()>=3)
    {
        glPushMatrix();
        glTranslatef(_center[0], _center[1], _center[2]);
        gluSphere(quad, _radius, slices, stacks);
        glPopMatrix();
    }
    glPopAttrib();
    gluDeleteQuadric(quad);
}

void Viewer::draw_qlfrs()
{
    int font_size=3;
    const int color_range = 10;
    std::vector<GLfloat*> vec_textcolors;

    if (qvec_qlfrs.size()==vecs_imgid.size())
    {
        for (int i=0; i<color_range; i++)
        {
            GLfloat* x = new GLfloat[4];
            std::fill(x, x+4, 1.0f);
            PUtils::generate_color((float)i/color_range, x[0], x[1], x[2]);
            //std::cout << std::endl << "Color " << i << ":" << x[0] << "\t" << x[1] << "\t" << x[2];
            vec_textcolors.push_back(x);
        }

        for (int i=0; i<qvec_qlfrs.size(); i++)
        {
            QStringListIterator qsliter(*qvec_qlfrs.at(i));
            QString qs;
            glPushMatrix();
            glTranslatef(vecs_imgid[i][0], vecs_imgid[i][1], vecs_imgid[i][2]);
            int j=0;
            while (qsliter.hasNext())
            {
                qs = qsliter.next();
                qs.replace(QString(";"), QString (","));
                glTranslatef(0.0f, -5.0f, 0.0f);

                glPushMatrix();
                glTranslatef((-0.5f)*qs.size(), -font_size-1, 0.0f);
                glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
                drawtext(qs, vec_textcolors[(j+3)%vec_textcolors.size()], font_size);
                glPopMatrix();
                j++;
            }

            glPopMatrix();

        }
    }
    for (std::vector<GLfloat*>::iterator iter=vec_textcolors.begin(); iter!=vec_textcolors.end(); iter++)
        delete[] (*iter);
}

void Viewer::draw_usefn()
{
    int font_size=12;
    GLfloat textcolor[]={0.0f, 1.0f, 0.0f, 1.0f};

    if (vec_usefulns.size()==vecs_imgid.size())
    {
        for (int i=0; i<vec_usefulns.size(); i++)
        {
            QString qs = QString::number(vec_usefulns.at(i), 'g', 4);
            glPushMatrix();
            glTranslatef(vecs_imgid[i][0], vecs_imgid[i][1], vecs_imgid[i][2]);
            glTranslatef(0.0f, -5.0f, 0.0f);

            glTranslatef(-0.5*font_size*qs.size(), -font_size-1, 0.0f);
            glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
            drawtext(qs, textcolor, font_size);

            glPopMatrix();
        }
    }
}

void Viewer::draw_diff()
{
    int font_size=12;
    GLfloat textcolor[]={1.0f, 0.5f, 0.0f, 1.0f};
    GLfloat y_trans = -5.0f;

    if (!vec_usefulns.empty())
    {
        y_trans*=2;
    }

    if (vec_diff.size()==vecs_imgid.size())
    {
        for (int i=0; i<vec_diff.size(); i++)
        {
            QString qs = QString::number(vec_diff.at(i), 'g', 4);
            glPushMatrix();
            glTranslatef(vecs_imgid[i][0], vecs_imgid[i][1], vecs_imgid[i][2]);
            glTranslatef(0.0f, y_trans, 0.0f);

            glTranslatef(-0.5*font_size*qs.size(), -font_size-1, 0.0f);
            glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
            drawtext(qs, textcolor, font_size);

            glPopMatrix();
        }
    }
}

void Viewer::draw_allqlfrs()
{
    int font_size=3;
    size_t qlfrsize = vec_xqlfr.size();
    if ( vec_xqlfr.size()==qlfrsize
         && vec_yqlfr.size()==qlfrsize
         && vec_zqlfr.size()==qlfrsize
         && vec_qlfrgrmr.size()==qlfrsize
         && vec_allqlfrs.size()==qlfrsize
         )
    {
        GLfloat sphrcolor[4] = {0.8f, 0.0f, 0.4f, 0.8f};
        GLfloat textcolor[4] = {0.0f, 1.0f, 0.8f, 0.8f};
        for (int i=0; i<vec_allqlfrs.size(); i++)
        {
            std::string qlfr_tmp = vec_allqlfrs[i];
            QString qs_qlfr_tmp(vec_allqlfrs[i].c_str());
            boost::algorithm::to_lower(qlfr_tmp);
            if (std::find(vec_qlfrfilter.begin(), vec_qlfrfilter.end(), qlfr_tmp)!=vec_qlfrfilter.end())
            {
                std::vector<float> vec_coords_tmp;
                vec_coords_tmp.push_back(vec_xqlfr[i]);
                vec_coords_tmp.push_back(vec_yqlfr[i]);
                vec_coords_tmp.push_back(vec_zqlfr[i]);
                drawsphere(vec_coords_tmp, idxr_radius-0.5, sphrcolor);
                glPushMatrix();
                glTranslatef(vec_xqlfr[i], vec_yqlfr[i], vec_zqlfr[i]);
                glTranslatef((-0.5f)*qs_qlfr_tmp.size(), -font_size-1, 0.0f);
                glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
                drawtext(qs_qlfr_tmp, textcolor, font_size);
                glPopMatrix();
            }
        }
    }
}

void Viewer::keyPressEvent(QKeyEvent *e)
{
    const int MIN_VIEW_MODE = STD_ORIENT;
    const int MAX_VIEW_MODE = REAL_GAP;
    const int MIN_DASH_MODE = DISABLE_DASHD;
    const int MAX_DASH_MODE = DASH_MANUAL;
    const Qt::KeyboardModifiers modifiers = e->modifiers();
    bool handled = false;
    if ((e->key()==Qt::Key_Left) && (modifiers==Qt::ShiftModifier))
    {
        if (box_orientation==MIN_VIEW_MODE)
            box_orientation = MAX_VIEW_MODE;
        else box_orientation--;
        qDebug() << "Change to viewing mode: " << box_orientation;
        updateGL();
        handled = true;
    }
    else if ((e->key()==Qt::Key_Right) && (modifiers==Qt::ShiftModifier))
    {
        if (box_orientation==MAX_VIEW_MODE)
            box_orientation = MIN_VIEW_MODE;
        else box_orientation++;
        qDebug() << "Change to viewing mode: " << box_orientation;
        updateGL();
        handled = true;
    }
    else if ((e->key()==Qt::Key_L) && (modifiers==Qt::ControlModifier))
    {
        if (dash_method==MAX_DASH_MODE)
            dash_method = MIN_DASH_MODE;
        else dash_method++;
        if (dash_method == DASH_MANUAL)
            if (!fn_manmatch.empty())
                load_matching(fn_manmatch);
        //if (dash_method == DASH_MANUAL)
            //load_matching("tree1_0912_nykomatch.csv");
            //load_matching("tree2_0912_nykomatch.csv");
            //load_matching("tree3_0912_nykomatch.csv");
            //load_matching("ternary_branching_0912_nykomatch.csv");
            //load_matching("alternative_branch_0912_nykomatch.csv");
            //load_matching("honda_sympodial_0912_nykomatch.csv");
        updateGL();
    }
    else if ((e->key()==Qt::Key_T) && (modifiers==Qt::ControlModifier))
    {
        is_qualifier_drawn = !is_qualifier_drawn;
        updateGL();
        handled = true;
    }
    else if ((e->key()==Qt::Key_Plus)/* && (modifiers==Qt::ControlModifier)*/)
    {
        if (idxr_radius<20)
        {
            idxr_radius+=1;
            idxr_fontsize+=1;
        }
        updateGL();
        handled = true;
    }
    else if ((e->key()==Qt::Key_Minus)/* && (modifiers==Qt::ControlModifier)*/)
    {
        if (idxr_radius>1)
        {
            idxr_radius-=1;
            idxr_fontsize-=1;
        }
        updateGL();
        handled = true;
    }
    else if ((e->key()==Qt::Key_P) && (modifiers==Qt::ControlModifier))
    {
        is_scattered_drawn = !is_scattered_drawn;
        updateGL();
        handled = true;
    }
    else if ((e->key()==Qt::Key_B) && (modifiers==Qt::ControlModifier))
    {
        QColor color_tmp = backgroundColor();
        if (color_tmp == bgcolor_default)
        {
            color_tmp = Qt::white;
            setBackgroundColor(color_tmp);
        }
        else
            setBackgroundColor(bgcolor_default);
        updateGL();
        handled = true;
    }
    else if ((e->key()==Qt::Key_R) && (modifiers==Qt::ControlModifier))
    {
        is_obb_drawn = !is_obb_drawn;
        is_obbresampled_drawn = !is_obbresampled_drawn;
        updateGL();
        handled = true;
    }
    if (!handled)
        QGLViewer::keyPressEvent(e);
}

void Viewer::update_orientation()
{
//    idxr_radius = 6.0f;
//    idxr_fontsize = 5;
    is_realtarget_drawn = false;
    switch (box_orientation)
    {
    case CUSTOM_ORIENT1:
        vecs_imgid[0] = vecs_std_imgid[0];
        vecs_imgid[1] = vecs_std_imgid[3];
        vecs_imgid[2] = vecs_std_imgid[6];
        vecs_imgid[3] = vecs_std_imgid[1];
        vecs_imgid[4] = vecs_std_imgid[4];
        vecs_imgid[5] = vecs_std_imgid[7];
        vecs_imgid[6] = vecs_std_imgid[2];
        vecs_imgid[7] = vecs_std_imgid[5];
        vecs_imgid[8] = vecs_std_imgid[8];

        vecs_imgid[9] = vecs_std_imgid[9];
        vecs_imgid[10] = vecs_std_imgid[12];
        vecs_imgid[11] = vecs_std_imgid[15];
        vecs_imgid[12] = vecs_std_imgid[10];
        vecs_imgid[13] = vecs_std_imgid[13];
        vecs_imgid[14] = vecs_std_imgid[16];
        vecs_imgid[15] = vecs_std_imgid[11];
        vecs_imgid[16] = vecs_std_imgid[14];
        vecs_imgid[17] = vecs_std_imgid[17];

        vecs_imgid[18] = vecs_std_imgid[18];
        vecs_imgid[19] = vecs_std_imgid[21];
        vecs_imgid[20] = vecs_std_imgid[24];
        vecs_imgid[21] = vecs_std_imgid[19];
        vecs_imgid[22] = vecs_std_imgid[22];
        vecs_imgid[23] = vecs_std_imgid[25];
        vecs_imgid[24] = vecs_std_imgid[20];
        vecs_imgid[25] = vecs_std_imgid[23];
        vecs_imgid[26] = vecs_std_imgid[26];
        break;
    case CUSTOM_ORIENT2:
        vecs_imgid[0] = vecs_std_imgid[0];
        vecs_imgid[1] = vecs_std_imgid[3];
        vecs_imgid[2] = vecs_std_imgid[6];
        vecs_imgid[3] = vecs_std_imgid[9];
        vecs_imgid[4] = vecs_std_imgid[12];
        vecs_imgid[5] = vecs_std_imgid[15];
        vecs_imgid[6] = vecs_std_imgid[18];
        vecs_imgid[7] = vecs_std_imgid[21];
        vecs_imgid[8] = vecs_std_imgid[24];

        vecs_imgid[9] = vecs_std_imgid[1];
        vecs_imgid[10] = vecs_std_imgid[4];
        vecs_imgid[11] = vecs_std_imgid[7];
        vecs_imgid[12] = vecs_std_imgid[10];
        vecs_imgid[13] = vecs_std_imgid[13];
        vecs_imgid[14] = vecs_std_imgid[16];
        vecs_imgid[15] = vecs_std_imgid[19];
        vecs_imgid[16] = vecs_std_imgid[22];
        vecs_imgid[17] = vecs_std_imgid[25];

        vecs_imgid[18] = vecs_std_imgid[2];
        vecs_imgid[19] = vecs_std_imgid[5];
        vecs_imgid[20] = vecs_std_imgid[8];
        vecs_imgid[21] = vecs_std_imgid[11];
        vecs_imgid[22] = vecs_std_imgid[14];
        vecs_imgid[23] = vecs_std_imgid[17];
        vecs_imgid[24] = vecs_std_imgid[20];
        vecs_imgid[25] = vecs_std_imgid[23];
        vecs_imgid[26] = vecs_std_imgid[26];
        break;
    case CUSTOM_ORIENT3:
        vecs_imgid[0] = vecs_std_imgid[0];
        vecs_imgid[1] = vecs_std_imgid[1];
        vecs_imgid[2] = vecs_std_imgid[2];
        vecs_imgid[3] = vecs_std_imgid[9];
        vecs_imgid[4] = vecs_std_imgid[10];
        vecs_imgid[5] = vecs_std_imgid[11];
        vecs_imgid[6] = vecs_std_imgid[18];
        vecs_imgid[7] = vecs_std_imgid[19];
        vecs_imgid[8] = vecs_std_imgid[20];

        vecs_imgid[9] = vecs_std_imgid[3];
        vecs_imgid[10] = vecs_std_imgid[4];
        vecs_imgid[11] = vecs_std_imgid[5];
        vecs_imgid[12] = vecs_std_imgid[12];
        vecs_imgid[13] = vecs_std_imgid[13];
        vecs_imgid[14] = vecs_std_imgid[14];
        vecs_imgid[15] = vecs_std_imgid[21];
        vecs_imgid[16] = vecs_std_imgid[22];
        vecs_imgid[17] = vecs_std_imgid[23];

        vecs_imgid[18] = vecs_std_imgid[6];
        vecs_imgid[19] = vecs_std_imgid[7];
        vecs_imgid[20] = vecs_std_imgid[8];
        vecs_imgid[21] = vecs_std_imgid[15];
        vecs_imgid[22] = vecs_std_imgid[16];
        vecs_imgid[23] = vecs_std_imgid[17];
        vecs_imgid[24] = vecs_std_imgid[24];
        vecs_imgid[25] = vecs_std_imgid[25];
        vecs_imgid[26] = vecs_std_imgid[26];
        break;
    case CUSTOM_ORIENT4:
        vecs_imgid[0] = vecs_std_imgid[0];
        vecs_imgid[1] = vecs_std_imgid[9];
        vecs_imgid[2] = vecs_std_imgid[18];
        vecs_imgid[3] = vecs_std_imgid[1];
        vecs_imgid[4] = vecs_std_imgid[10];
        vecs_imgid[5] = vecs_std_imgid[19];
        vecs_imgid[6] = vecs_std_imgid[2];
        vecs_imgid[7] = vecs_std_imgid[11];
        vecs_imgid[8] = vecs_std_imgid[20];

        vecs_imgid[9] = vecs_std_imgid[3];
        vecs_imgid[10] = vecs_std_imgid[12];
        vecs_imgid[11] = vecs_std_imgid[21];
        vecs_imgid[12] = vecs_std_imgid[4];
        vecs_imgid[13] = vecs_std_imgid[13];
        vecs_imgid[14] = vecs_std_imgid[22];
        vecs_imgid[15] = vecs_std_imgid[5];
        vecs_imgid[16] = vecs_std_imgid[14];
        vecs_imgid[17] = vecs_std_imgid[23];

        vecs_imgid[18] = vecs_std_imgid[6];
        vecs_imgid[19] = vecs_std_imgid[15];
        vecs_imgid[20] = vecs_std_imgid[24];
        vecs_imgid[21] = vecs_std_imgid[7];
        vecs_imgid[22] = vecs_std_imgid[16];
        vecs_imgid[23] = vecs_std_imgid[25];
        vecs_imgid[24] = vecs_std_imgid[8];
        vecs_imgid[25] = vecs_std_imgid[17];
        vecs_imgid[26] = vecs_std_imgid[26];
        break;
    case CUSTOM_ORIENT5:
        vecs_imgid[0] = vecs_std_imgid[0];
        vecs_imgid[1] = vecs_std_imgid[9];
        vecs_imgid[2] = vecs_std_imgid[18];
        vecs_imgid[3] = vecs_std_imgid[3];
        vecs_imgid[4] = vecs_std_imgid[12];
        vecs_imgid[5] = vecs_std_imgid[21];
        vecs_imgid[6] = vecs_std_imgid[6];
        vecs_imgid[7] = vecs_std_imgid[15];
        vecs_imgid[8] = vecs_std_imgid[24];

        vecs_imgid[9] = vecs_std_imgid[1];
        vecs_imgid[10] = vecs_std_imgid[10];
        vecs_imgid[11] = vecs_std_imgid[19];
        vecs_imgid[12] = vecs_std_imgid[4];
        vecs_imgid[13] = vecs_std_imgid[13];
        vecs_imgid[14] = vecs_std_imgid[22];
        vecs_imgid[15] = vecs_std_imgid[7];
        vecs_imgid[16] = vecs_std_imgid[16];
        vecs_imgid[17] = vecs_std_imgid[25];

        vecs_imgid[18] = vecs_std_imgid[2];
        vecs_imgid[19] = vecs_std_imgid[11];
        vecs_imgid[20] = vecs_std_imgid[20];
        vecs_imgid[21] = vecs_std_imgid[5];
        vecs_imgid[22] = vecs_std_imgid[14];
        vecs_imgid[23] = vecs_std_imgid[23];
        vecs_imgid[24] = vecs_std_imgid[8];
        vecs_imgid[25] = vecs_std_imgid[17];
        vecs_imgid[26] = vecs_std_imgid[26];
        break;
    case REAL_TARGETS:
        init_imgids(true);
        break;
    case REAL_PTS:
//        idxr_radius = 3.0f;
//        idxr_fontsize = 2;
        vecs_imgid = vecs_absoluteimgid;
        break;
    case REAL_GAP:
        is_realtarget_drawn = true;
//        idxr_radius = 3.0f;
//        idxr_fontsize = 2;
        init_imgids(true);
    case RESAMPLED:
        resampledist_method1();
        break;
    default:
        vecs_imgid = vecs_std_imgid;
    }
}

void Viewer::scattercount()
{
    float precision=2;
    float threshold=0;
    size_t init_tmp, end_tmp;
    std::vector<GLfloat>  vec_tmp, vec_dist;
    std::vector< std::vector<GLfloat> > vecs_scatplotround, vecs_scatplottmp;



    // First compression attempt
    init_scattersize = vecs_scatterplot.size();
    std::sort(vecs_scatterplot.begin(), vecs_scatterplot.end());
    vecs_scatterplot.erase(std::unique(vecs_scatterplot.begin(), vecs_scatterplot.end()), vecs_scatterplot.end());
    final_scattersize = vecs_scatterplot.size();
    std::cout << std::endl;
    std::cout << std::endl << "__FIRST_ATTEMPT__";
    std::cout << std::endl << "Duplicate removal compression: ";
    std::cout << std::endl << "Initial cloud points n#: " << init_scattersize;
    std::cout << std::endl << "Removed duplicates: " << init_scattersize-final_scattersize;
    std::cout << std::endl << "Final cloud points n#: " << final_scattersize;
    std::cout << std::endl;

    threshold = 0.01*maxdist(vecs_scatterplot);
    std::cout << std::endl << "Threshold = " << threshold << std::endl;

    // Second compression attempt, rounding floating point
    std::cout << std::endl << "__SECOND_ATTEMPT__";
    for (std::vector< std::vector<GLfloat> >::iterator iter = vecs_scatterplot.begin(); iter!=vecs_scatterplot.end(); iter++)
    {
        vec_tmp.clear();
        for (std::vector<GLfloat>::iterator jter = (*iter).begin(); jter!=(*iter).end(); jter++)
        {
            vec_tmp.push_back(roundf((*jter)*pow(10, precision))/pow(10, precision));
        }
        vecs_scatplotround.push_back(vec_tmp);
    }
    init_tmp = vecs_scatplotround.size();
    std::sort(vecs_scatplotround.begin(), vecs_scatplotround.end());
    vecs_scatplotround.erase(std::unique(vecs_scatplotround.begin(), vecs_scatplotround.end()), vecs_scatplotround.end());
    end_tmp = vecs_scatplotround.size();
    std::cout << std::endl << "For precision = " << precision << " :";
    std::cout << std::endl << "Initial cloud points n#: " << init_tmp;
    std::cout << std::endl << "Removed duplicates: " << init_tmp-end_tmp;
    std::cout << std::endl << "Final cloud points n#: " << end_tmp;
    std::cout << std::endl;

    // Third attempt on identifying minimal euclidean distance, cancelled
//    for (std::vector< std::vector<GLfloat> >::iterator iter = vecs_scatterplot.begin(); iter!=vecs_scatterplot.end(); iter++)
//    {
//        GLfloat min = 123456789.0f;
//        for (std::vector< std::vector<GLfloat> >::iterator jter = vecs_scatterplot.begin(); jter!=vecs_scatterplot.end(); jter++)
//        {
//            if (iter!=jter)
//            {
//                //std::cout << std::endl << "Indices : " << vecs_scatterplot.end()-iter << " < > " << vecs_scatterplot.end()-jter;
//                min = PROCEDURAL_MIN(min, euclidist(*iter,*jter));
//            }
//        }
//        vec_dist.push_back(min);
//    }
//    std::stringstream ss_dist;
//    for (std::vector<GLfloat>::iterator iter=vec_dist.begin(); iter!=vec_dist.end(); iter++)
//    {
//        ss_dist << *iter << std::endl;
//    }
//    PUtils::save("dist.tmp", ss_dist);

    // Fourth attempt compressing from minimal euclidian distance
    vecs_scatplotround = vecs_scatterplot;
    init_tmp = vecs_scatplotround.size();
    for (std::vector< std::vector<GLfloat> >::iterator iter = vecs_scatplotround.begin(); iter!=vecs_scatplotround.end(); iter++)
    {
        for (std::vector< std::vector<GLfloat> >::iterator jter = vecs_scatplotround.begin(); jter!=vecs_scatplotround.end(); jter++)
        {
            if (iter!=jter)
            {
                if (euclidist(*iter,*jter)<threshold)
                {
                    vecs_scatplotround.erase(jter);
                    jter--;
                }
            }
            //std::cout << std::endl << "jter: " << vecs_scatplotround.end()-jter << " size: ";
        }
    }
    end_tmp = vecs_scatplotround.size();
    std::cout << std::endl;
    std::cout << std::endl << "__FOURTH_ATTEMPT__";
    std::cout << std::endl << "Euclidian compression: ";
    std::cout << std::endl << "Initial cloud points n#: " << init_tmp;
    std::cout << std::endl << "Removed duplicates: " << init_tmp-end_tmp;
    std::cout << std::endl << "Final cloud points n#: " << end_tmp;
    std::cout << std::endl;



    // Fifth attempt compressing from minimal euclidian distance
    vecs_scatplottmp = vecs_scatterplot;
    init_tmp = vecs_scatplottmp.size();
    for (std::vector< std::vector<GLfloat> >::iterator iter = vecs_scatplottmp.begin(); iter!=vecs_scatplottmp.end(); iter++)
    {
        for (std::vector< std::vector<GLfloat> >::iterator jter = iter+1; jter!=vecs_scatplottmp.end(); jter++)
        {
            //std::cout << std::endl << "distance = " << euclidist(*iter,*jter);
            if (euclidist(*iter,*jter)<threshold)
            {
                jter = vecs_scatplottmp.erase(jter);
//                iter--;
//                break;
                jter--;
            }
        }
    }
    end_tmp = vecs_scatplottmp.size();
    std::cout << std::endl;
    std::cout << std::endl << "__FIFTH_ATTEMPT__";
    std::cout << std::endl << "Alternative Euclidian compression: ";
    std::cout << std::endl << "Initial cloud points n#: " << init_tmp;
    std::cout << std::endl << "Removed duplicates: " << init_tmp-end_tmp;
    std::cout << std::endl << "Final cloud points n#: " << end_tmp;
    std::cout << std::endl;

    std::stringstream ss_scatterplot;
    for (std::vector< std::vector<GLfloat> >::iterator iter = vecs_scatplottmp.begin(); iter!=vecs_scatplottmp.end(); iter++)
    {
        for (std::vector<GLfloat>::iterator jter = (*iter).begin(); jter!=(*iter).end(); jter++)
        {
            if (jter!=(*iter).begin())
                ss_scatterplot << ',';
            ss_scatterplot << (*jter);
        }
        ss_scatterplot << std::endl;
    }
    PUtils::save("scatterplot.tmp", ss_scatterplot);

    init_imgids(true);
    resampledist_method2();
    resampledist_method3();
}

GLfloat Viewer::euclidist(const std::vector<float>& _pt1, const std::vector<float>& _pt2)
{
    GLfloat value=-1;
    if (_pt1.size()==_pt2.size())
    {
        value = 0;
        for (int i=0; i<_pt1.size(); i++)
        {
            value += pow((_pt1[i]-_pt2[i]), 2.0f);
        }
        value = sqrt(value);
    }
    else
    {
        std::cout << std::endl << "Warning: Attempt to compare unequal sized vectors!";
    }
    return value;
}

GLfloat Viewer::maxdist(std::vector< std::vector<GLfloat> >& _src)
{
    GLfloat value=0;
    for (size_t i=0; i<_src.size(); i++)
    {
        for (size_t j=0; j<_src.size(); j++)
        {
            value = PROCEDURAL_MAX(value, euclidist(_src[i],_src[j]));
        }
    }
    return value;
}

void Viewer::resampledist_method1()
{
    uint idx_tmp;
    GLfloat min = 123456789.0f;
    GLfloat min_tmp = 123456789.0f;
    float sum_min=0;
    vec_absolresampld1.clear();
    if (vecs_absoluteimgid.size()==vecs_imgid.size())
    {
        init_imgids(true);
        for (unsigned int i=0; i<vecs_imgid.size(); i++)
        {
            min = min_tmp = 123456789.0f;
            for (unsigned int j=0; j<vecs_absoluteimgid.size(); j++)
            {
                min = PROCEDURAL_MIN(min, euclidist(vecs_imgid[i], vecs_absoluteimgid[j]));
                if (min<min_tmp)
                {
                    idx_tmp = j;
                    min_tmp = min;
                }
            }
            vec_absolresampld1.push_back(idx_tmp);
            sum_min+=min_tmp;
        }


        std::vector<uint> vec_absolresuniq = vec_absolresampld1;
        std::sort(vec_absolresuniq.begin(), vec_absolresuniq.end());
        vec_absolresuniq.erase(std::unique(vec_absolresuniq.begin(), vec_absolresuniq.end()), vec_absolresuniq.end());

//        std::cout << std::endl;
//        std::cout << std::endl << "Dropped Images After Resampling: " << vec_absolresampld.size()-vec_absolresuniq.size();
//        std::cout << std::endl << "Final Images N#: " << vec_absolresuniq.size();
//        std::cout << std::endl << "Images Kept Indices: ";
//        for (unsigned int i=0; i<vec_absolresuniq.size(); i++)
//        {
//            std::cout << vec_absolresuniq[i]+1;
//            if (i<vec_absolresuniq.size())
//                std::cout << ", ";
//        }
//        std::cout << std::endl;
       std::cout << std::endl << "Resample Dist Method 1: min sum = " << sum_min;
    }
}

void Viewer::resampledist_method2()
{
    std::vector< std::vector<GLfloat> >::iterator minter;
    std::vector< std::vector<GLfloat> > vecs_imgid_tmp, vecs_absoluteimgid_res;
    std::vector<uint> vec_idxrespld;
    float min_tmp;
    vecs_imgid_tmp = vecs_imgid;
    float sum_min=0;

    vec_absolresampld2.clear();
    std::cout << std::endl << "Processing call for resampledist_method2() ..." ;
    for (unsigned int j=0; j<vecs_absoluteimgid.size(); j++)
    {
        float dist_tmp;
        min_tmp = euclidist(vecs_imgid_tmp.front(), vecs_absoluteimgid[j]);
        minter = vecs_imgid_tmp.begin();
        for (std::vector< std::vector<GLfloat> >::iterator iter = vecs_imgid_tmp.begin(); iter!=vecs_imgid_tmp.end(); iter++)
        {
            dist_tmp = euclidist(*iter, vecs_absoluteimgid[j]);
            //std::cout << std::endl << "Resample Dist Method 2: all dist = " << min_tmp;
            if (dist_tmp<min_tmp)
            {
                minter = iter;
                min_tmp = dist_tmp;
            }
        }
        //std::cout << std::endl << "Resample Dist Method 2: min dist = " << min_tmp;
        vecs_absoluteimgid_res.push_back(*minter);
        vecs_imgid_tmp.erase(minter);
        sum_min += min_tmp;
    }
    std::cout << std::endl << "Resample Dist Method 2: min sum = " << sum_min;

    // Converting points to indices so that we know what to draw
    for (unsigned int i=0; i<vecs_absoluteimgid_res.size(); i++)
    {
        for (unsigned int j=0; j<vecs_imgid.size(); j++)
        {
            if (euclidist(vecs_absoluteimgid_res[i], vecs_imgid[j])<0.001)
            {
                vec_idxrespld.push_back(j);
                break;
            }
        }
    }

    // Printing result
    std::cout << std::endl;
    std::cout << std::endl << "Newly established matching: " ;
    for (unsigned int i=0; i<vec_idxrespld.size(); i++)
    {
        std::cout << std::endl << "Image " << i+1 << "\t=>\t" << "Point #" << vec_idxrespld.at(i)+1;
    }

    vec_absolresampld2 = vec_idxrespld;
}

void Viewer::resampledist_method3()
{
    std::vector< std::vector<GLfloat> >::iterator imgter, abster;
    std::vector< std::vector<GLfloat> > vecs_imgid_tmp, vecs_absoluteimgid_tmp;
    std::vector< std::vector<GLfloat> > vecs_distmx;
    std::map<uint, uint> map_idx;
    std::vector<uint> vec_imgidx, vec_absimgidx;
    float min_tmp;
    float sum_min=0;
    vecs_imgid_tmp = vecs_imgid;
    vecs_absoluteimgid_tmp = vecs_absoluteimgid;

    vec_absolresampld3.clear();
    std::cout << std::endl << "Processing call for resampledist_method3() ..." ;
    if ((vecs_absoluteimgid_tmp.size()==vecs_imgid_tmp.size())&&(!vecs_absoluteimgid_tmp.empty()))
    {
        for (imgter = vecs_imgid_tmp.begin(); imgter!=vecs_imgid_tmp.end(); imgter++)
        {
            std::vector<GLfloat> vec_disttmp;
            for (std::vector< std::vector<GLfloat> >::iterator abster = vecs_absoluteimgid_tmp.begin(); abster!=vecs_absoluteimgid_tmp.end(); abster++)
            {
                vec_disttmp.push_back(euclidist(*imgter, *abster));
            }
            vecs_distmx.push_back(vec_disttmp);
        }

        //Output temporarily matrix
        std::stringstream ss_tmp;
        QVector<GLfloat> qvec_tmp;
        QVector< QVector<GLfloat> > qvecs_mx_tmp;
        for (unsigned int i=0; i<vecs_distmx.size(); i++)
        {
            qvec_tmp.clear();
            for (unsigned int j=0; j<vecs_distmx.at(i).size(); j++)
            {
                qvec_tmp.push_back(vecs_distmx.at(i).at(j));
            }
            qvecs_mx_tmp.push_back(qvec_tmp);
        }
        PUtils::data_to_csv(ss_tmp, qvecs_mx_tmp);
        PUtils::save("dist_mx.csv", ss_tmp);

        for (uint i=0; i<vecs_absoluteimgid_tmp.size(); i++)
        {
            vec_imgidx.push_back(i);
            vec_absimgidx.push_back(i);
        }
        while (!vecs_distmx.empty())
        {
            float min_tmp = vecs_distmx[0][0];
            uint min_idx1=0, min_idx2=0;
            for (uint i=0; i<vecs_distmx.size(); i++)
            {
                for (uint j=0; j<vecs_distmx[i].size(); j++)
                {
                    if (min_tmp>vecs_distmx[i][j])
                    {
                        min_tmp = vecs_distmx[i][j];
                        min_idx1 = i;
                        min_idx2 = j;
                    }
                }
            }
            sum_min+=min_tmp;
            map_idx.insert(std::pair<uint,uint>(vec_imgidx[min_idx1], vec_absimgidx[min_idx2]));
            vec_imgidx.erase(vec_imgidx.begin()+min_idx1);
            vec_absimgidx.erase(vec_absimgidx.begin()+min_idx2);
            for (uint i=0; i<vecs_distmx.size(); i++)
            {
                vecs_distmx[i].erase(vecs_distmx[i].begin()+min_idx2);
            }
            vecs_distmx.erase(vecs_distmx.begin()+min_idx1);
        }
        std::cout << std::endl << "Resample Dist Method 3:";
        for (std::map<uint, uint>::iterator map_iter = map_idx.begin(); map_iter!=map_idx.end(); map_iter++)
        {
            std::cout << std::endl << "Image " << map_iter->first+1 << "\t=>\t Point #" << map_iter->second+1;
        }
        std::cout << std::endl << "Resample Dist Method 3: min sum = " << sum_min;
    }
    for (uint i=0; i<map_idx.size(); i++)
        vec_absolresampld3.push_back(map_idx.at(i));
//        for (uint j=0; j<map_idx.size(); j++)
//            if (map_idx.at(j) == i)
//            {
//                vec_absolresampld3.push_back(j);
//                break;
//            }
    std::cout << std::endl << "Resample Dist Method 3 Computed.";
}

void Viewer::load_matching(std::string _filename)
{
    std::stringstream ss_match;
    std::vector<std::vector <std::string> > vecs_match;

    vec_absolresampld_man.clear();
    PUtils::load(_filename.c_str(), ss_match);
    PUtils::csv_to_data(vecs_match, ss_match);

    if (vecs_match.size()!=1)
        std::cout << std::endl << "Vector size retrieved for manual matching is inconsistent!";
    else
    {
        if (vecs_match.at(0).size()!=IMAGE_NB)
        {
            std::cout << std::endl << "Expected entries number are " << IMAGE_NB << " but the application could retrieve " << vecs_match.at(0).size() << " !";
        }
        else
        {
            for (size_t i=0; i<vecs_match.at(0).size(); i++)
            {
                uint tmp;
                std::stringstream ss_tmp;
                ss_tmp << vecs_match.at(0).at(i);
                ss_tmp >> tmp;
                vec_absolresampld_man.push_back(tmp);
            }
        }
    }
}

void Viewer::update_qlfrfilter()
{
    std::vector< std::vector<std::string> > vecs_datas;
    std::ifstream filterfile("qlfr_filter.csv");
    std::stringstream ss;

    ss << filterfile.rdbuf();
    filterfile.close();
    vec_qlfrfilter.clear();
    PUtils::csv_to_data(vecs_datas, ss);
    for (std::vector< std::vector<std::string> >::iterator iter = vecs_datas.begin(); iter != vecs_datas.end(); iter++)
    {
        for (std::vector<std::string>::iterator jter = (*iter).begin(); jter!=(*iter).end(); jter++)
        {
            std::string qlfr_tmp = (*jter);
            boost::algorithm::to_lower(qlfr_tmp);
            vec_qlfrfilter.push_back(qlfr_tmp);
        }
    }
}

std::vector<size_t> Viewer::check_neighbours(std::vector< std::vector<float> >& _vecs_clouds, std::vector<float> _target, unsigned int _neighbours)
{
    std::vector<size_t> value_all, value;
    std::vector<float> vec_dist;

    for (std::vector< std::vector<float> >::iterator iter = _vecs_clouds.begin(); iter != _vecs_clouds.end(); iter++)
    {
        vec_dist.push_back(euclidist(*iter, _target));
    }
    value_all = ordered(vec_dist);
    if (_neighbours<_vecs_clouds.size())
    {
        for (size_t i=0; i<_neighbours; i++)
            value.push_back(value_all[i]);
    }
    else value = value_all;
    return value;
}

std::vector< std::vector<float> > Viewer::getclouds()
{
    std::vector< std::vector<float> > value;
    size_t cloudsize = vec_xqlfr.size();
    if (    cloudsize==vec_xqlfr.size()
            && cloudsize==vec_yqlfr.size()
            && cloudsize==vec_zqlfr.size()
            )
    {
        for (size_t i=0; i<cloudsize; i++)
        {
            std::vector<float> tmp;
            tmp.push_back(vec_xqlfr[i]);
            tmp.push_back(vec_yqlfr[i]);
            tmp.push_back(vec_zqlfr[i]);
            value.push_back(tmp);
        }
    }
    return value;
}

void Viewer::draw_obb(PBoundingBox& _obb)
{
    if (_obb.getVecCorner().size()!=8) return;
    drawline(_obb.getVecCorner()[0], _obb.getVecCorner()[1]);
    drawline(_obb.getVecCorner()[1], _obb.getVecCorner()[2]);
    drawline(_obb.getVecCorner()[2], _obb.getVecCorner()[3]);
    drawline(_obb.getVecCorner()[3], _obb.getVecCorner()[0]);

    drawline(_obb.getVecCorner()[4], _obb.getVecCorner()[5]);
    drawline(_obb.getVecCorner()[5], _obb.getVecCorner()[6]);
    drawline(_obb.getVecCorner()[6], _obb.getVecCorner()[7]);
    drawline(_obb.getVecCorner()[7], _obb.getVecCorner()[4]);

    drawline(_obb.getVecCorner()[0], _obb.getVecCorner()[4]);
    drawline(_obb.getVecCorner()[1], _obb.getVecCorner()[5]);
    drawline(_obb.getVecCorner()[2], _obb.getVecCorner()[6]);
    drawline(_obb.getVecCorner()[3], _obb.getVecCorner()[7]);
}

void Viewer::draw_obb(PBoundingBox& _obb, GLfloat _red, GLfloat _green, GLfloat _blue)
{
    if (_obb.getVecCorner().size()!=8) return;
    drawline(_obb.getVecCorner()[0], _obb.getVecCorner()[1], _red, _green, _blue);
    drawline(_obb.getVecCorner()[1], _obb.getVecCorner()[2], _red, _green, _blue);
    drawline(_obb.getVecCorner()[2], _obb.getVecCorner()[3], _red, _green, _blue);
    drawline(_obb.getVecCorner()[3], _obb.getVecCorner()[0], _red, _green, _blue);

    drawline(_obb.getVecCorner()[4], _obb.getVecCorner()[5], _red, _green, _blue);
    drawline(_obb.getVecCorner()[5], _obb.getVecCorner()[6], _red, _green, _blue);
    drawline(_obb.getVecCorner()[6], _obb.getVecCorner()[7], _red, _green, _blue);
    drawline(_obb.getVecCorner()[7], _obb.getVecCorner()[4], _red, _green, _blue);

    drawline(_obb.getVecCorner()[0], _obb.getVecCorner()[4], _red, _green, _blue);
    drawline(_obb.getVecCorner()[1], _obb.getVecCorner()[5], _red, _green, _blue);
    drawline(_obb.getVecCorner()[2], _obb.getVecCorner()[6], _red, _green, _blue);
    drawline(_obb.getVecCorner()[3], _obb.getVecCorner()[7], _red, _green, _blue);
}

void Viewer::drawline(CGLA::Vec3f _pt1, CGLA::Vec3f _pt2)
{
    glPushAttrib(GL_CURRENT_BIT);
    glColor3f(1.0,0.0,0.0);

    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex3f(_pt1[0], _pt1[1], _pt1[2]);
    glVertex3f(_pt2[0], _pt2[1], _pt2[2]);
    glEnd();

    glPopAttrib();
}

void Viewer::drawline(CGLA::Vec3f _pt1, CGLA::Vec3f _pt2, GLfloat _red, GLfloat _green, GLfloat _blue)
{
    glPushAttrib(GL_CURRENT_BIT);
    glColor3f(_red,_green,_blue);

    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex3f(_pt1[0], _pt1[1], _pt1[2]);
    glVertex3f(_pt2[0], _pt2[1], _pt2[2]);
    glEnd();

    glPopAttrib();
}

void Viewer::update_obbdisplay()
{
    vec_obbdisplayed.clear();
    for (std::vector<PBoundingBox>::iterator iter = vec_obb.begin(); iter != vec_obb.end(); iter++)
        vec_obbdisplayed.push_back(true);
    for (std::vector<PBoundingBox>::iterator iter = vec_obb_resampled.begin(); iter != vec_obb_resampled.end(); iter++)
        vec_obbdisplayed.push_back(true);
}

void Viewer::update_obbdisplay(std::vector<bool>& _vec_obbdisplayed)
{
    if (vec_obbdisplayed.size() == _vec_obbdisplayed.size())
    {
        for (int i=0; i < vec_obbdisplayed.size(); i++)
            vec_obbdisplayed[i] = _vec_obbdisplayed[i];
    }
}

std::vector< GLfloat > Viewer::mean(const std::vector< std::vector<GLfloat> >& _vec_clouds)
{
    std::vector< GLfloat > value;
    size_t s;
    if (!_vec_clouds.empty())
    {
        s = _vec_clouds.front().size();

        if (s!= 0)
        {
            for (int i=0; i < s; i++)
                value.push_back(0);
            for (std::vector< std::vector<GLfloat> >::const_iterator iter = _vec_clouds.begin(); iter != _vec_clouds.end(); iter++)
            {
                if ((*iter).size() == s)
                    for (int i=0; i < s; i++)
                    {
                        value[i] += (*iter)[i];
                    }
            }

            for (int i=0; i < s; i++)
                value[i] /= _vec_clouds.size();
        }
    }
    return value;
}

std::vector< std::vector<GLfloat> > Viewer::compute_center(const std::vector< std::vector< std::vector<GLfloat> > >& _vecs_clouds)
{
    std::vector< std::vector<GLfloat> > value;
    std::vector<GLfloat> vec_tmp;
    for (std::vector< std::vector< std::vector<GLfloat> > >::const_iterator iter = _vecs_clouds.begin(); iter != _vecs_clouds.end(); iter++)
    {
        vec_tmp = mean(*iter);
        value.push_back(vec_tmp);
    }
    return value;
}

void Viewer::save_obbcenter(std::string _filename, const std::vector<PBoundingBox>& _vec_obb)
{
    std::ofstream filetmp(_filename.c_str());
    std::stringstream ss;
    std::vector< std::vector< std::vector<GLfloat> > > vec_obbcorners;
    std::vector< std::vector<GLfloat> > vecs_tmp, vec_obbcenter;

    if (filetmp.is_open())
    {
        for (std::vector<PBoundingBox>::const_iterator iter = _vec_obb.begin(); iter != _vec_obb.end(); iter++)
        {
            std::vector<GLfloat> vec_tmp;
            vecs_tmp.clear();
            for (size_t i=0; i < (*iter).getVecCorner().size(); i++)
            {
                vec_tmp.clear();
                vec_tmp.push_back((*iter).getVecCorner()[i][0]);
                vec_tmp.push_back((*iter).getVecCorner()[i][1]);
                vec_tmp.push_back((*iter).getVecCorner()[i][2]);
                vecs_tmp.push_back(vec_tmp);
            }
            vec_obbcorners.push_back(vecs_tmp);
        }
        vec_obbcenter = compute_center(vec_obbcorners);
        PUtils::data_to_csv(ss, vec_obbcenter);
        filetmp << ss.str();
        filetmp.close();
    }
}

void Viewer::save_obbcenter(std::string _filename)
{
    std::ofstream filetmp(_filename.c_str());
    std::stringstream ss;
    std::vector< std::vector< std::vector<GLfloat> > > vec_obbcorners;
    std::vector< std::vector<GLfloat> > vecs_tmp, vec_obbcenter;

    if (filetmp.is_open())
    {
        for (std::vector<PBoundingBox>::const_iterator iter = vec_obb_resampled.begin(); iter != vec_obb_resampled.end(); iter++)
        {
            std::vector<GLfloat> vec_tmp;
            vecs_tmp.clear();
            for (size_t i=0; i < (*iter).getVecCorner().size(); i++)
            {
                vec_tmp.clear();
                vec_tmp.push_back((*iter).getVecCorner()[i][0]);
                vec_tmp.push_back((*iter).getVecCorner()[i][1]);
                vec_tmp.push_back((*iter).getVecCorner()[i][2]);
                vecs_tmp.push_back(vec_tmp);
            }
            vec_obbcorners.push_back(vecs_tmp);
        }
        vec_obbcenter = compute_center(vec_obbcorners);
        PUtils::data_to_csv(ss, vec_obbcenter);
        filetmp << ss.str();
        filetmp.close();
    }
}

void Viewer::save_obbcorner(std::string _filenameprefix, const std::vector<PBoundingBox>& _vec_obb)
{

    std::stringstream ss;
    std::vector< std::vector< std::vector<GLfloat> > > vec_obbcorners;
    std::vector< std::vector<GLfloat> > vecs_tmp;
    int suffix = 1;

    for (std::vector<PBoundingBox>::const_iterator iter = _vec_obb.begin(); iter != _vec_obb.end(); iter++)
    {
        std::string filename_tmp(_filenameprefix);
        std::vector<GLfloat> vec_tmp;
        vecs_tmp.clear();
        for (size_t i=0; i < (*iter).getVecCorner().size(); i++)
        {
            vec_tmp.clear();
            vec_tmp.push_back((*iter).getVecCorner()[i][0]);
            vec_tmp.push_back((*iter).getVecCorner()[i][1]);
            vec_tmp.push_back((*iter).getVecCorner()[i][2]);
            vecs_tmp.push_back(vec_tmp);
        }
        vec_obbcorners.push_back(vecs_tmp);
        ss.str("");

        filename_tmp += "_";
        filename_tmp += PUtils::float_to_stdstr((float)suffix);
        filename_tmp += ".csv";
        std::ofstream filetmp(filename_tmp.c_str());
        if (filetmp.is_open())
        {
            PUtils::data_to_csv(ss, vecs_tmp);

            filetmp << ss.str();
            filetmp.close();
        }
        suffix++;
    }
}

std::vector< std::vector<GLfloat> > Viewer::remove_furthest(std::vector< std::vector<GLfloat> > _vecs_clouds, std::vector<float> _target, unsigned int _remove_neighbours)
{
    std::vector< std::vector<GLfloat> > value;
    std::vector<size_t> vec_idx;
    std::vector<float> vec_dist;

    for (std::vector< std::vector<float> >::iterator iter = _vecs_clouds.begin(); iter != _vecs_clouds.end(); iter++)
    {
        vec_dist.push_back(euclidist(*iter, _target));
    }
    vec_idx = ordered(vec_dist);
    if (_remove_neighbours<vec_idx.size())
        for (size_t i=0; i<vec_idx.size()-_remove_neighbours; i++)
        {
            value.push_back(_vecs_clouds[vec_idx[i]]);
        }
    else value = _vecs_clouds;
    return value;
}

/*
void Viewer::resample_obb()
{
    std::vector< std::vector< std::vector<GLfloat> > >::iterator iter_clouds;
    std::vector< std::vector<GLfloat> >::iterator iter_center;
    std::vector< std::vector<GLfloat> > vecs_tmp, vec_obbcenter;
    std::vector< std::vector< std::vector<GLfloat> > > vec_obbcorners;
    std::vector<CGLA::Vec3f> vec_vertices;

    vecs_obb_cloudsresampled.clear();

    for (std::vector<PBoundingBox>::const_iterator iter = vec_obb.begin(); iter != vec_obb.end(); iter++)
    {
        std::vector<GLfloat> vec_tmp;
        vecs_tmp.clear();
        for (size_t i=0; i < (*iter).getVecCorner().size(); i++)
        {
            vec_tmp.clear();
            vec_tmp.push_back((*iter).getVecCorner()[i][0]);
            vec_tmp.push_back((*iter).getVecCorner()[i][1]);
            vec_tmp.push_back((*iter).getVecCorner()[i][2]);
            vecs_tmp.push_back(vec_tmp);
        }
        vec_obbcorners.push_back(vecs_tmp);
    }
    vec_obbcenter = compute_center(vec_obbcorners);
    for (iter_clouds = vecs_obb_clouds.begin(), iter_center = vec_obbcenter.begin(); iter_clouds != vecs_obb_clouds.end() && iter_center != vec_obbcenter.end(); iter_clouds++, iter_center++)
    {
        int remove_neighbours = (int)(7*(*iter_clouds).size());
        remove_neighbours /= 100;

        std::vector< std::vector<GLfloat> > vecs_tmp = remove_furthest(*iter_clouds, *iter_center, remove_neighbours);
        vecs_obb_cloudsresampled.push_back(vecs_tmp);
    }

    for (iter_clouds = vecs_obb_cloudsresampled.begin(); iter_clouds != vecs_obb_cloudsresampled.end(); iter_clouds++)
    {
        PBoundingBox* pobb = new PBoundingBox();
        vec_vertices.clear();
        for (std::vector< std::vector<GLfloat> >::iterator iter = (*iter_clouds).begin(); iter != (*iter_clouds).end(); iter++)
        {
            vec_vertices.push_back(CGLA::Vec3f((*iter)[0], (*iter)[1], (*iter)[2]));
        }
        pobb->compute_corner(vec_vertices);
        vec_obb_resampled.push_back(*pobb);
        delete pobb;
    }
}
*/

void Viewer::resample_obb()
{
    std::vector< std::vector< std::vector<GLfloat> > >::iterator iter_clouds;
    std::vector< std::vector<GLfloat> >::iterator iter_center;
    std::vector< std::vector<GLfloat> > vecs_tmp, vec_obbcenter;
    std::vector< std::vector< std::vector<GLfloat> > > vec_obbcorners;
    std::vector<CGLA::Vec3f> vec_vertices;

    vecs_obb_cloudsresampled.clear();

    for (std::vector<PBoundingBox>::const_iterator iter = vec_obb.begin(); iter != vec_obb.end(); iter++)
    {
        std::vector<GLfloat> vec_tmp;
        vecs_tmp.clear();
        for (size_t i=0; i < (*iter).getVecCorner().size(); i++)
        {
            vec_tmp.clear();
            vec_tmp.push_back((*iter).getVecCorner()[i][0]);
            vec_tmp.push_back((*iter).getVecCorner()[i][1]);
            vec_tmp.push_back((*iter).getVecCorner()[i][2]);
            vecs_tmp.push_back(vec_tmp);
        }
        vec_obbcorners.push_back(vecs_tmp);
    }
    // We want to compute the distance from the cloud means this time
    //vec_obbcenter = compute_center(vec_obbcorners);
    vec_obbcenter = compute_center(vecs_obb_clouds);
    for (iter_clouds = vecs_obb_clouds.begin(), iter_center = vec_obbcenter.begin(); iter_clouds != vecs_obb_clouds.end() && iter_center != vec_obbcenter.end(); iter_clouds++, iter_center++)
    {
        int remove_neighbours = (int)(7*(*iter_clouds).size());
        remove_neighbours /= 100;

        std::vector< std::vector<GLfloat> > vecs_tmp = remove_furthest(*iter_clouds, *iter_center, remove_neighbours);
        vecs_obb_cloudsresampled.push_back(vecs_tmp);
    }

    for (iter_clouds = vecs_obb_cloudsresampled.begin(); iter_clouds != vecs_obb_cloudsresampled.end(); iter_clouds++)
    {
        PBoundingBox* pobb = new PBoundingBox();
        vec_vertices.clear();
        for (std::vector< std::vector<GLfloat> >::iterator iter = (*iter_clouds).begin(); iter != (*iter_clouds).end(); iter++)
        {
            vec_vertices.push_back(CGLA::Vec3f((*iter)[0], (*iter)[1], (*iter)[2]));
        }
        pobb->compute_corner(vec_vertices);
        vec_obb_resampled.push_back(*pobb);
        delete pobb;
    }
}

void Viewer::save_obbvol()
{
    std::stringstream ss;
    std::ofstream fileobb("obbvol.csv");

    ss.str("");
    if (fileobb.is_open())
    {
        std::vector< std::vector<GLfloat> > vecs_vol_tmp;
        for (std::vector<PBoundingBox>::iterator iter = vec_obb.begin(); iter != vec_obb.end(); iter++)
            vec_obbvol.push_back((*iter).volume());
        vecs_vol_tmp.push_back(vec_obbvol);
        PUtils::data_to_csv(ss, vecs_vol_tmp);
        fileobb << ss.str();
        fileobb.close();
    }

    std::ofstream fileobbres("obbvolres.csv");
    ss.str("");
    if (fileobbres.is_open())
    {
        std::vector< std::vector<GLfloat> > vecs_vol_tmp;
        for (std::vector<PBoundingBox>::iterator iter = vec_obb_resampled.begin(); iter != vec_obb_resampled.end(); iter++)
            vec_obbvol_resampled.push_back((*iter).volume());
        vecs_vol_tmp.push_back(vec_obbvol_resampled);
        PUtils::data_to_csv(ss, vecs_vol_tmp);
        fileobbres << ss.str();
        fileobbres.close();
    }
}

bool Viewer::is_within(PBoundingBox& _obb, std::vector<GLfloat> _pt)
{
    CGLA::Mat3x3f transfer_matrix;
    CGLA::Vec3f vec3f_center_tmp, vec3f_pt, vec3f_pt_proj;
    std::vector<GLfloat> vec_gltmp;
    bool value = false;

    vec3f_center_tmp = _obb.center();

    vec3f_pt = CGLA::Vec3f(_pt[0],_pt[1],_pt[2]);
    CGLA::Vec3f vec3f_u = _obb.u();
    CGLA::Vec3f vec3f_v = _obb.v();
    CGLA::Vec3f vec3f_w = _obb.w();
    CGLA::Vec3f vec1, vec2, vec3;
    vec1 = CGLA::Vec3f(vec3f_w[0], vec3f_w[1], vec3f_w[2]);
    vec2 = CGLA::Vec3f(vec3f_v[0], vec3f_v[1], vec3f_v[2]);
    vec3 = CGLA::Vec3f(vec3f_u[0], vec3f_u[1], vec3f_u[2]);
    //transfer_matrix = CGLA::Mat3x3f(vec1, vec2, vec3);
    transfer_matrix = CGLA::Mat3x3f(vec3f_w, vec3f_v, vec3f_u);

    //vec3f_pt = CGLA::Vec3f(vec3f_pt[0]-vec3f_center_tmp[0], vec3f_pt[1]-vec3f_center_tmp[1], vec3f_pt[2]-vec3f_center_tmp[2]);
    //transfer_matrix = CGLA::invert(transfer_matrix);
    vec3f_center_tmp.operator *=(-1);
    vec3f_pt_proj = PBoundingBox::change_basis(vec3f_pt, vec3f_center_tmp, transfer_matrix);
    value = ((fabs(vec3f_pt_proj[2])-0.01)<=_obb.length()*0.5) && ((fabs(vec3f_pt_proj[1])-0.01)<=_obb.width()*0.5) && ((fabs(vec3f_pt_proj[0])-0.01)<=_obb.height()*0.5);
    return value;
}

float Viewer::belong_rates(PBoundingBox& _obb, std::vector < std::vector<GLfloat> >& _vec_pts)
{
    float value = 0;
    size_t belong_nb = 0;
    for (std::vector < std::vector<GLfloat> >::iterator iter = _vec_pts.begin(); iter != _vec_pts.end(); iter++)
    {
        if (is_within(_obb, *iter))
            belong_nb++;
    }
    if (!_vec_pts.empty())
        value = (float)belong_nb/_vec_pts.size();
    return value;
}

void Viewer::save_belongrates()
{
    std::ofstream filerates("belongrates.csv");
    std::stringstream ss;
    std::vector< std::vector<float> > vecs_belongs;
    std::vector<float> vec_belongs_tmp;
    float rate_tmp;
    for (std::vector<PBoundingBox>::iterator iter_box = vec_obb.begin(); iter_box != vec_obb.end(); iter_box++)
    {
        vec_belongs_tmp.clear();
        for(std::vector< std::vector< std::vector<GLfloat> > >::iterator iter_clouds = vecs_obb_clouds.begin(); iter_clouds != vecs_obb_clouds.end(); iter_clouds++)
        {
            vec_belongs_tmp.push_back(belong_rates(*iter_box, *iter_clouds));
        }
        vecs_belongs.push_back(vec_belongs_tmp);
    }
    if (filerates.is_open())
    {
        PUtils::data_to_csv(ss, vecs_belongs);
        filerates << ss.str();
        filerates.close();
    }
}

void Viewer::save_belongrates_regen()
{
    std::ofstream filerates("belongrates_regen.csv");
    std::stringstream ss;
    std::vector< std::vector<float> > vecs_belongs;
    std::vector<float> vec_belongs_tmp;
    float rate_tmp;
    for (std::vector<PBoundingBox>::iterator iter_box = vec_obb_resampled.begin(); iter_box != vec_obb_resampled.end(); iter_box++)
    {
        vec_belongs_tmp.clear();
        for(std::vector< std::vector< std::vector<GLfloat> > >::iterator iter_clouds = vecs_obb_cloudsresampled.begin(); iter_clouds != vecs_obb_cloudsresampled.end(); iter_clouds++)
        {
            vec_belongs_tmp.push_back(belong_rates(*iter_box, *iter_clouds));
        }
        vecs_belongs.push_back(vec_belongs_tmp);
    }
    if (filerates.is_open())
    {
        PUtils::data_to_csv(ss, vecs_belongs);
        filerates << ss.str();
        filerates.close();
    }
}

void Viewer::save_averages_dist()
{
    std::ofstream filerates("averages_matrix_res.csv");
    std::stringstream ss;
    std::vector< std::vector< GLfloat > > vecs_mean, vecs_matrix;
    //vecs_mean = compute_center(vecs_obb_cloudsresampled);
    vecs_mean = compute_center(vecs_obb_clouds);
    //vecs_mean = compute_center(vecs_obb_cloudsresampled);
    for(std::vector< std::vector<GLfloat> >::iterator iter = vecs_mean.begin(); iter != vecs_mean.end(); iter++)
    {
        std::vector< GLfloat > vec_tmp;
        for(std::vector< std::vector<GLfloat> >::iterator jter = vecs_mean.begin(); jter != vecs_mean.end(); jter++)
        {
            vec_tmp.push_back(euclidist(*iter, *jter));
        }
        vecs_matrix.push_back(vec_tmp);
    }
    if (filerates.is_open())
    {
        PUtils::data_to_csv(ss, vecs_matrix);
        filerates << ss.str();
        filerates.close();
    }
}

void Viewer::save_center_dist()
{
    std::ofstream filerates("center_matrix_res.csv");
    std::stringstream ss;
    std::vector< std::vector< GLfloat > > vecs_matrix;
    std::vector< std::vector<GLfloat> > vecs_tmp, vec_obbcenter;
    std::vector< std::vector< std::vector<GLfloat> > > vec_obbcorners;

    for (std::vector<PBoundingBox>::const_iterator iter = vec_obb_resampled.begin(); iter != vec_obb_resampled.end(); iter++)
    {
        std::vector<GLfloat> vec_tmp;
        vecs_tmp.clear();
        for (size_t i=0; i < (*iter).getVecCorner().size(); i++)
        {
            vec_tmp.clear();
            vec_tmp.push_back((*iter).getVecCorner()[i][0]);
            vec_tmp.push_back((*iter).getVecCorner()[i][1]);
            vec_tmp.push_back((*iter).getVecCorner()[i][2]);
            vecs_tmp.push_back(vec_tmp);
        }
        vec_obbcorners.push_back(vecs_tmp);
    }
    vec_obbcenter = compute_center(vec_obbcorners);

    for(std::vector< std::vector<GLfloat> >::iterator iter = vec_obbcenter.begin(); iter != vec_obbcenter.end(); iter++)
    {
        std::vector< GLfloat > vec_tmp;
        for(std::vector< std::vector<GLfloat> >::iterator jter = vec_obbcenter.begin(); jter != vec_obbcenter.end(); jter++)
        {
            vec_tmp.push_back(euclidist(*iter, *jter));
        }
        vecs_matrix.push_back(vec_tmp);
    }
    if (filerates.is_open())
    {
        PUtils::data_to_csv(ss, vecs_matrix);
        filerates << ss.str();
        filerates.close();
    }
}

