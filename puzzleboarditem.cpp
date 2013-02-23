
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

#include <QQuickView>
#include <QGuiApplication>
#include <QSGSimpleTextureNode>
#include <QSGTransformNode>
#include <QSGTexture>

#include "puzzleboarditem.h"
#include "puzzle/puzzlepiece.h"
#include "puzzle/puzzlepieceprimitive.h"

extern QQuickView *view;

PuzzleBoardItem::PuzzleBoardItem(QQuickItem *parent)
    : QQuickItem(parent)
{
    _game = new PuzzleGame(this);
    _cleanNodes = false;

    connect(this, SIGNAL(widthChanged()), this, SLOT(updateGame()));
    connect(this, SIGNAL(heightChanged()), this, SLOT(updateGame()));
    connect(_game, SIGNAL(gameStarted()), this, SLOT(update()));
    connect(_game, SIGNAL(loadProgressChanged(int)), this, SLOT(update()));

    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
    setFlag(QQuickItem::ItemHasContents, true);
}

void PuzzleBoardItem::updateGame()
{
    _game->setWidth(this->width());
    _game->setHeight(this->height());
}

void PuzzleBoardItem::mousePressEvent(QMouseEvent *event)
{
    event->accept();
    _game->handleMousePress(event->button(), event->pos());
    update();
}

void PuzzleBoardItem::mouseReleaseEvent(QMouseEvent *event)
{
    event->accept();
    _game->handleMouseRelease(event->button(), event->pos());
}

void PuzzleBoardItem::mouseMoveEvent(QMouseEvent *event)
{
    event->accept();
    _game->handleMouseMove(event->pos());
    update();
}

void PuzzleBoardItem::touchEvent(QTouchEvent *event)
{
    event->accept();
    _game->handleTouchEvent(event);
    update();
}

QSGNode *PuzzleBoardItem::updatePaintNode(QSGNode *mainNode, UpdatePaintNodeData *)
{
    // Create the main node if not existing yet
    if (!mainNode)
    {
        mainNode = new QSGNode();
        mainNode->setFlag(QSGNode::OwnedByParent);
    }

    // Order the puzzle pieces by z value (ascending)
    QList<PuzzlePiece*> puzzleItems = _game->puzzleItems().toList();
    qSort(puzzleItems.begin(), puzzleItems.end(), PuzzlePiece::puzzleItemAscLessThan);

    // Remove all child nodes from the main node
    mainNode->removeAllChildNodes();

    // Iterate through the previously known puzzle pieces
    foreach (PuzzlePiece *piece, _transformNodes.keys())
    {
        // If the piece no longer exists in the game, remove the node
        if (!puzzleItems.contains(piece))
        {
            QSGTransformNode *trn = _transformNodes[piece];
            _transformNodes.remove(piece);

            qDebug() << "removing transform node for" << piece->puzzleCoordinates();
            trn->removeAllChildNodes();
            delete trn;
        }
    }

    foreach (PuzzlePiece *piece, puzzleItems)
    {
        QPointF p = piece->mapToParent(QPointF(0, 0));
        QTransform transform = QTransform::fromTranslate(p.x(), p.y()).rotate(piece->rotation());

        if (!_transformNodes.contains(piece))
        {
            _transformNodes[piece] = new QSGTransformNode();
            _transformNodes[piece]->setFlag(QSGNode::OwnedByParent);
        }

        QSGTransformNode *trn = _transformNodes[piece];
        trn->setMatrix(QMatrix4x4(transform));
        mainNode->appendChildNode(trn);

        foreach (const PuzzlePiecePrimitive *pr, piece->primitives())
        {
            if (!_pieceTextureNodes.contains(pr))
            {
                QSGTexture *pieceTex = view->createTextureFromImage(pr->pixmap().toImage());
                QSGSimpleTextureNode *pieceNode = new QSGSimpleTextureNode();
                pieceNode->setRect(pr->pixmapOffset().x(), pr->pixmapOffset().y(), pr->pixmap().width(), pr->pixmap().height());
                pieceNode->setTexture(pieceTex);
                pieceNode->setFlag(QSGNode::OwnedByParent);
                _pieceTextureNodes[pr] = pieceNode;
            }

            if (!_strokeTextureNodes.contains(pr))
            {
                QSGTexture *strokeTex = view->createTextureFromImage(pr->stroke().toImage());
                QSGSimpleTextureNode *strokeNode = new QSGSimpleTextureNode();
                strokeNode->setRect(pr->strokeOffset().x(), pr->strokeOffset().y(), pr->stroke().width(), pr->stroke().height());
                strokeNode->setTexture(strokeTex);
                strokeNode->setFlag(QSGNode::OwnedByParent);
                _strokeTextureNodes[pr] = strokeNode;
            }
        }

        trn->removeAllChildNodes();

        foreach (const PuzzlePiecePrimitive *pr, piece->primitives())
        {
            QSGSimpleTextureNode *strokeNode = _strokeTextureNodes[pr];
            strokeNode->setRect(pr->strokeOffset().x(), pr->strokeOffset().y(), pr->stroke().width(), pr->stroke().height());
            trn->appendChildNode(strokeNode);
        }

        foreach (const PuzzlePiecePrimitive *pr, piece->primitives())
        {
            QSGSimpleTextureNode *pieceNode = _pieceTextureNodes[pr];
            pieceNode->setRect(pr->pixmapOffset().x(), pr->pixmapOffset().y(), pr->pixmap().width(), pr->pixmap().height());
            trn->appendChildNode(pieceNode);
        }
    }

    return mainNode;
}
