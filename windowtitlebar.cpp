#include "windowtitlebar.h"
#include <iostream>
#include <QPainter>
#include <QMessageBox>
#include <QMouseEvent>

WindowTitleBar::WindowTitleBar(QWidget *parent) :
    QWidget(parent)
{
    setFixedHeight(33);
    m_Cache = NULL;

    isCloseHover = false;
    isMinimizeHover = false;
    is_pressed = false;
    setMouseTracking( true );
}

WindowTitleBar::~WindowTitleBar()
{
    delete m_Cache;
}


void WindowTitleBar::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    delete m_Cache; // Remove old cache

    m_Cache = new QPixmap(size());  // Create a cache with same size as the widget

    //std::cout << std::endl << "m_Cache " << size().width() << " " << size().height();
    m_Cache->fill(QColor(177, 177, 203,   255));  // Create a the transparent background

    QPainter painter(m_Cache); // Start painting the cache

    //QColor customGrey    (128, 128, 128,   255);
    QColor lightBlue    (177, 177, 203,   255);
    QColor gradientStart(  128,   128,   128,   160);
    QColor gradientEnd  (  16,   16,   16,   240);

    QLinearGradient linearGrad(QPointF(0, 0), QPointF(0, height()));
    linearGrad.setColorAt(0, gradientStart);
    linearGrad.setColorAt(1, gradientEnd);

    /********** Title bar's frame **********/
    QPolygon frame;

    frame << QPoint(          10,  0)
    << QPoint(width() - 2,  0)
    << QPoint(width() - 2, 32)
    << QPoint(          0, 32)
    << QPoint(          0, 10);

    painter.setPen  (QPen  (lightBlue ));
    painter.setBrush(QBrush(linearGrad));

    painter.drawPolygon(frame);
    /***************************************/

    /********** Title bar's buttons area **********/
    QPolygon buttons;

    buttons << QPoint(width() - 54,  2)
      << QPoint(width() -  2,  2)
      << QPoint(width() -  2, 32)
      << QPoint(width() - 64, 32)
      << QPoint(width() - 64, 12);

    painter.setPen  (QPen  (lightBlue));
    painter.setBrush(QBrush(lightBlue));

    painter.drawPolygon(buttons);
    /**********************************************/

    m_Title.setStyleSheet("color: white; font-family: Sans; font-weight: bold; font-size: 14px");
    m_Title.setText("Procedural Modeling");

    if (!pmIco.load( ":/images/promoico.png" ))
    {
        qWarning("Failed to load /images/ets_logo.png");
    }
    if (!pmClose.load( ":/images/close.png" ))
    {
        qWarning("Failed to load /images/close.png");
    }
    if (!pmMinimize.load( ":/images/minimize.png" ))
    {
        qWarning("Failed to load /images/minimize.png");
    }
    if (!pmMinimizeHover.load( ":/images/minimize_hover.png" ))
    {
        qWarning("Failed to load /images/minimize.png");
    }

    if (!pmCloseHover.load( ":/images/close_hover.png" ))
    {
        qWarning("Failed to load /images/close_hover.png");
    }
}

void WindowTitleBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QFont font("Verdana");

    font.setBold(true);
    if(m_Cache != NULL)
    {
        QPainter painter(this);

        painter.drawPixmap(0, -4, *m_Cache);
        painter.setPen(QColor(255,255,255));

        painter.setFont(font);
        painter.drawText(32, 18, m_Title.text());
        painter.drawPixmap(2,2,pmIco.scaled(QSize(24,24), Qt::KeepAspectRatio));


        if(!isCloseHover)
            painter.drawPixmap(width()-30,2,pmClose.scaled(QSize(28,26), Qt::KeepAspectRatio));
        else
            painter.drawPixmap(width()-30,2,pmCloseHover.scaled(QSize(28,26), Qt::KeepAspectRatio));

        if(!isMinimizeHover)
            painter.drawPixmap(width()-60,2,pmMinimize.scaled(QSize(28,28), Qt::KeepAspectRatio));
        else
            painter.drawPixmap(width()-60,2,pmMinimizeHover.scaled(QSize(28,28), Qt::KeepAspectRatio));
    }

}


void WindowTitleBar::mousePressEvent(QMouseEvent *event)
{
    //QMessageBox msgBox;
    int x = event->pos().x();
    int y = event->pos().y();

    if (event->button()== Qt::LeftButton)
    {
        if ((y>2)&&(y<30))
        {
            if ((x>width()-30)&&(x<width()-2))
            {
                emit on_close();
            }
            else if ((x>width()-60)&&(x<width()-32))
            {
                emit on_minimize();
            }
        }
    }
    // Drag purpose:
    is_pressed = true;
    pOffsetPress = event->pos();
}

/*manage button hover*/
void WindowTitleBar::mouseMoveEvent ( QMouseEvent * event )
{
    int x = event->pos().x();
    int y = event->pos().y();

    if ((y>2)&&(y<30))
    {
        if ((x>width()-30)&&(x<width()-2))
        {
            //hover close button
             isCloseHover = true;
        }
        else if ((x>width()-60)&&(x<width()-32))
        {
            //hover Minimize button
            isMinimizeHover = true;
        } else
        {
            isCloseHover = false;
            isMinimizeHover = false;
        }
    } else {
        isCloseHover = false;
        isMinimizeHover = false;
    }
    this->update();
    // dragging purpose:
    if (is_pressed==true)
    {
        setCursor(QCursor(Qt::PointingHandCursor));
        QPoint p = event->globalPos();
        window()->move(p - pOffsetPress);
    }
}

void WindowTitleBar::mouseReleaseEvent(QMouseEvent *event)
{
  Q_UNUSED(event);

  setCursor(QCursor(Qt::ArrowCursor));
  is_pressed = false;
}

