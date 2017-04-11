#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include "viewer.h"
#include "engine/ptextparser.h"
#include "QGLViewer/manipulatedCameraFrame.h"
#include "QGLViewer/VRender/VRender.h"
#include <QMouseEvent>
#include <QFileDialog>

using namespace qglviewer;
using namespace std;

Viewer::Viewer()
{
    scene = 0;
    selection_occured = false;
    multiselect_activated = false;
    horizgrid_drawn = false;
}

Viewer::Viewer(Scene* _pScene, int type, QWidget* parent, const QGLWidget* shareWidget, Qt::WindowFlags flags, bool _draw_grid, bool _draw_axis)
    : QGLViewer(parent, shareWidget, flags), scene(_pScene)
{ 
    selection_occured = false;
    multiselect_activated = false;
    horizgrid_drawn = false;

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
    setSceneRadius(10);
    restoreStateFromFile();

    setGridIsDrawn(_draw_grid);
    setAxisIsDrawn(_draw_axis);
    //connect(this, SIGNAL(drawFinished(bool)), SLOT(saveSnapshot(bool)));
}

Viewer::~Viewer()
{
    glMatrixMode(GL_MODELVIEW);
    //delete scene;
}

void Viewer::draw()
{
    if (horizgrid_drawn==true)
        draw_horiz_grid();
    if (scene) scene->draw(false);
    // Reset model view matrix to world coordinates origin
    //saveSnapshot(true);
    if(selectMode != NONE)
       drawSelectionRectangle();
}

void Viewer::drawWithNames()
{
    if (scene) scene->draw(true);
}

void Viewer::init()
{
    //setSceneRadius(radius_from_str(scene->getLSystem()->getText()));
    //glDisable(GL_LIGHTING);
    //setSelectRegionWidth(10);
    //setSelectRegionHeight(10);
    bgcolor_default = backgroundColor();
}

//void Viewer::postSelection(const QPoint& _point)
//{
//    bool found;
//    camera()->convertClickToLine(_point, origin, direction);
//    selected_point = camera()->pointUnderPixel(_point, found);
//    selected_point -= 0.01f*direction;
//    if (scene)
//    {
//        scene->setSelection(selectedName());
//        scene->setSelectionChanged(true);
//    }
//    drawWithNames();
//    selection_occured = true;
//}

void Viewer::endSelection(const QPoint&)
{
    glFlush();
    GLint nbHits = glRenderMode(GL_RENDER);

    if (nbHits <= 0)
    {
        setSelectedName(-1);
        scene->clear_list_selection();
        scene->setSelectionChanged(true);
    }
    if(nbHits)
    {
        for(int i=0; i<nbHits; i++)
            switch(selectMode)
            {
            case ADD:
                scene->addIdToSelection((selectBuffer())[4*i+3]);
                //std::cout << std::endl << "Added " << selectBuffer()[4*i+3] << " to selection!";
                break;
            case REMOVE:
                scene->removeIdFromSelection((selectBuffer())[4*i+3]);
                break;
            case SINGLE:
                scene->clear_list_selection();
                scene->addIdToSelection((selectBuffer())[4*i+3]);
            default: break;
            }
        scene->setSelectionChanged(true);
    }
    selectMode = NONE;
    drawWithNames();
    selection_occured = true;
}

void Viewer::mousePressEvent(QMouseEvent *e)
{

    rectangle = QRect(e->pos(),e->pos());
//    if((e->button() == Qt::LeftButton) && (e->modifiers() == Qt::MetaModifier))
//    {
//        scene->clear_list_selection();
//        scene->setSelectionChanged(true);
//    }

    if((e->button() == Qt::LeftButton) && (e->modifiers() == Qt::ShiftModifier))
        selectMode = SINGLE;
    else
    {
        if((e->button() == Qt::LeftButton) && (e->modifiers() == Qt::ControlModifier))
            selectMode = ADD;
        else
        {
            QGLViewer::mousePressEvent(e);
        }
    }
}

void Viewer::mouseMoveEvent(QMouseEvent *e)
{
    if(selectMode != NONE)
    {
        rectangle.setBottomRight(e->pos());
        updateGL();
    }
    else
        QGLViewer::mouseMoveEvent(e);
}

void Viewer::mouseReleaseEvent(QMouseEvent *e)
{
    if(selectMode != NONE)
    {
        rectangle = rectangle.normalized();
        setSelectRegionWidth(rectangle.width());
        setSelectRegionHeight(rectangle.height());
        select(rectangle.center());
        updateGL();
    }
    else
        QGLViewer::mouseReleaseEvent(e);
}

