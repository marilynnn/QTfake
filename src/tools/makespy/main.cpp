/***************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact:  Qt Software Information (qt-info@nokia.com)
**
**
** Non-Open Source Usage
**
** Licensees may use this file in accordance with the Qt Beta Version
** License Agreement, Agreement version 2.2 provided with the Software or,
** alternatively, in accordance with the terms contained in a written
** agreement between you and Nokia.
**
** GNU General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU General
** Public License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the packaging
** of this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
**
** http://www.fsf.org/licensing/licenses/info/GPLv2.html and
** http://www.gnu.org/copyleft/gpl.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt GPL Exception
** version 1.3, included in the file GPL_EXCEPTION.txt in this package.
**
***************************************************************************/

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QProcess>
#include <QStringList>
#include <QTextStream>
#include <QTextEdit>

class Process : public QProcess
{
    Q_OBJECT
public:
    Process();

    QString output() const { return m_output; }
    QString error() const { return m_error; }

private slots:
    void standardErrorReady();
    void standardOutputReady();

private:
    QString m_output;
    QString m_error;
};

Process::Process()
{
    connect(this, SIGNAL(readyReadStandardError()),
        this, SLOT(standardErrorReady()));
    connect(this, SIGNAL(readyReadStandardOutput()),
        this, SLOT(standardOutputReady()));
}

void Process::standardErrorReady()
{
    m_error += readAllStandardError();
}

void Process::standardOutputReady()
{
    m_output += readAllStandardOutput();
}


class MakeProcess : public Process
{
public:
    void handleOutput();

private:
    void handleMakeLine(const QString &line);
    void handleGccLine(const QString &line);

    QStringList m_dirStack;

    QVector<QString> m_options;
    QStringList m_sourceFiles;
    QStringList m_headerFiles;
};

void MakeProcess::handleOutput()
{
    QStringList lines = output().split('\n');
    m_dirStack.clear();
    m_dirStack.append(workingDirectory());
    foreach (const QString &line, lines) {
        qDebug() << "LINE  : " << line;
        if (line.startsWith("make["))
            handleMakeLine(line);
        else if (line.startsWith("gcc") || line.startsWith("g++"))
            handleGccLine(line);
        else
            qDebug() << "IGNORE: " << line;
    }
}

void MakeProcess::handleMakeLine(const QString &line)
{
    int pos1 = line.indexOf('`');
    int pos2 = line.indexOf('\'');
    if (pos1 >= 0 && pos2 >= 0) {
        QString dir = line.mid(pos1 + 1, pos2 - pos1 - 1);
        qDebug() << "MAKE" << pos1 << pos2 << dir;
        if (line.contains(": Entering directory")) {
            qDebug() << "ENTER: " << dir;
            m_dirStack.append(dir);
        } else if (line.contains(": Leaving directory")) {
            qDebug() << "LEAVE: " << dir;
            Q_ASSERT(m_dirStack.last() == dir);
            (void) m_dirStack.takeLast();
        }
    }
}

QStringList parseLine(const QString &line)
{
    QStringList result;
    QString word;
    bool quoted = false;
    bool escaped = false;
    for (int i = 0; i != line.size(); ++i) {
        char c = line.at(i).unicode();
        if (c == '\'') {
            escaped = true;
            continue;
        }
        if (c == '\"' && !escaped) {
            quoted = !quoted;
        } else if (c == ' ' && !quoted) {
            if (!word.isEmpty())
                result.append(word);
            word.clear();
        } else {
            word += c;
        }
    }
    if (!word.isEmpty())
        result.append(word);
    return result;
}

void MakeProcess::handleGccLine(const QString &line)
{
    QStringList args = parseLine(line);
    qDebug() << "GCC: " << args;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QStringList args = app.arguments();
    QString buildName = ".makespybuild/gdb";
    //args << "

    QDir oldDir = QDir::current();
    QDir buildDir("/home/apoenitz/gdb/archer");
    buildDir.mkdir(buildName);
    buildDir.cd(buildName);
    QDir::setCurrent(buildDir.absolutePath());

/*
    Process configure;
    configure.setWorkingDirectory(buildDir.absolutePath());
    configure.start("../configure", QStringList());
    configure.waitForFinished();
    qDebug() << configure.errorString();
    qDebug() << configure.error();
    qDebug() << configure.output();
*/
    MakeProcess make;
    make.setWorkingDirectory(buildDir.absolutePath());
    make.start("make", QStringList());
    make.waitForFinished();
    make.handleOutput();
    qDebug() << make.errorString();
    qDebug() << make.error();
    qDebug() << make.output();

    Process clean;
    clean.setWorkingDirectory(buildDir.absolutePath());
    clean.start("make", QStringList() << "clean");
    clean.waitForFinished();
    qDebug() << clean.errorString();
    qDebug() << clean.error();
    qDebug() << clean.output();

    QTextEdit edit;
    edit.setText(make.error() + make.output());
    edit.resize(800, 600);
    edit.show();

    return app.exec();
    return 0;
}

#include "main.moc"
