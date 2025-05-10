#include "utild.h"
#include<QNetworkRequest>
#include<QtAlgorithms>
#include<QFile>
#include<QFileInfo>
#include<QProcess>
#include<QStandardPaths>
#include<QDir>
#include<QTimer>
#include<QSettings>
#include<QDirIterator>
#include<QThread>
#include<InstallerWorker.h>
#include<QCoreApplication>
void whatExist(const QString&file);
namespace fs=std::filesystem;
// /
//
static QVector<QPair<QUrl,QString>> tools{{QUrl("https://aka.ms/vs/17/release/vs_BuildTools.exe"),"vs_build_tools.exe"},
                          {QUrl("https://github.com/ninja-build/ninja/releases/download/v1.12.1/ninja-win.zip"),"ninja-win.zip"},
                          {QUrl("https://github.com/Kitware/CMake/releases/download/v3.31.5/cmake-3.31.5-windows-x86_64.zip"),"cmake-windows.zip"},
                          {QUrl("https://github.com/vishal-ahirwar/solix/releases/latest/download/solix.exe"),"solix.exe"},
                          {QUrl("https://raw.githubusercontent.com/vishal-ahirwar/solix/refs/heads/master/res/solix.vsconfig"),"solix.vsconfig"},
                        {QUrl("https://github.com/llvm/llvm-project/releases/download/llvmorg-19.1.7/clang+llvm-19.1.7-x86_64-pc-windows-msvc.tar.xz"),"clang+llvm19.1.7-windows.tar.xz"}
};

Utild::Utild(QObject *parent)
    : QObject{parent},_current_index{0}
{
    // Connect signals with queued connection to ensure proper sequence
    connect(this, &Utild::downloadingFinished, this, &Utild::onDowloadingFinished, Qt::QueuedConnection);
    connect(this, &Utild::installingFinished, this, &Utild::addToPath, Qt::QueuedConnection);
    connect(this, &Utild::addToPathFinished, this, &Utild::onAddToPathFinished, Qt::QueuedConnection);
    connect(this,&Utild::downloadFailed,this,&Utild::errorOccured);
    setFilName("Checking if Required Tools are installed or not");
    QTimer::singleShot(1000,[this](){canProceed();});
};


void Utild::onError(QNetworkReply::NetworkError err){

}
void Utild::downloadProgress(qint64 recieved,qint64 total){
    if(total==0)return;
    setDownloadProgress(static_cast<float>(recieved)/static_cast<float>(total));
};

void Utild::onDownloadFinished(){
    setDownloadProgress(1.0);

    downloadNext();
}

void Utild::setDownloadProgress(float newDownloadProgress)
{
    qDebug() << newDownloadProgress;
    if (qFuzzyCompare(_download_progress, newDownloadProgress))
        return;
    _download_progress = newDownloadProgress;
    emit downloadProgressChanged();
}

QString Utild::fileName() const
{
    return _file_name;
}

void Utild::setFilName(const QString &newFile_name)
{
    qDebug() << newFile_name;
    if (_file_name == newFile_name)
        return;
    _file_name = newFile_name;
    emit fileNameChanged();
}

void Utild::start()
{
    // for(const auto&tool:tools){
    //     whatExist(tool.second);
    // }
    _current_index = 0;  // Reset index when starting
    download(tools.first().first, tools.first().second);
}

void Utild::reboot()
{
#ifdef Q_OS_WIN
    QProcess::startDetached("shutdown", {"/r", "/t", "0"});
#elif defined(Q_OS_LINUX) || defined(Q_OS_UNIX)
    QProcess::startDetached("sudo", {"reboot"});
#elif defined(Q_OS_MAC)
    QProcess::startDetached("sudo", {"shutdown", "-r", "now"});
#endif
    QCoreApplication::quit();
};

void whatExist(const QString&file){
    if(QFile(file).exists()){
        qDebug()<<QFileInfo(file).completeSuffix();
        qDebug()<<file<<" exist";
    }else{
        qDebug()<<file<<" doesn't exist";
    }
}
void Utild::download(const QUrl &url, const QString &file_path) {
    if (!download_flag) {
        setFilName("Download halted due to previous failure.");
        return;
    }

    setDownloadProgress(0);
    if(QFile(file_path).exists()){
        setFilName(file_path+" already exist!");
        downloadNext();
        return;
    }
    setFilName("Downloading " + QFileInfo(file_path).fileName());
    QTimer::singleShot(1000,[this,file_path,url](){
         QNetworkRequest req{url};
         auto res = _m.get(req);

         connect(res, &QNetworkReply::errorOccurred, this, [this, file_path, res]() {
             emit downloadFailed("Error while downloading " + file_path + ": " + res->errorString());
         });

         connect(res, &QNetworkReply::downloadProgress, this, [this](auto rec, auto total) {
             if (total > 0) {
                 setDownloadProgress(static_cast<float>(rec) / static_cast<float>(total));
             }
         });

         connect(res, &QNetworkReply::finished, this, [res, this, file_path]() {
             setDownloadProgress(1.0);

             if (res->error() == QNetworkReply::NoError) {
                 QFile file(file_path);
                 if (file.open(QIODevice::WriteOnly|QIODevice::Truncate | QIODevice::Unbuffered)) {
                     file.write(res->readAll());
                     file.close();
                     setFilName("File " + file_path + " saved successfully.");
                 } else {
                     setFilName ("Failed to save file: " + file_path);
                 }
             } else {
                 setFilName("Download failed for: " + file_path + " with error: " + res->errorString());
             }

             res->deleteLater();
             downloadNext();
         });
     });

}

