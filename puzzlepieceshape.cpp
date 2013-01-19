#include "puzzlepieceshape.h"

#include <QPainter>

namespace PuzzlePieceShape
{
    QPixmap processImage(const QString &url, int width, int height)
    {
        QString url2(url);
        if (url.contains(QRegExp("/[A-Za-z]:/")))
            url2.remove("file:///");
        else
            url2.remove("file://");
        QPixmap pix(url2);

        if (pix.isNull())
            return pix;

        // If the image is better displayed in "portrait mode", rotate it.
        if ((pix.width() < pix.height() && width >= height) || (pix.width() >= pix.height() && width < height))
        {
            pix = pix.scaledToHeight(width);
            QPixmap pix2(pix.height(), pix.width());
            QPainter p;
            p.begin(&pix2);
            p.rotate(-90);
            p.translate(- pix2.height(), 0);
            p.drawPixmap(0, 0, pix);
            p.end();
            pix = pix2;
        }

        // Scale it to our width
        if (pix.width() - 1 > width || pix.width() + 1 < width)
            pix = pix.scaledToWidth(width);

        // If still not good enough, just crop it
        if (pix.height() > height)
        {
            QPixmap pix2(width, height);
            QPainter p;
            p.begin(&pix2);
            p.drawPixmap(0, 0, pix, 0, (pix.height() - height) / 2, pix.width(), pix.height());
            p.end();
            pix = pix2;
        }

        return pix;
    }
}
