#include "psimplisticleaf.h"

PSimplisticLeaf::PSimplisticLeaf()
{
    u = 0.0f;
    v = 0.0f;
    w = 0.0f;
    section = 0.7f; // The section will decide the leaf's shape
    update_size();
}

PSimplisticLeaf::PSimplisticLeaf(GLdouble _height):PCylinder(_height)
{
    u = 0.0f;
    v = 0.0f;
    w = 0.0f;
    section = 0.7f; // The section will decide the leaf's shape
    update_size();
}

PSimplisticLeaf::~PSimplisticLeaf()
{
}

void PSimplisticLeaf::draw()
{
    GLfloat p1[]= {x+leaf_base/2.0f, y-leaf_height, z-leaf_base/2.0f};
    GLfloat p2[]= {x+leaf_base/2.0f, y-leaf_height, z+leaf_base/2.0f};
    GLfloat p3[]= {x-leaf_base/2.0f, y-leaf_height, z+leaf_base/2.0f};
    GLfloat p4[]= {x-leaf_base/2.0f, y-leaf_height, z-leaf_base/2.0f};
    GLfloat up[]= {x, y, z};
    GLfloat down[] = {u, v, w};

    glRotatef(-90, 1.0, 0.0, 0.0);
    gluCylinder(quad_obj, base, top, height, slices, stacks);
    glRotatef(+90, 1.0, 0.0, 0.0);
    glTranslatef(0.0f, height*PROCEDURAL_UNIT, 0.0f);

    glPushAttrib(GL_CURRENT_BIT);
    glColor4f(1.0, 0.0, 0.0, 0.0);
    glBegin(GL_TRIANGLES);

    glVertex3fv(up);
    glVertex3fv(p1);
    glVertex3fv(p2);

    glVertex3fv(up);
    glVertex3fv(p2);
    glVertex3fv(p3);

    glVertex3fv(up);
    glVertex3fv(p3);
    glVertex3fv(p4);

    glVertex3fv(up);
    glVertex3fv(p4);
    glVertex3fv(p1);

    // Lower Pyramid
    glVertex3fv(down);
    glVertex3fv(p2);
    glVertex3fv(p1);

    glVertex3fv(down);
    glVertex3fv(p3);
    glVertex3fv(p2);

    glVertex3fv(down);
    glVertex3fv(p4);
    glVertex3fv(p3);

    glVertex3fv(down);
    glVertex3fv(p1);
    glVertex3fv(p4);

    glEnd();

    glBegin(GL_QUADS);
    glVertex3fv(p1);
    glVertex3fv(p2);
    glVertex3fv(p3);
    glVertex3fv(p4);
    glEnd();
    glPopAttrib();

}

void PSimplisticLeaf::draw(PCglaProcessor& _cgla_proc)
{
    GLfloat p1[]= {x+leaf_base/2.0f, y-(leaf_height*section), z-leaf_base/32.0f};
    GLfloat p2[]= {x+leaf_base/2.0f, y-(leaf_height*section), z+leaf_base/32.0f};
    GLfloat p3[]= {x-leaf_base/2.0f, y-(leaf_height*section), z+leaf_base/32.0f};
    GLfloat p4[]= {x-leaf_base/2.0f, y-(leaf_height*section), z-leaf_base/32.0f};
    GLfloat up[]= {x, y, z};
    GLfloat down[] = {u, v, w};
    GLfloat color_tmp[4];

    CGLA::Vec3f vec(1.0, 0.0, 0.0);
    GLfloat gl_mat[16];
    float angle=-90;

    glPushMatrix();
    _cgla_proc.to_gl_matrix(gl_mat);
    glMultMatrixf(gl_mat);

    //glPushAttrib(GL_CURRENT_BIT);
    glBegin(GL_TRIANGLES);

    // Upper Pyramid
    glVertex3fv(up);
    glVertex3fv(p1);
    glVertex3fv(p2);

    glVertex3fv(up);
    glVertex3fv(p2);
    glVertex3fv(p3);

    glVertex3fv(up);
    glVertex3fv(p3);
    glVertex3fv(p4);

    glVertex3fv(up);
    glVertex3fv(p4);
    glVertex3fv(p1);

    // Lower Pyramid
    glVertex3fv(down);
    glVertex3fv(p2);
    glVertex3fv(p1);

    glVertex3fv(down);
    glVertex3fv(p3);
    glVertex3fv(p2);

    glVertex3fv(down);
    glVertex3fv(p4);
    glVertex3fv(p3);

    glVertex3fv(down);
    glVertex3fv(p1);
    glVertex3fv(p4);

    glEnd();

    glBegin(GL_QUADS);
    glVertex3fv(p1);
    glVertex3fv(p2);
    glVertex3fv(p3);
    glVertex3fv(p4);
    glEnd();

    //glPopAttrib();
    glPopMatrix();

    _cgla_proc.cgla_rotate(angle, vec);
    _cgla_proc.to_gl_matrix(gl_mat);

    glGetFloatv(GL_CURRENT_COLOR,color_tmp);
    glColor4f(0.501f, 0.101f, 0.051f, 1.0f);

    // Before every drawing process, OpenGL matrix must be saved and updated
    glPushMatrix();
    glMultMatrixf(gl_mat);
    gluCylinder(quad_obj, base, top, height, slices, stacks);

    glColor4f(color_tmp[0], color_tmp[1], color_tmp[2], color_tmp[3]);

    // Rotate back and translate properly
    angle=90;
    _cgla_proc.cgla_rotate(angle, vec);
    vec=CGLA::Vec3f(0.0f, height*PROCEDURAL_UNIT, 0.0f);
    _cgla_proc.cgla_translate(vec);

    glPopMatrix();
}

void PSimplisticLeaf::update_size()
{
    x = 0.0f;
    y = height;
    z = 0.0f;
    leaf_base = base*3.0f/2.0f;
    leaf_height = height;
}
