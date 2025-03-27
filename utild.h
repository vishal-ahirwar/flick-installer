#ifndef UTILD_H
#define UTILD_H

#include <QObject>
#include<QNetworkAccessManager>
#include<QNetworkReply>
#include<QFile>
#include<QMap>
class Utild : public QObject
{
    Q_OBJECT
    Q_PROPERTY(float download_progress READ getDownloadProgress WRITE setDownloadProgress NOTIFY downloadProgressChanged FINAL)
    Q_PROPERTY(QString file_name READ getFileName WRITE setFilName NOTIFY fileNameChanged FINAL)
public:
    explicit Utild(QObject *parent = nullptr);
    void download(const QUrl&url,const QString&file_path);
    void setDownloadProgress(float new_download_progress);
    Q_INLINE_TEMPLATE float getDownloadProgress()const{
        return _download_progress;
    };
    Q_INLINE_TEMPLATE QString getFileName()const{
        return _file_name;
    }
    QString fileName() const;
    void setFilName(const QString &new_file_name);
    Q_INVOKABLE void start();
signals:
    void downloadProgressChanged();

    void fileNameChanged();

    void complete();
private:
    QNetworkAccessManager*_manager{};
    QNetworkReply*_reply{};
    float _download_progress;
    QString _file_name;
    QFile*_file{};
private:
    void onError(QNetworkReply::NetworkError);
    void downloadProgress(qint64,qint64);
    void onDownloadFinished();
    void downloadNext();
};

#endif // UTILD_H
