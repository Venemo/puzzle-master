#ifndef UTIL_H
#define UTIL_H

#include <QtCore>
#include <cmath>

#define APP_VERSION "1.2.3"
#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

inline QPointF operator*(const QPoint &point, const QSize &size)
{
    return QPointF(point.x() * size.width(), point.y() * size.height());
}

inline qreal operator*(const QPointF &p1, const QPointF &p2)
{
    return p1.x() * p2.x() + p1.y() * p2.y();
}

inline qreal abs(const QPointF &p)
{
    return sqrt(p.x() * p.x() + p.y() * p.y());
}

inline qreal angle(const QPointF &v1, const QPointF &v2)
{
    return acos((v1 * v2) / (abs(v1) * abs(v2)));
}

inline int randomInt(int low, int high)
{
    // Random number between low and high
    return qrand() % ((high + 1) - low) + low;
}

inline int max(int i, int j)
{
    return i > j? i : j;
}

template<class T>
inline T min(T i, T j)
{
    return i < j? i : j;
}

inline const QString &fetchAboutString()
{
    static QString *aboutString = 0;
    if (aboutString == 0)
    {
        QFile file(":/about.txt");
        file.open(QIODevice::ReadOnly);
        aboutString = new QString(QString::fromUtf8(file.readAll().constData()));
        file.close();
    }
    return *aboutString;
}

#endif // UTIL_H
