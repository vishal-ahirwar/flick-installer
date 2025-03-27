#include "utild.h"
#include<QNetworkRequest>
#include<QtAlgorithms>
#include<QFile>
#include<QFileInfo>
//{QUrl("https://github.com/llvm/llvm-project/releases/download/llvmorg-19.1.7/clang+llvm-19.1.7-x86_64-pc-windows-msvc.tar.xz"),"../clang+llvm19.1.7-windows.tar.xz"}
const static QVector<QPair<QUrl,QString>> tools{
                          {QUrl("https://github.com/vishal-ahirwar/aura/releases/latest/download/utool.exe"),"../utool.exe"},
                          {QUrl("https://aka.ms/vs/17/release/vs_BuildTools.exe"),"../vs_build_tools.exe"},
                          {QUrl("https://github.com/ninja-build/ninja/releases/download/v1.12.1/ninja-win.zip"),"../ninja-win.zip"},
                          {QUrl("https://github.com/Kitware/CMake/releases/download/v3.31.5/cmake-3.31.5-windows-x86_64.zip"),"../cmake-windows.zip"},
                          {QUrl("https://github.com/vishal-ahirwar/aura/releases/latest/aura.exe"),"../aura.exe"}
};

Utild::Utild(QObject *parent)
    : QObject{parent}
{
    _manager=new QNetworkAccessManager(this);
}

void Utild::download(const QUrl &url, const QString &file_path)
{
    setDownloadProgress(0);
    setFilName(QFileInfo(file_path).fileName());
    QNetworkRequest req;
    _file=new QFile(file_path,this);
    req.setUrl(url);
    _reply=_manager->get(req);
    connect(_reply,&QNetworkReply::errorOccurred,this,&Utild::onError);
    connect(_reply,&QNetworkReply::downloadProgress,this,&Utild::downloadProgress);
    connect(_reply,&QNetworkReply::finished,this,&Utild::onDownloadFinished);
};


void Utild::onError(QNetworkReply::NetworkError err){
}
void Utild::downloadProgress(qint64 recieved,qint64 total){
    if(total==0)return;
    setDownloadProgress(static_cast<float>(recieved)/static_cast<float>(total));
};

void Utild::onDownloadFinished(){
    setDownloadProgress(1.0);
    if(!_file)return;
    if(!_file->open(QIODevice::WriteOnly)){
        _reply->deleteLater();
        return;
    };
    _file->write(_reply->readAll());
    _file->close();
    _reply->deleteLater();
    _file->deleteLater();
    downloadNext();
}

void Utild::setDownloadProgress(float newDownloadProgress)
{
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
    if (_file_name == newFile_name)
        return;
    _file_name = newFile_name;
    emit fileNameChanged();
}

void Utild::start()
{
    download(tools.at(tools.size()-1).first,tools.at(tools.size()-1).second);
};

void Utild::downloadNext(){
    static int index=tools.size()-1;
    index-=1;
    if(index>=0){
         download(tools.at(index).first,tools.at(index).second);
    }else{
        emit complete();
        return;
    }

}
