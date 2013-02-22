
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

#include <QCoreApplication>
#include <QTranslator>
#include <QDebug>
#include <QFile>

void loadTranslations()
{
    // Checking for translations

    QString langCode(getenv("LANG"));
    if (langCode.isEmpty() || langCode == "C" || !langCode.contains("_"))
        langCode = QLocale::system().name();
    if (langCode.contains('.'))
        langCode = langCode.mid(0, langCode.lastIndexOf('.'));

    // Uncomment for testing purposes
    //langCode = "hu_HU";

    qDebug() << "Puzzle Master's current language code is" << langCode;

    if (QFile::exists(":/translations/puzzle-master_" + langCode + ".qm"))
    {
        qDebug() << "A translation for the language code" << langCode << "exists, loading it";
        QTranslator *translator = new QTranslator(QCoreApplication::instance());
        translator->load("puzzle-master_" + langCode, ":/translations");
        QCoreApplication::installTranslator(translator);
    }
    else
    {
        qDebug() << "There is NO translation for the language code" << langCode;
    }
}
