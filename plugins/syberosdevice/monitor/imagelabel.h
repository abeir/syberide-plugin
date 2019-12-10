#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#include <QLabel>
#include <QBrush>

namespace SyberOSDevice {
namespace Internal {

class ImageLabel : public QLabel
{
    Q_OBJECT
public:
    ImageLabel(QWidget *parent = 0);
    ~ImageLabel() override;
    void setImage(const QString imagePath);

signals:
    void mouseMoved(QPoint p);
    void mousePressed(QPoint p, bool);


protected:
    virtual void resizeEvent(QResizeEvent * event) Q_DECL_OVERRIDE;
    virtual void paintEvent(QPaintEvent * event) Q_DECL_OVERRIDE;
    virtual void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
    void scaleTo(QSizeF size );

private:
    QImage *m_image;
    QImage *m_originImage;
    QPointF m_point;
    QBrush  m_brush;
    bool    m_mouseGrabed;
    float   m_scaleNum;
    QSizeF   m_screenSize;
};

}
}

#endif // IMAGELABEL_H
