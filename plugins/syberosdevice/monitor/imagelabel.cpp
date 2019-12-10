#include "imagelabel.h"
#include <QImage>
#include <QResizeEvent>
#include <QPainter>
#include <QPixmap>

using namespace SyberOSDevice::Internal;
const static float S_H_DIV_W = 1.5;
ImageLabel::ImageLabel(QWidget *parent)
    : QLabel(parent)
    , m_image(NULL)
    , m_originImage(NULL)
    , m_mouseGrabed(false)
    , m_scaleNum(1.0)
{

}

ImageLabel::~ImageLabel()
{
    if(m_image)
    {
        delete m_image;
        m_image = NULL;
    }

    if(m_originImage)
    {
        delete m_originImage;
        m_originImage = NULL;
    }

}

void ImageLabel::setImage(const QString imagePath)
{
    if(!m_originImage)
    {
        m_originImage = new QImage;
    }

    m_originImage->load(imagePath);
    if(m_originImage->isNull())
    {
        if(m_image)
        {
            delete m_image;
            m_image = NULL;
        }
    }
    else if(!m_image)
    {
        m_image = new QImage(*m_originImage);
    }


    m_screenSize = m_originImage->size();
    scaleTo(size());
    update();
}

void ImageLabel::resizeEvent(QResizeEvent *event)
{
    scaleTo(event->size());
}

void ImageLabel::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);
    if(m_image && !m_image->isNull())
    {
        QPainter painter;
        painter.begin(this);
        painter.fillRect(0, 0, m_image->width(), m_image->height(), m_brush);
        if(m_mouseGrabed&& m_point.rx() < m_image->width() && m_point.ry() < m_image->height())
        {
            painter.drawLine(0, m_point.ry(), m_image->width(), m_point.ry());
            painter.drawLine(m_point.rx(), 0, m_point.rx(), m_image->height());
        }
    }
}

void ImageLabel::mousePressEvent(QMouseEvent *event)
{
    if(m_image && !m_image->isNull())
    {
        m_mouseGrabed = true;
        m_point = event->pos();
        update();
        if(m_mouseGrabed&& m_point.rx() < m_image->width() && m_point.ry() < m_image->height())
        {
            emit mousePressed((m_point*m_scaleNum).toPoint(), true);
        }
    }

}

void ImageLabel::mouseMoveEvent(QMouseEvent *event)
{
    m_point = event->pos() ;
//    if(m_mouseGrabed && m_image && m_point.rx() < m_image->width() && m_point.ry() < m_image->height())
//        emit mouseMoved(m_point*m_scaleNum);
}

void ImageLabel::mouseReleaseEvent(QMouseEvent *event)
{
    m_mouseGrabed = false;
    update();
    emit mousePressed((m_point*m_scaleNum).toPoint(), false);
}

void ImageLabel::scaleTo(QSizeF size)
{
    QSizeF newSize(size);
    if(m_image && !m_image->isNull())
    {
        if(newSize.width() * S_H_DIV_W > newSize.height())
        {
            newSize.setWidth(newSize.height()/S_H_DIV_W);
            m_scaleNum = m_screenSize.height()/newSize.height();
        }
        else
        {
            newSize.setHeight(newSize.width() * S_H_DIV_W);
            m_scaleNum = m_screenSize.width()/newSize.width();
        }
        *m_image = m_originImage->scaled(newSize.toSize(), Qt::KeepAspectRatio);
        //m_image->swap(img);
        //m_pixmap = QPixmap::fromImage(*m_image);
        m_brush.setTextureImage(*m_image);
        update();
    }
}
