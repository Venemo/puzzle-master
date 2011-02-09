#ifndef UTIL_H
#define UTIL_H

#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

inline QPointF operator*(const QPoint &point, const QSize &size)
{
    return QPointF(point.x() * size.width(), point.y() * size.height());
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
        *aboutString = QString::fromUtf8(file.readAll().constData());
        file.close();
    }
    return *aboutString;
}

#endif // UTIL_H
