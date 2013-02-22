
// This file is part of Puzzle Master, a fun and addictive jigsaw puzzle game.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//
// Copyright (C) 2010-2013, Timur Kristóf <venemo@fedoraproject.org>

#include <QPainter>
#include "puzzle/creation/imageprocessor.h"
#include "util.h"

namespace Puzzle
{
namespace Creation
{


class ImageProcessorPrivate
{
    friend class ImageProcessor;
    QPixmap pixmap;
    GameDescriptor descriptor;
    QPixmap processImage(const QString &url, int width, int height);
};

QPixmap ImageProcessorPrivate::processImage(const QString &url, int width, int height)
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

ImageProcessor::ImageProcessor(const QString &url, const QSize &viewportSize, int rows, int cols, int strokeThickness)
{
    _p = new ImageProcessorPrivate();
    _p->pixmap = _p->processImage(url, viewportSize.width(), viewportSize.height());
    _p->descriptor.rows = rows;
    _p->descriptor.cols = cols;
    _p->descriptor.viewportSize = viewportSize;
    _p->descriptor.pixmapSize = _p->pixmap.size();
    _p->descriptor.unitSize = QSize(_p->pixmap.width() / cols, _p->pixmap.height() / rows);
    _p->descriptor.tabSize = MIN(_p->descriptor.unitSize.width() / 6.0, _p->descriptor.unitSize.height() / 6.0);
    _p->descriptor.tabOffset = _p->descriptor.tabSize * 0.55;
    _p->descriptor.tabTolerance = 1;
    _p->descriptor.tabFull = _p->descriptor.tabSize + _p->descriptor.tabOffset + _p->descriptor.tabTolerance;
    _p->descriptor.strokeThickness = strokeThickness;
    _p->descriptor.usabilityThickness = MIN(80, MIN(_p->descriptor.unitSize.width(), _p->descriptor.unitSize.height()));
}

ImageProcessor::~ImageProcessor()
{
    delete _p;
}

bool ImageProcessor::isValid()
{
    return !_p->pixmap.isNull();
}

const GameDescriptor &ImageProcessor::descriptor()
{
    return _p->descriptor;
}

QPixmap ImageProcessor::drawPiece(int i, int j, const QPainterPath &shape, const Puzzle::Creation::Correction &corr)
{
    QPainter p;
    QPixmap px(_p->descriptor.unitSize.width() + corr.widthCorrection + 1,
               _p->descriptor.unitSize.height() + corr.heightCorrection + 1);
    px.fill(Qt::transparent);

    p.begin(&px);
    p.setRenderHint(QPainter::SmoothPixmapTransform);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::HighQualityAntialiasing);
    p.setClipping(true);
    p.setClipPath(shape);

    p.drawPixmap(_p->descriptor.tabFull + corr.xCorrection + corr.sxCorrection,
                 _p->descriptor.tabFull + corr.yCorrection + corr.syCorrection,
                 _p->pixmap,
                 i * _p->descriptor.unitSize.width() + corr.sxCorrection,
                 j * _p->descriptor.unitSize.height() + corr.syCorrection,
                 _p->descriptor.unitSize.width() * 2,
                 _p->descriptor.unitSize.height() * 2);

    p.end();
    return px;
}

QPixmap ImageProcessor::drawStroke(const QPainterPath &strokeShape, const QSize &pxSize)
{
    QPainter p;
    QPixmap stroke(pxSize.width() + _p->descriptor.strokeThickness * 2,
                   pxSize.height() + _p->descriptor.strokeThickness * 2);
    stroke.fill(Qt::transparent);
    p.begin(&stroke);
    p.setRenderHint(QPainter::SmoothPixmapTransform, true);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::HighQualityAntialiasing, true);
    p.fillPath(strokeShape, QBrush(QColor(255, 255, 255, 255)));
    p.end();

    return stroke;
}

}
}