// Modify downloadNext() to increment instead of decrement
void Utild::downloadNext() {
    _current_index++;

    if (_current_index < tools.size()) {
        download(tools.at(_current_index).first, tools.at(_current_index).second);
    } else {
         setFilName("Downloading Finished");
         qDebug() << "Downloading finished";
         emit downloadingFinished();
    }
}
void Utild::installing() {
    setFilName("Installing tools...");

    QTimer::singleShot(1000,[this](){
        QString path = QDir::homePath() + "/solix";
        QThread* thread = new QThread;
        InstallerWorker* worker = new InstallerWorker(tools, path);

        worker->moveToThread(thread);

        connect(thread, &QThread::started, worker, &InstallerWorker::run);
        connect(worker, &InstallerWorker::updateStatus, this, &Utild::setFilName);
        connect(worker, &InstallerWorker::updateProgress, this, &Utild::setDownloadProgress);
        connect(worker, &InstallerWorker::finished, this, [this,thread, worker]() {
            thread->quit();
            thread->wait();
            worker->deleteLater();
            thread->deleteLater();
            emit installingFinished();
        });

        thread->start();
    });
}

void Utild::addToPath() {
    setFilName("Adding tools to PATH...");
    setDownloadProgress(0.8);
    QTimer::singleShot(1000,[this](){
        auto home=QDir::homePath();
        home+="/solix";
        home=QDir::cleanPath(home);
        auto r=QSettings("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment", QSettings::NativeFormat).value("path").toString();
        QDirIterator path(home,QDir::NoDotDot|QDir::AllEntries);
        while(path.hasNext()){
            auto p=path.next();

            if(path.fileInfo().isFile()){
                continue;
            };
            if(path.fileInfo().isDir()){
                if(r.contains(p,Qt::CaseInsensitive)){
                    continue;
                };

                if((QDir::cleanPath(p)!=home)&&(!QDir::cleanPath(p).endsWith("vcpkg"))){
                    p+="/bin";
                }
                qDebug()<<"adding to path : "<<QDir::cleanPath(p);
                r.append(";"+QDir::cleanPath(p));
            }
        };
        // Notify the system to reload environment variables
        QProcess::execute("powershell", {"/c", "setx Path \"" + r + "\""});
        emit addToPathFinished();
    });

}


void Utild::onDowloadingFinished(){
    installing();
};
void Utild::onInstallingFinished(){
    addToPath();
};
void Utild::onAddToPathFinished(){
    setDownloadProgress(1.0);
    setFilName("solix has been succesfully installed!");
    emit complete();
};


bool Utild::can_procceed() const
{
    return m_can_procceed;
}

void Utild::setCan_procceed(bool newCan_procceed)
{
    if (m_can_procceed == newCan_procceed)
        return;
    m_can_procceed = newCan_procceed;
    emit can_procceedChanged();
}
void Utild::canProceed()
{
    QProcess process;
    process.start("git", QStringList() << "--version");

    if (!process.waitForFinished() || process.error() == QProcess::FailedToStart) {
        setCan_procceed(false);
        setFilName("Git is not installed or failed to start");
        return;
    }
    scanWhatNeedsToBeInstalled();
    setCan_procceed(true);
}

void Utild::errorOccured(QString error)
{
    setFilName(error);
    download_flag=false;
}

void Utild::scanWhatNeedsToBeInstalled()
{
    QVector<QString>toolsNeedsToBeInstalled{"ninja","cmake","clang","vcpkg"};
    for(const auto&tool:toolsNeedsToBeInstalled)
    {
        QProcess process;
        process.start(tool, QStringList() << "--version");

        if (!process.waitForFinished() || process.error() == QProcess::FailedToStart) {
            setCan_procceed(false);
            setFilName("Git is not installed or failed to start");
            continue;
        }
        for(auto&pair:tools)
        {
            if(pair.second.contains(tool))
            {
                tools.removeOne(pair);
            }
        }
    }
    if(QFile("C:\\Program Files (x86)\\Microsoft Visual Studio\\Installer\\vswhere.exe").exists()){

        tools.removeFirst();
    };
}

