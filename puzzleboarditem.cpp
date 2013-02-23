
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
    _clearNodes = false;
    _previousPuzzlePieces = 0;

    connect(this, SIGNAL(widthChanged()), this, SLOT(updateGame()));
    connect(this, SIGNAL(heightChanged()), this, SLOT(updateGame()));
    connect(_game, SIGNAL(gameStarted()), this, SLOT(update()));
    connect(_game, SIGNAL(loadProgressChanged(int)), this, SLOT(update()));
    connect(this, SIGNAL(visibleChanged()), this, SLOT(clearNodes()));

    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
    setFlag(QQuickItem::ItemHasContents, true);
}

PuzzleBoardItem::~PuzzleBoardItem()
{
    qDeleteAll(_textures);
    _textures.clear();
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

void PuzzleBoardItem::clearNodes()
{
    // At the next update, delete all the SG nodes
    _clearNodes = true;
    update();
}

QSGNode *PuzzleBoardItem::updatePaintNode(QSGNode *mainNode, UpdatePaintNodeData *)
{
    // If all the nodes need to be cleared, delete the main node
    // Since all the others have OwnedByParent they will be deleted too.
    if (_clearNodes && mainNode)
    {
        delete mainNode;
        mainNode = 0;
        qDeleteAll(_textures);
        _textures.clear();
        _transformNodes.clear();
        _pieceTextureNodes.clear();
        _strokeTextureNodes.clear();
        _clearNodes = false;
    }

    // Create the main node if it doesn't exist yet
    if (!mainNode)
    {
        mainNode = new QSGNode();
        mainNode->setFlag(QSGNode::OwnedByParent);
    }

    // Order the puzzle pieces by z value (ascending)
    QList<PuzzlePiece*> puzzleItems = _game->puzzleItems().toList();
    qSort(puzzleItems.begin(), puzzleItems.end(), PuzzlePiece::puzzleItemAscLessThan);

    // If the number of pieces has changed
    if (_previousPuzzlePieces != puzzleItems.count())
    {
        // Check for removed puzzle pieces
        foreach (PuzzlePiece *piece, _transformNodes.keys())
        {
            // If the piece no longer exists in the game, remove the node
            if (!puzzleItems.contains(piece))
            {
                QSGTransformNode *trn = _transformNodes[piece];
                _transformNodes.remove(piece);
                mainNode->removeChildNode(trn);

                // Remove its child nodes, those will be appended to another transform node
                trn->removeAllChildNodes();
                delete trn;
            }
        }

        // Check for newly added puzzle pieces
        foreach (PuzzlePiece *piece, puzzleItems)
        {
            if (!_transformNodes.contains(piece))
            {
                // Create a new transform node
                // (Child nodes will be appended to it later)
                _transformNodes[piece] = new QSGTransformNode();
                _transformNodes[piece]->setFlag(QSGNode::OwnedByParent);
            }
        }
    }

    // TODO: optimize out this if the order of the pieces hasn't changed
    // Remove all child nodes from the main node
    mainNode->removeAllChildNodes();

    foreach (PuzzlePiece *piece, puzzleItems)
    {
        // Calculate the transformation of this puzzle piece
        QPointF p = piece->mapToParent(QPointF(0, 0));
        QTransform transform = QTransform::fromTranslate(p.x(), p.y()).rotate(piece->rotation());

        // Find the transform node of this puzzle piece
        QSGTransformNode *trn = _transformNodes[piece];
        trn->setMatrix(QMatrix4x4(transform));
        mainNode->appendChildNode(trn);

        // Create child nodes for its primitives if they don't exist yet
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
                _textures.append(pieceTex);
            }

            if (!_strokeTextureNodes.contains(pr))
            {
                QSGTexture *strokeTex = view->createTextureFromImage(pr->stroke().toImage());
                QSGSimpleTextureNode *strokeNode = new QSGSimpleTextureNode();
                strokeNode->setRect(pr->strokeOffset().x(), pr->strokeOffset().y(), pr->stroke().width(), pr->stroke().height());
                strokeNode->setTexture(strokeTex);
                strokeNode->setFlag(QSGNode::OwnedByParent);
                _strokeTextureNodes[pr] = strokeNode;
                _textures.append(strokeTex);
            }
        }

        // TODO: optimize this out if the primitives of the piece haven't changed

        // Remove all child nodes (so that they can be readded in the correct order)
        trn->removeAllChildNodes();

        // Update the stroke nodes and append them
        foreach (const PuzzlePiecePrimitive *pr, piece->primitives())
        {
            QSGSimpleTextureNode *strokeNode = _strokeTextureNodes[pr];
            strokeNode->setRect(pr->strokeOffset().x(), pr->strokeOffset().y(), pr->stroke().width(), pr->stroke().height());
            trn->appendChildNode(strokeNode);
        }

        // Update the piece nodes and append them
        foreach (const PuzzlePiecePrimitive *pr, piece->primitives())
        {
            QSGSimpleTextureNode *pieceNode = _pieceTextureNodes[pr];
            pieceNode->setRect(pr->pixmapOffset().x(), pr->pixmapOffset().y(), pr->pixmap().width(), pr->pixmap().height());
            trn->appendChildNode(pieceNode);
        }
    }

    _previousPuzzlePieces = puzzleItems.count();
    return mainNode;
}
