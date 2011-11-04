
#include <QtDeclarative>

struct Segment
{
    Segment() {}
    Segment(const QPoint &p)
        : added(false)
        , point(p)
    {
    }

    int left() const
    {
        return qMin(point.x(), next->point.x());
    }

    int right() const
    {
        return qMax(point.x(), next->point.x());
    }

    bool overlaps(const Segment &other) const
    {
        return left() < other.right() && other.left() < right();
    }

    void connect(Segment &other)
    {
        next = &other;
        other.prev = this;

        horizontal = (point.y() == other.point.y());
    }

    void merge(Segment &other)
    {
        if (right() <= other.right()) {
            QPoint p = other.point;
            Segment *oprev = other.prev;

            other.point = point;
            other.prev = prev;
            prev->next = &other;

            point = p;
            prev = oprev;
            oprev->next = this;
        } else {
            Segment *onext = other.next;
            other.next = next;
            next->prev = &other;

            next = onext;
            next->prev = this;
        }
    }

    int horizontal : 1;
    int added : 1;

    QPoint point;
    Segment *prev;
    Segment *next;
};

void mergeSegments(Segment *a, int na, Segment *b, int nb)
{
    int i = 0;
    int j = 0;

    while (i != na && j != nb) {
        Segment &sa = a[i];
        Segment &sb = b[j];
        const int ra = sa.right();
        const int rb = sb.right();
        if (sa.overlaps(sb))
            sa.merge(sb);
        i += (rb >= ra);
        j += (ra >= rb);
    }
}

void addSegmentsToPath(Segment *segment, QPainterPath &path)
{
    Segment *current = segment;
    path.moveTo(current->point);

    current->added = true;

    Segment *last = current;
    current = current->next;
    while (current != segment) {
        if (current->horizontal != last->horizontal)
            path.lineTo(current->point);
        current->added = true;
        last = current;
        current = current->next;
    }
}

QPainterPath qt_regionToPath(const QRegion &region)
{
    QPainterPath result;
    if (region.rectCount() == 1) {
        result.addRect(region.boundingRect());
        return result;
    }

    const QVector<QRect> rects = region.rects();

    QVarLengthArray<Segment> segments;
    segments.resize(4 * rects.size());

    const QRect *rect = rects.constData();
    const QRect *end = rect + rects.size();

    int lastRowSegmentCount = 0;
    Segment *lastRowSegments = 0;

    int lastSegment = 0;
    int lastY = 0;
    while (rect != end) {
        const int y = rect[0].y();
        int count = 0;
        while (&rect[count] != end && rect[count].y() == y)
            ++count;

        for (int i = 0; i < count; ++i) {
            int offset = lastSegment + i;
            segments[offset] = Segment(rect[i].topLeft());
            segments[offset += count] = Segment(rect[i].topRight() + QPoint(1, 0));
            segments[offset += count] = Segment(rect[i].bottomRight() + QPoint(1, 1));
            segments[offset += count] = Segment(rect[i].bottomLeft() + QPoint(0, 1));

            offset = lastSegment + i;
            for (int j = 0; j < 4; ++j)
                segments[offset + j * count].connect(segments[offset + ((j + 1) % 4) * count]);
        }

        if (lastRowSegments && lastY == y)
            mergeSegments(lastRowSegments, lastRowSegmentCount, &segments[lastSegment], count);

        lastRowSegments = &segments[lastSegment + 2 * count];
        lastRowSegmentCount = count;
        lastSegment += 4 * count;
        lastY = y + rect[0].height();
        rect += count;
    }

    for (int i = 0; i < lastSegment; ++i) {
        Segment *segment = &segments[i];
        if (!segment->added)
            addSegmentsToPath(segment, result);
    }

    return result;
}
