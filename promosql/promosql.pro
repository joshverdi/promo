#-------------------------------------------------
#
# Project created by QtCreator 2014-09-24T12:56:04
#
#-------------------------------------------------

QT       += core sql widgets
QT       *= xml opengl
QT       -= gui

TARGET = promosql
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app
LIBS *= -lglut

unix {
    LIBS *= -lGLU
    CONFIG(debug, debug|release){LIBS *= -L/usr/lib -lQGLViewer}
    else  {LIBS *= -L/usr/lib -lQGLViewer}
 }

win32-g++ {
    CONFIG(debug, debug|release){LIBS *= -L"C:/QtSDK/Desktop/libQGLViewer-2.3.17/build/mingw" -lQGLViewerd2}
    else  {LIBS *= -L"C:/QtSDK/Desktop/libQGLViewer-2.3.17/build/mingw" -lQGLViewer2}
}

win32-msvc* {
    CONFIG(debug, debug|release){LIBS *= "C:/QtSDK/Desktop/libQGLViewer-2.3.17/build/msvc/QGLViewerd2.lib"}
    else  {LIBS *= "C:/QtSDK/Desktop/libQGLViewer-2.3.17/build/msvc/QGLViewer2.lib"}
}

SOURCES += main.cpp \
    pgrouprecord.cpp \
    putils.cpp \
    pimgset.cpp \
    pdbsetup.cpp \
    pqualifier.cpp \
    pquestionnaire.cpp \
    viewer.cpp \
    ptext3d.cpp \
    CGLA/Mat2x2f.cpp \
    CGLA/Mat3x3f.cpp \
    CGLA/Mat4x4f.cpp \
    CGLA/Quaternion.cpp \
    CGLA/TableTrigonometry.cpp \
    CGLA/Vec2f.cpp \
    CGLA/Vec2i.cpp \
    CGLA/Vec3f.cpp \
    CGLA/Vec3i.cpp \
    pboundingbox.cpp \
    dialogqlfr.cpp

HEADERS += \
    pgrouprecord.h \
    putils.h \
    pimgset.h \
    pdbsetup.h \
    main.h \
    pqualifier.h \
    pquestionnaire.h \
    viewer.h \
    ptext3d.h \
    CGLA/ArgExtracter.h \
    CGLA/ArithMat.h \
    CGLA/ArithSqMat.h \
    CGLA/ArithVec.h \
    CGLA/BitMask.h \
    CGLA/CGLA.h \
    CGLA/CommonDefs.h \
    CGLA/ExceptionStandard.h \
    CGLA/Filter.h \
    CGLA/Map.h \
    CGLA/Mat1x3f.h \
    CGLA/Mat1x4f.h \
    CGLA/Mat2x2f.h \
    CGLA/Mat2x3f.h \
    CGLA/Mat3x3f.h \
    CGLA/Mat3x4f.h \
    CGLA/Mat4x4f.h \
    CGLA/Quaternion.h \
    CGLA/RootSolvers.h \
    CGLA/TableTrigonometry.h \
    CGLA/UnitVector.h \
    CGLA/Vec1f.h \
    CGLA/Vec2f.h \
    CGLA/Vec2i.h \
    CGLA/Vec3d.h \
    CGLA/Vec3f.h \
    CGLA/Vec3Hf.h \
    CGLA/Vec3i.h \
    CGLA/Vec3uc.h \
    CGLA/Vec3usi.h \
    CGLA/Vec4f.h \
    CGLA/Vec4uc.h \
    pboundingbox.h \
    dialogqlfr.h

INCLUDEPATH += \
    "/home/kryptorkid/lab/boost_1_54_0"

FORMS += \
    dialogqlfr.ui
