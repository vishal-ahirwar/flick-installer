#include "InstallerWorker.h"
#include<QUrl>
#include<QTimer>

InstallerWorker::InstallerWorker(const QVector<QPair<QUrl, QString>>& tools, const QString& path): tools(tools), installPath(path) {};
void InstallerWorker::run() {
    QDir homeDir(installPath);

    if (!homeDir.exists() && !homeDir.mkpath(".")) {
        emit updateStatus("Failed to create aura directory");
        emit finished();
        return;
    }

    float progressStep = 1.0f / tools.size();
    float currentProgress = 0.2f;

    for (const auto& tool : tools) {
        QString file = tool.second;
        emit updateStatus("Installing " + file);

        if (!QFile::exists(file)) {
            qDebug() << file << " doesn't exist";
            emit updateStatus("Clean Install required!");
            continue;
        }

        QFileInfo fileInfo(file);

        if (fileInfo.suffix() == "zip" || fileInfo.suffix() == "xz"||fileInfo.suffix()=="tar") {
            QProcess *process=new QProcess(this);
            QStringList args;

            if (fileInfo.suffix() == "zip") {
                args << "-xf" << file << "-C" << installPath;
            };
            //TODO
            if(fileInfo.suffix()=="xz"){
                args << "-xvJf" << QDir::toNativeSeparators(file) << "-C" << QDir::toNativeSeparators(installPath);
            };
            if(fileInfo.suffix()=="tar"){
                args << "-xvf" << QDir::toNativeSeparators(file) << "-C" << QDir::toNativeSeparators(installPath);
            }
            process->start("tar", args);
            connect(process, &QProcess::readyReadStandardError, this, [process]() {
                QByteArray errorOutput = process->readAllStandardError();
                qDebug() << "Error:" << errorOutput;
            });

            connect(process, &QProcess::readyReadStandardOutput, this, [process]() {
                QByteArray output = process->readAllStandardOutput();
                qDebug() << "Output:" << output;
            });
            if (!process->waitForStarted()) {
                emit updateStatus("Failed to start extraction for " + file);
                continue;
            }

            if (!process->waitForFinished()) {
                QByteArray output = process->readAllStandardOutput();
                qDebug() << output<<process->readAllStandardError();
                emit updateStatus("Extraction failed for " + file + ": " + process->readAllStandardError());
                continue;
            }
            process->deleteLater();
            emit updateStatus("Successfully installed " + file);
        }
        else if (file.startsWith("vs")) {
            if(QFile("C:\\Program Files (x86)\\Microsoft Visual Studio\\Installer\\vswhere.exe").exists()){
                emit updateStatus("Visual Studio Already installed Install Desktop Dev With C++");
                qDebug()<<"Visual Studio Already installed Install Desktop Dev With C++";
                continue;
            };
            QProcess*process=new QProcess(this);
            process->start(file,QStringList()<<"--wait"<<"--config"<<"aura.vsconfig");
            connect(process, &QProcess::readyReadStandardError, this, [process]() {
                QByteArray errorOutput = process->readAllStandardError();
                qDebug() << "Error:" << errorOutput;
            });

            connect(process, &QProcess::readyReadStandardOutput, this, [process]() {
                QByteArray output = process->readAllStandardOutput();
                qDebug() << "Output:" << output;
            });
            if (!process->waitForStarted()) {
                emit updateStatus("Failed to start install  " + file);
                continue;
            }

            if (!process->waitForFinished()) {
                emit updateStatus("Installation failed for " + file + ": " + process->readAllStandardError());
                continue;
            }
            process->deleteLater();
            emit updateStatus("Successfully installed " + file);
        }
        else if (file.endsWith("aura.exe")) {
            QString dest = installPath + "/" + QFileInfo(file).fileName();
            if (QFile::exists(dest)) {
                QFile::remove(dest);
            }
            if (QFile::copy(file, dest)) {
                emit updateStatus("Successfully installed " + file);
            } else {
                emit updateStatus("Failed to copy " + file);
            }
        }else{
            qDebug()<<"Ignoring "<<file;
            continue;
        }

        currentProgress += progressStep;
        emit updateProgress(currentProgress);
    }

    emit finished();
}
