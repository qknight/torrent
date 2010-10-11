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

  private:
    TorrentClient* torrentClient;
    QString dstDirectory;
    QString torrent;
    int m_uprate;
    int m_downrate;
    void update();
    void saveState();
    
  private Q_SLOTS:
    void startTorrentDownload();
    void finished(int state);

    void updateState(TorrentClient::State);
    void updatePeerInfo();
    void updateProgress(int);
    void updateDownloadRate(int);
    void updateUploadRate(int);
    void torrentStopped();
    void torrentError(TorrentClient::Error);
};

#endif
