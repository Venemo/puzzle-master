
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

#ifndef PUZZLEBOARDITEM_H
#define PUZZLEBOARDITEM_H

#include <QQuickItem>
#include <QMap>

#include "puzzle/puzzlegame.h"

class QSGTexture;
class QSGSimpleTextureNode;
class QSGTransformNode;
class PuzzlePiece;
class PuzzlePiecePrimitive;

class PuzzleBoardItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(PuzzleGame* game READ game NOTIFY gameChanged)

    QMap<PuzzlePiece*, QSGTransformNode*> _transformNodes;
    QMap<const PuzzlePiecePrimitive*, QSGSimpleTextureNode*> _pieceTextureNodes;
    QMap<const PuzzlePiecePrimitive*, QSGSimpleTextureNode*> _strokeTextureNodes;
    QList<QSGTexture*> _textures;
    PuzzleGame *_game;

    bool _clearNodes;
    int _previousPuzzlePieces;

public:
    explicit PuzzleBoardItem(QQuickItem *parent = 0);
    PuzzleGame *game() { return _game; }

protected:
    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void touchEvent(QTouchEvent *event);

protected slots:
    void updateGame();
    void clearNodes();

signals:
    void gameChanged();

};

#endif // PUZZLEBOARDITEM_H
