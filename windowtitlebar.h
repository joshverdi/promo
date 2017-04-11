#ifndef WINDOWTITLEBAR_H
#define WINDOWTITLEBAR_H

#include <QWidget>
#include <QPixmap>
#include <QLabel>

class WindowTitleBar : public QWidget
{
    Q_OBJECT
    QPixmap *m_Cache;
    QLabel m_Title;
    QPixmap pmIco, pmClose, pmMinimize;
    QPixmap pmCloseHover, pmMinimizeHover;
    QPoint pOffsetPress;
    bool is_pressed;

protected:
  void resizeEvent(QResizeEvent *event);
  void paintEvent (QPaintEvent  *event);
  void mouseMoveEvent ( QMouseEvent * event );

  void mousePressEvent  (QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  bool isCloseHover, isMinimizeHover;
public:
    explicit WindowTitleBar(QWidget *parent = 0);
    ~WindowTitleBar();
signals:
  void on_close();
  void on_minimize();
    
public slots:
    
};

#endif // WINDOWTITLEBAR_H
