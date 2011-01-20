#ifndef PUZZLEPIECE_H
#define PUZZLEPIECE_H

#include <QtCore>

class PuzzlePiece
{
private:
    QPoint _position;
    QList<PuzzlePiece*> _neighbours;

public:
    explicit PuzzlePiece();

    const QPoint &position() const;
    const QList<PuzzlePiece*> &neighbours() const;

    void setPosition(const QPoint &p);
    void addNeighbour(PuzzlePiece *piece);
    void removeNeighbour(PuzzlePiece *piece);

    bool isNeighbourOf(const PuzzlePiece *piece) const;
    virtual bool merge(PuzzlePiece *piece);

    static void setNeighbours(QList<PuzzlePiece*> *pieces, int x, int y);
    static PuzzlePiece *find(QList<PuzzlePiece*> *pieces, QPoint position);

};

#endif // PUZZLEPIECE_H
