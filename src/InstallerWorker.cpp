#include "InstallerWorker.h"
#include <QTimer>
#include <QUrl>


InstallerWorker::InstallerWorker(const QVector<QPair<QUrl, QString>> &tools,
                                 const QString &path)
    : tools(tools), installPath(path) {

      };
void InstallerWorker::run() {
  QDir homeDir(installPath);

  if (!homeDir.exists() && !homeDir.mkpath(".")) {
    emit updateStatus("Failed to create flick directory");
    emit finished();
    return;
  }

  float progressStep = 1.0f / tools.size();
  float currentProgress = 0.2f;
  installVcpkg();
  for (const auto &tool : tools) {
    QString file = tool.second;
    emit updateStatus("Installing " + file);

    if (!QFile::exists(file)) {
      qDebug() << file << " doesn't exist";
      emit updateStatus("Clean Install required!");
      continue;
    }

    QFileInfo fileInfo(file);

    if (fileInfo.suffix() == "zip" || fileInfo.suffix() == "xz" ||
        fileInfo.suffix() == "tar") {
      QProcess *process = new QProcess(this);
      QStringList args;

      if (fileInfo.suffix() == "zip") {
        args << "-xf" << file << "-C" << installPath;
      };
      // TODO
      if (fileInfo.suffix() == "xz") {
        args << "-xvJf" << QDir::toNativeSeparators(file) << "-C"
             << QDir::toNativeSeparators(installPath);
      };
      if (fileInfo.suffix() == "tar") {
        args << "-xvf" << QDir::toNativeSeparators(file) << "-C"
             << QDir::toNativeSeparators(installPath);
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
        qDebug() << output << process->readAllStandardError();
        emit updateStatus("Extraction failed for " + file + ": " +
                          process->readAllStandardError());
        continue;
      }
      process->deleteLater();
      emit updateStatus("Successfully installed " + file);
    } else if (file.startsWith("vs")) {
      if (QFile("C:\\Program Files (x86)\\Microsoft Visual "
                "Studio\\Installer\\vswhere.exe")
              .exists()) {
        emit updateStatus(
            "Visual Studio Already installed Install Desktop Dev With C++");
        qDebug()
            << "Visual Studio Already installed Install Desktop Dev With C++";
        continue;
      };
      QProcess *process = new QProcess(this);
      process->start(file, QStringList()
                               << "--wait" << "--config" << "flick.vsconfig");
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
        emit updateStatus("Installation failed for " + file + ": " +
                          process->readAllStandardError());
        continue;
      }
      process->deleteLater();
      emit updateStatus("Successfully installed " + file);
    } else if (file.endsWith("flick.exe")) {
      QString dest = installPath + "/" + QFileInfo(file).fileName();
      if (QFile::exists(dest)) {
        QFile::remove(dest);
      }
      if (QFile::copy(file, dest)) {
        emit updateStatus("Successfully installed " + file);
      } else {
        emit updateStatus("Failed to copy " + file);
      }
    } else {
      qDebug() << "Ignoring " << file;
      continue;
    }

    currentProgress += progressStep;
    emit updateProgress(currentProgress);
  }
#if _WIN32
  ifVsAlreadyInstalled();
#endif
  emit finished();
}

void InstallerWorker::installVcpkg() {
    QProcess _process;
    _process.start("vcpkg", QStringList() << "--version");

    if (!_process.waitForFinished() || _process.error() == QProcess::FailedToStart) {
        //continue
    } else {
        return;
    }
  emit updateStatus("Installing vcpkg...");
  emit updateProgress(0.2);
  QProcess process;
  connect(&process, &QProcess::errorOccurred, this,
          [this](QProcess::ProcessError error) {
            switch (error) {
            case QProcess::Timedout:
              emit updateStatus("timeout");
              break;
            case QProcess::Crashed:
              emit updateStatus("crashed");
              break;
            case QProcess::UnknownError:
              emit updateStatus("Unknown error");
              break;
            }
          });

  auto path = QDir::homePath() + "/vcpkg";

  process.start("git", QStringList()
                           << "clone"
                           << "https://github.com/microsoft/vcpkg.git" << path);
  process.waitForFinished();
  {
    QProcess c;
    c.start("powershell", QStringList() << "-Command"
                                        << QString("setx VCPKG_ROOT \"%1\"")
                                               .arg(QDir::cleanPath(path)));
    c.waitForFinished();
  }
  {
    QProcess c;
#ifdef _WIN32
    c.start("powershell", QStringList() << path + "/bootstrap-vcpkg.bat");
#else
    c.start("bash", QStringList() << path + "/bootstrap-vcpkg.sh");
#endif
    c.waitForFinished();
  }
}

void InstallerWorker::ifVsAlreadyInstalled() {
  QProcess *process = new QProcess(this);
  process->start(
      "C:\\Program Files (x86)\\Microsoft Visual Studio\\Installer\\setup.exe",
      QStringList() << "modify" << "--installPath"
                    << "C:\\Program Files (x86)\\Microsoft Visual "
                       "Studio\\2022\\BuildTools"
                    << "--config" << "flick.vsconfig");
  connect(process, &QProcess::readyReadStandardError, this, [process]() {
    QByteArray errorOutput = process->readAllStandardError();
    qDebug() << "Error:" << errorOutput;
  });

  connect(process, &QProcess::readyReadStandardOutput, this, [process]() {
    QByteArray output = process->readAllStandardOutput();
    qDebug() << "Output:" << output;
  });
  if (!process->waitForStarted()) {
    emit updateStatus("Failed to start install Visual C++ Component");
    return;
  }

  if (!process->waitForFinished()) {
    emit updateStatus("Installation failed " + process->readAllStandardError());
    return;
  }
  process->deleteLater();
  emit updateStatus("Successfully installed Desktop Development with C++");
}
