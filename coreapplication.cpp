#include "coreapplication.h"
#include "ratecontroller.h"

CoreApplication::CoreApplication() {
  // the directory you want the torrent contents to be stored in
  dstDirectory="/tmp/evopedia/";
  // the local torrent file you want to download
  torrent = "wikipedia_sl_2010-08-12.torrent";
  torrentClient = new TorrentClient(this);
  // it is mendatory to set a default, if not it will not donwload anything 
  // and it will not find any seeds
  int rate = 5000;
  RateController::instance()->setUploadLimit(rate * 1024);
  RateController::instance()->setDownloadLimit(rate * 1024);
  startTorrentDownload();
}

void CoreApplication::startTorrentDownload() {
  // Setup the client connections
  connect(torrentClient, SIGNAL(stateChanged(TorrentClient::State)),
	  this, SLOT(updateState(TorrentClient::State)));
  connect(torrentClient, SIGNAL(peerInfoUpdated()),
	  this, SLOT(updatePeerInfo()));
  connect(torrentClient, SIGNAL(progressUpdated(int)),
	  this, SLOT(updateProgress(int)));
  connect(torrentClient, SIGNAL(downloadRateUpdated(int)),
	  this, SLOT(updateDownloadRate(int)));
  connect(torrentClient, SIGNAL(uploadRateUpdated(int)),
	  this, SLOT(updateUploadRate(int)));
  connect(torrentClient, SIGNAL(stopped()),
	  this, SLOT(torrentStopped()));
  connect(torrentClient, SIGNAL(error(TorrentClient::Error)),
	  this, SLOT(torrentError(TorrentClient::Error)));

    QString torrentName = dstDirectory + "" + torrent;

    if (!torrentClient->setTorrent(torrentName)) {
            qDebug() << "error: " << QString("The torrent file %1 cannot not be opened/resumed.").arg(torrentName);
            delete torrentClient;
            return;
     }
     qDebug() << dstDirectory << ", " << torrentName;
     torrentClient->setDestinationFolder(dstDirectory);
     QByteArray resumeState;// = settings.value("resumeState").toByteArray();
     torrentClient->setDumpedState(resumeState);

     torrentClient->start();
}

void CoreApplication::updateState(TorrentClient::State) {
    qDebug() << __PRETTY_FUNCTION__ << torrentClient->stateString();
}

void CoreApplication::updatePeerInfo() {
    qDebug() << __PRETTY_FUNCTION__ << "peers" << torrentClient->connectedPeerCount()
            << ", seeds: " << torrentClient->seedCount();
}

void CoreApplication::updateProgress(int percent) {
    qDebug() << __PRETTY_FUNCTION__ << percent << "% " << torrentClient->stateString();
}

void CoreApplication::updateDownloadRate(int rate) {
    static int previousrate = 0;
    if (previousrate == rate)
        return;
    previousrate = rate;
    qDebug() << __PRETTY_FUNCTION__ << rate << " download rate kb/s";
}

void CoreApplication::updateUploadRate(int rate) {
    static int previousrate = 0;
    if (previousrate == rate)
        return;
    previousrate = rate;
    qDebug() << __PRETTY_FUNCTION__ << rate << " upload rate kb/s";
}

void CoreApplication::torrentStopped() {
    qDebug() << __PRETTY_FUNCTION__;
    //torrentClient->deleteLater();
}

void CoreApplication::torrentError(TorrentClient::Error) {
    qDebug() << __PRETTY_FUNCTION__ << torrentClient->errorString();
}

CoreApplication::~CoreApplication() {

}

