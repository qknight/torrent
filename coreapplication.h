#ifndef COREAPPLICATION_H
#define COREAPPLICATION_H

#include <QDebug>
#include <QObject>
#include <QString>
#include <QVector>

#include "torrentclient.h"

class CoreApplication : public QObject {
  Q_OBJECT

  public:
    CoreApplication();
    ~CoreApplication();
    void startTorrentDownload();
    
  private:
    TorrentClient* torrentClient;
    QString dstDirectory;
    QString torrent;
    
  private Q_SLOTS:
    void updateState(TorrentClient::State);
    void updatePeerInfo();
    void updateProgress(int);
    void updateDownloadRate(int);
    void updateUploadRate(int);
    void torrentStopped();
    void torrentError(TorrentClient::Error);
};

#endif
