#ifndef UTILD_H
#define UTILD_H

#include <QObject>
#include<QNetworkAccessManager>
#include<QNetworkReply>
#include<QFile>
#include<QMap>
#include<QThread>
class Utild : public QObject
{
    Q_OBJECT
    Q_PROPERTY(float download_progress READ getDownloadProgress WRITE setDownloadProgress NOTIFY downloadProgressChanged FINAL)
    Q_PROPERTY(QString file_name READ getFileName WRITE setFilName NOTIFY fileNameChanged FINAL)
    Q_PROPERTY(bool can_procceed READ can_procceed WRITE setCan_procceed NOTIFY can_procceedChanged FINAL)
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
    Q_INVOKABLE void reboot();
    bool can_procceed() const;
    void setCan_procceed(bool newCan_procceed);

signals:
    void downloadFailed(QString error);
    void downloadProgressChanged();

    void fileNameChanged();

    void complete();

    void downloadingFinished();

    void installingFinished();
    void addToPathFinished();
    void can_procceedChanged();

private slots:
    void onDowloadingFinished();
    void  onInstallingFinished();
    void onAddToPathFinished();
private:
    float _download_progress{};
    QString _file_name{};
    QNetworkAccessManager _m{};
    qint16 _current_index{};
    bool m_can_procceed{};
    bool download_flag{true};
private:
    void onError(QNetworkReply::NetworkError);
    void downloadProgress(qint64,qint64);
    void onDownloadFinished();
    void downloadNext();
    void installing();
    void addToPath();
    void canProceed();
    void errorOccured(QString error);
    void installVcpkg();
    void scanWhatNeedsToBeInstalled();
};

#endif // UTILD_H
