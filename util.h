#ifndef UTIL_H
#define UTIL_H

#include <QFile>
#include <QtCore>
#include <cmath>

#define APP_VERSION "2.0"
#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

// Simple property
#define GENPROPERTY_S(type, pname, name, settername) \
    private: type pname; \
    public: inline const type &name() const { return pname; } inline void settername (const type &value) { pname = value; } \
    private:

// "Full" property with signal
#define GENPROPERTY_F(type, pname, name, settername, signalname) \
    private: type pname; \
    public: inline const type &name() const { return pname; } inline void settername (const type &value) { pname = value; emit signalname (); } \
    private:

// Read-only property
#define GENPROPERTY_R(type, pname, name) \
    private: type pname; \
    public: inline const type &name() const { return pname; } \
    private:

// Property which returns a pointer
#define GENPROPERTY_PTR(type, pname, name, settername, signalname) \
    private: type pname; \
    public: inline type name() { return pname; } inline void settername (type value) { pname = value; emit signalname (); } \
    private:

inline qreal angle(const QPointF &v)
{
    if (v.x() >= 0)
        return atan(v.y() / v.x());
    return atan(v.y() / v.x()) - M_PI;
}

inline qreal angle(const QPointF &v1, const QPointF &v2)
{
    return angle(v2) - angle(v1);
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

inline qreal simplifyAngle(qreal a)
{
    while (a >= 360)
        a -= 360;
    while (a <= -360)
        a += 360;

    return a;
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
