TARGET = promo
CONFIG += qt opengl console
QT *= xml opengl
QMAKE_CXXFLAGS += -std=c++0x
LIBS += -lglut

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

HEADERS += \
    CGLA/Vec4f.h \
    CGLA/Vec3usi.h \
    CGLA/Vec3uc.h \
    CGLA/Vec3i.h \
    CGLA/Vec3Hf.h \
    CGLA/Vec3f.h \
    CGLA/Vec3d.h \
    CGLA/Vec2i.h \
    CGLA/Vec2f.h \
    CGLA/Vec1f.h \
    CGLA/UnitVector.h \
    CGLA/TableTrigonometry.h \
    CGLA/RootSolvers.h \
    CGLA/Quaternion.h \
    CGLA/Mat4x4f.h \
    CGLA/Mat3x4f.h \
    CGLA/Mat3x3f.h \
    CGLA/Mat2x3f.h \
    CGLA/Mat2x2f.h \
    CGLA/Mat1x4f.h \
    CGLA/Mat1x3f.h \
    CGLA/Map.h \
    CGLA/Filter.h \
    CGLA/ExceptionStandard.h \
    CGLA/CommonDefs.h \
    CGLA/CGLA.h \
    CGLA/BitMask.h \
    CGLA/ArithVec.h \
    CGLA/ArithSqMat.h \
    CGLA/ArithMat.h \
    CGLA/ArgExtracter.h \
    CGLA/Vec4uc.h \
    viewport/viewer.h \
    viewport/scene.h \
    viewport/pcylinder.h \
    viewport/pshape.h \
    viewport/pline.h \
    viewport/pmodelingconfig.h \
    grammar/pgrammar.h \
    grammar/palphabet.h \
    grammar/pconstant.h \
    grammar/paxiom.h \
    grammar/pignore.h \
    grammar/parithmetic.h \
    grammar/palgebra.h \
    grammar/pinclude.h \
    grammar/pcontext.h \
    grammar/ppreproc.h \
    grammar/pdefine.h \
    grammar/pword.h \
    grammar/pvariable.h \
    engine/procedural.h \
    engine/plsystem.h \
    engine/prule.h \
    engine/pruleparametric.h \
    engine/pnumericvalue.h \
    engine/plsystemparametric.h \
    engine/pbooleanoperator.h \
    engine/pcomparisonoperator.h \
    engine/plogicaloperator.h \
    engine/pnumeric.h \
    engine/pderivhistoric.h \
    engine/piorecord.h \
    engine/pcontrol.h \
    engine/ptextparser.h \
    mainwindow.h \
    aboutdialog.h \
    outputdialog.h \
    treeviewdialog.h \
    historicitem.h \
    historictreemodel.h \
    windowtitlebar.h \
    engine/pmediator.h \
    viewport/pboundingbox.h \
    viewport/pshapeindexer.h \
    viewport/pcglaprocessor.h \
    viewport/pshapeinitializer.h \
    engine/pmetric.h \
    engine/ppcaeval.h \
    ppcaviewer.h \
    engine/ptextmap.h \
    gettimeofday.h \
    pimgbrowser.h \
    dialogviewer.h \
    viewport/pbox.h \
    viewport/viewerraw.h \
    viewport/ptext3d.h \
    viewport/pimgdata.h \
    grammar/psymbol.h \
    viewport/psimplisticleaf.h \
    engine/block.h \
    engine/graph.h \
    engine/Node.h \
    engine/Data.h \
    engine/GraphEdge.h \
    engine/GraphNode.h \
    engine/pgraphcreator.h \
    qlfrthread.h

