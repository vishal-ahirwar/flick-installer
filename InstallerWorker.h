#ifndef INSTALLERWORKER_H
#define INSTALLERWORKER_H

#include <QObject>
#include <QProcess>
#include <QVector>
#include <QPair>
#include <QDir>

class InstallerWorker : public QObject {
    Q_OBJECT

public:
    InstallerWorker(const QVector<QPair<QUrl, QString>>& tools, const QString& path);

signals:
    void updateStatus(const QString& status);
    void updateProgress(float progress);
    void finished();

public slots:
    void run();

private:
    QVector<QPair<QUrl, QString>> tools;
    QString installPath;
};

#endif // INSTALLERWORKER_H
