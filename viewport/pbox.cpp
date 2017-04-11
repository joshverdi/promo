#include "pbox.h"

PBox::PBox()
{
    length = width = height = PROCEDURAL_UNIT;
    init();
}

PBox::PBox(GLdouble _length,GLdouble _width, GLdouble _height)
{
    length  = _length;
    width   = _width;
    height  = _height;
    init();
}

void PBox::init_vertices()
{
    vertices[0][0] = (-length/2); vertices[0][1] = height; vertices[0][2] = width/2;
    vertices[1][0] = (length/2);  vertices[1][1] = height; vertices[1][2] = width/2;
    vertices[2][0] = (length/2);  vertices[2][1] = height; vertices[2][2] = (-width/2);
    vertices[3][0] = (-length/2); vertices[3][1] = height; vertices[3][2] = (-width/2);
    vertices[4][0] = (-length/2); vertices[4][1] = 0;      vertices[4][2] = width/2;
    vertices[5][0] = (length/2);  vertices[5][1] = 0;      vertices[5][2] = width/2;
    vertices[6][0] = (length/2);  vertices[6][1] = 0;      vertices[6][2] = (-width/2);
    vertices[7][0] = (-length/2); vertices[7][1] = 0;      vertices[7][2] = (-width/2);
}

void PBox::init_normals()
{
    GLfloat normals_tmp[6][3] = {  /* Normals for the 6 faces of a cube. */
          {0.0, 1.0, 0.0}, {0.0, 0.0, -1.0}, {0.0, -1.0, 0.0},
          {0.0, 0.0, 1.0}, {1.0, 0.0, 0.0},  {-1.0, 0.0, 0.0} };
    for (int i=0; i<6; i++)
        for (int j=0; j<3; j++)
            normals[i][j] = normals_tmp[i][j];
}

void PBox::init_faces()
{
    GLint faces_tmp[6][4] = {  /* Vertex indices for the 6 faces of a cube. */
          {0, 1, 2, 3}, {3, 2, 6, 7}, {7, 6, 5, 4},
          {4, 5, 1, 0}, {5, 6, 2, 1}, {7, 4, 0, 3} };
    for (int i=0; i<6; i++)
        for (int j=0; j<4; j++)
            faces[i][j] = faces_tmp[i][j];
}

void PBox::init()
{
    init_faces();
    init_normals();
    init_vertices();
}

void PBox::draw()
{
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
    glTranslatef(0.0f, height*PROCEDURAL_UNIT, 0.0f);
}

void PBox::draw(PCglaProcessor& _cgla_proc)
{
    CGLA::Vec3f vec(0.0, 1.0, 0.0);
    GLfloat gl_mat[16];
    _cgla_proc.to_gl_matrix(gl_mat);

    // Before every drawing process, OpenGL matrix must be saved and updated
    glPushMatrix();
    glMultMatrixf(gl_mat);
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
    glPopMatrix();
    _cgla_proc.cgla_translate(vec);
}