SOURCES += \
    CGLA/Vec3i.cpp \
    CGLA/Vec3f.cpp \
    CGLA/Vec2i.cpp \
    CGLA/Vec2f.cpp \
    CGLA/TableTrigonometry.cpp \
    CGLA/Quaternion.cpp \
    CGLA/Mat4x4f.cpp \
    CGLA/Mat3x3f.cpp \
    CGLA/Mat2x2f.cpp \
    viewport/viewer.cpp \
    viewport/scene.cpp \
    viewport/pshape.cpp \
    viewport/pcylinder.cpp \
    viewport/pline.cpp \
    viewport/pmodelingconfig.cpp \
    grammar/pgrammar.cpp \
    grammar/palphabet.cpp \
    grammar/pconstant.cpp \
    grammar/paxiom.cpp \
    grammar/pignore.cpp \
    grammar/parithmetic.cpp \
    grammar/palgebra.cpp \
    grammar/pinclude.cpp \
    grammar/pcontext.cpp \
    grammar/ppreproc.cpp \
    grammar/pdefine.cpp \
    grammar/pvariable.cpp \
    grammar/pword.cpp \
    mainwindow.cpp \
    main.cpp \
    engine/plsystem.cpp \
    engine/prule.cpp \
    engine/pruleparametric.cpp \
    engine/pnumericvalue.cpp \
    engine/plsystemparametric.cpp \
    engine/pbooleanoperator.cpp \
    engine/pcomparisonoperator.cpp \
    engine/plogicaloperator.cpp \
    engine/pnumeric.cpp \
    engine/procedural.cpp \
    engine/pderivhistoric.cpp \
    engine/piorecord.cpp \
    engine/ptextparser.cpp \
    engine/pcontrol.cpp \
    aboutdialog.cpp \
    outputdialog.cpp \
    treeviewdialog.cpp \
    historicitem.cpp \
    historictreemodel.cpp \
    windowtitlebar.cpp \
    engine/pmediator.cpp \
    viewport/pboundingbox.cpp \
    viewport/pshapeindexer.cpp \
    viewport/pcglaprocessor.cpp \
    viewport/pshapeinitializer.cpp \
    engine/pmetric.cpp \
    engine/ppcaeval.cpp \
    ppcaviewer.cpp \
    engine/ptextmap.cpp \
    gettimeofday.cpp \
    pimgbrowser.cpp \
    dialogviewer.cpp \
    viewport/pbox.cpp \
    viewport/viewerraw.cpp \
    viewport/ptext3d.cpp \
    viewport/pimgdata.cpp \
    grammar/psymbol.cpp \
    viewport/psimplisticleaf.cpp \
    engine/graph.cpp \
    engine/maxflow.cpp \
    engine/Node.cpp \
    engine/Data.cpp \
    engine/GraphEdge.cpp \
    engine/GraphNode.cpp \
    engine/pgraphcreator.cpp \
    qlfrthread.cpp

FORMS += \
    mainwindow.ui \
    aboutdialog.ui \
    outputdialog.ui \
    treeviewdialog.ui \
    ppcaviewer.ui \
    pimgbrowser.ui \
    dialogviewer.ui

INCLUDEPATH += \
    "/home/kryptorkid/lab/boost_1_54_0"

OTHER_FILES += \
    images/ets_logo.png \
    stylesheets/promo.qss \
    images/promoico.png \
    images/close.png \
    images/minimize.png \
    images/radiobutton-checked-disabled.png \
    images/radiobutton_unchecked_disabled.png \
    images/promoico.png \
    images/minimize_hover.png \
    images/minimize.png \
    images/ets_logo.png \
    images/dropdown_arrow.png \
    images/close_hover.png \
    images/close.png \
    images/arrow_up.png \
    images/arrow_right_v3.png \
    images/arrow_right_v2.png \
    images/arrow_right.png \
    images/arrow_left_v3.png \
    images/arrow_left_v2.png \
    images/arrow_left.png \
    images/arrow_down.png \
    images/dropdown_arrow_v2.png \
    images/spinbox_arrow_up.png \
    images/spinbox_arrow_down.png \
    images/treeview_arrow_right.png \
    images/treeview_arrow_down.png \
    images/treeview_right_v4.png \
    images/treeview_down_v4.png \
    images/arrow_left_v4.png \
    images/arrow_right_v4.png \
    engine/instances.inc

RESOURCES += \
    images.qrc

RC_FILE = promo.rc