void Viewer::drawSelectionRectangle() const
{
    startScreenCoordinatesSystem();
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(0.0, 0.0, 0.3f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2i(rectangle.left(),  rectangle.top());
    glVertex2i(rectangle.right(), rectangle.top());
    glVertex2i(rectangle.right(), rectangle.bottom());
    glVertex2i(rectangle.left(),  rectangle.bottom());
    glEnd();

    glLineWidth(2.0);
    glColor4f(0.4f, 0.4f, 0.5f, 0.3f);
    glBegin(GL_LINE_LOOP);
    glVertex2i(rectangle.left(),  rectangle.top());
    glVertex2i(rectangle.right(), rectangle.top());
    glVertex2i(rectangle.right(), rectangle.bottom());
    glVertex2i(rectangle.left(),  rectangle.bottom());
    glEnd();

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    stopScreenCoordinatesSystem();
}

void Viewer::take_snapshot(int _resolut, QString _filename)
{
    unsigned int vwr_width, vwr_height;
    QString qs_png("PNG");
    switch (_resolut)
    {
    case RES_1024x768:
        vwr_width = 1024;
        vwr_height = 768;
        break;
    case RES_1200x780:
        vwr_width = 1200;
        vwr_height = 780;
        break;
    }
    //resize(vwr_width, vwr_height);
    setSnapshotFileName(_filename);
    setSnapshotFormat(qs_png);
    setSnapshotQuality(100);
    updateGL();
    saveSnapshot(_filename, true);
}

void Viewer::take_snapshot(int _resolut, QString _filename, QString _qs_format)
{
    unsigned int vwr_width, vwr_height;
    switch (_resolut)
    {
    case RES_1024x768:
        vwr_width = 1024;
        vwr_height = 768;
        break;
    case RES_1200x780:
        vwr_width = 1200;
        vwr_height = 780;
        break;
    }
    //resize(vwr_width, vwr_height);
    setSnapshotFileName(_filename);
    setSnapshotFormat(_qs_format);
    setSnapshotQuality(100);
    updateGL();
    saveSnapshot(_filename, true);
}

float Viewer::radius_from_str(std::string _src)
{
    unsigned int value;
    unsigned int prnthss_closer, offset;
    PAlphabet alphabet;
    PTextParser textparser(_src);
    std::vector<float> vec_allparams, vec_tmp;

    value = 1;
    for (unsigned int i=0; i<_src.size(); i+=offset)
    {
        offset=1;
        prnthss_closer = textparser.find_bracket_closer(i+1);
        if (prnthss_closer!=0)
            offset = prnthss_closer-i+1;
        if (alphabet.is_alphabet(_src[i]))
        {
            vec_tmp.clear();
            vec_tmp = textparser.pick_params(i);
            if (vec_tmp.empty())
                vec_allparams.insert(vec_allparams.end(), vec_tmp.begin(), vec_tmp.end());
        }
    }
    for (std::vector<float>::iterator iter = vec_allparams.begin(); iter!=vec_allparams.end(); iter++)
        value = PROCEDURAL_MAX(value, (*iter));
    return value;
}

void Viewer::set_pointSize()
{
    glPointSize(0.5);
}

void Viewer::draw_horiz_grid()
{
    glPushAttrib(GL_CURRENT_BIT);
    glColor4f(1.0f,1.0f,1.0f,1.0f);
    glPushMatrix();
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    QGLViewer::drawGrid(5.0f, 10);
    glPopMatrix();
    glPopAttrib();
}

void Viewer::keyPressEvent(QKeyEvent *e)
{
    bool handled = false;
    const Qt::KeyboardModifiers modifiers = e->modifiers();
    if ((e->key()==Qt::Key_B) && (modifiers==Qt::ControlModifier))
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
    else if ((e->key()==Qt::Key_O) && (modifiers==Qt::ControlModifier))
    {
        if (scene) scene->clear_obb();
        updateGL();
        handled = true;
    }
    else if ((e->key()==Qt::Key_P) && (modifiers==Qt::ControlModifier))
    {
        setSnapshotFormat("EPS");
        saveSnapshot("testsnapshot.eps");
        updateGL();
        handled = true;
    }
    else if ((e->key()==Qt::Key_K) && (modifiers==Qt::ControlModifier))
    {
        save_camera();
        handled = true;
    }
    else if ((e->key()==Qt::Key_L) && (modifiers==Qt::ControlModifier))
    {
        load_camera();
        updateGL();
        handled = true;
    }
    else if ((e->key()==Qt::Key_M) && (modifiers==Qt::ControlModifier))
    {
        save_selection();
        handled = true;
    }
    else if ((e->key()==Qt::Key_W) && (modifiers==Qt::ControlModifier))
    {
        load_selection();
        updateGL();
        handled = true;
    }
    if (!handled)
        QGLViewer::keyPressEvent(e);
}

void Viewer::save_camera()
{
    QDomDocument document("promo_camera");
    document.appendChild( this->camera()->domElement("Camera", document) );
    QFile f("promo_camera.xml");
    if (f.open(QFile::WriteOnly))
    {
        QTextStream out(&f);
        document.save(out, 2);
    }
}

void Viewer::load_camera()
{
    // Load DOM from file
    QDomDocument document;
    QFile f("promo_camera.xml");
    if (f.open(QFile::ReadOnly))
    {
        document.setContent(&f);
        f.close();

        // Parse the DOM tree
        QDomElement main = document.documentElement();
        this->camera()->initFromDOMElement(main);
    }
    else
    {
        std::cout << std::endl << "Camera File NOT FOUND!!!";
    }


}

void Viewer::save_selection()
{
    QFile f("promo_selection.csv");
    std::stringstream ss;
    if (f.open(QFile::WriteOnly|QFile::Text))
    {
        if (scene)
        {
            int i=0;
            while (i<scene->getListSelection().size())
            {
                ss << scene->getListSelection().at(i);
                if (++i < scene->getListSelection().size())
                    ss << ",";
            }
        }
        f.write(QString(ss.str().c_str()).toUtf8());
        f.close();
    }
}

void Viewer::load_selection()
{
    QFile f("promo_selection.csv");
    float val;
    QList<int> qsel;
    std::vector<std::string> vecs;
    std::string line;

    if (f.open(QFile::ReadOnly|QFile::Text))
    {
        QTextStream ts(&f);
        line = ts.readLine().toStdString();

        boost::split(vecs, line, boost::is_any_of(","));
        for (auto s: vecs)
        {
            try
            {
                val = boost::lexical_cast<float>(s);
            }
            catch(boost::bad_lexical_cast const&)
            {
                std::cout<<std::endl<<"Boost Lexical Cast Exception";
            }
            qsel.push_back(val);
        }
        if (scene)
        {
            scene->setSelection(qsel);
            scene->setSelectionChanged(true);
        }
    }
}

