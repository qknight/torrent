#include <QStringList>
#include <QCoreApplication>
#include <QSettings>
#include "coreapplication.h"
#include "ratecontroller.h"

CoreApplication::CoreApplication() {
}

void CoreApplication::startTorrentDownload() {
    QStringList arguments = QCoreApplication::arguments();
    if (arguments.size() != 3) {
        qDebug() << "error: invalid number of arguments given";
        qDebug() << QString("usage: %1 (torrent file) (destination directory)").arg(arguments[0]);
        emit finished(1);
    } else {
        // the local torrent file you want to download
        torrent = arguments[1];

        // the directory you want the torrent contents to be stored in
        dstDirectory=arguments[2];

        torrentClient = new TorrentClient(this);

        // NOTE: it is mendatory to set a default, if not:
        //        - it will not donwload anything
        //        - and it will not find any seeds
        // NOTE: i changed the inline constructor call and now there is a default of 10MiB/s
        //       so the lines below are only needed if one wants to change that default
        //int rate = 358400; // 1024*350 = 358400 kib/s ~ 350kb/s
        //RateController::instance()->setUploadLimit(rate);
        //RateController::instance()->setDownloadLimit(rate);

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

        if (!torrentClient->setTorrent(torrent)) {
            qDebug() << "error: " << QString("The torrent file %1 cannot not be opened/resumed.").arg(torrent);
            emit finished(1);
        }
        torrentClient->setDestinationFolder(dstDirectory);

        QByteArray resumeState;

        QSettings settings("Trolltech", "Torrent");
        if (settings.value("torrentName") == torrent && settings.value("dstDirectory") == dstDirectory) {
            resumeState = settings.value("resumeState").toByteArray();
            torrentClient->setUploadedBytes(settings.value("uploadedBytes").toInt());
            torrentClient->setDownloadedBytes(settings.value("downloadedBytes").toInt());
        }

        torrentClient->setDumpedState(resumeState);
        torrentClient->start();
    }
}

void CoreApplication::updateState(TorrentClient::State s) {
    if (s == TorrentClient::Endgame || s == TorrentClient::Seeding) {
        qDebug() << " --------";
        qDebug() << " - torrent download finished, consider seeding (not possible with this client).";
        qDebug() << " -   uploaded bytes: " << torrentClient->uploadedBytes();
        qDebug() << " - downloaded bytes: " << torrentClient->downloadedBytes();
        emit finished(0);
    }
    qDebug() << "TorrentClient state changed to: " << torrentClient->stateString();
}

void CoreApplication::updatePeerInfo() {
    update();
}

void CoreApplication::updateProgress(int percent) {
    update();
}

void CoreApplication::updateDownloadRate(int rate) {
    static int previousrate = 0;
    if (previousrate == rate)
        return;
    previousrate = rate;
    m_downrate = rate;
    update();
}

void CoreApplication::updateUploadRate(int rate) {
    static int previousrate = 0;
    if (previousrate == rate)
        return;
    previousrate = rate;
    m_uprate = rate;
    update();
}

void CoreApplication::torrentStopped() {
    qDebug() << "not implemented (see the gui torrent client example), exiting";
    emit finished(0);
}

void CoreApplication::torrentError(TorrentClient::Error) {
    qDebug() << __PRETTY_FUNCTION__ << torrentClient->errorString();
}

void CoreApplication::update() {
    qDebug() << "  -> peers/seeds: " << torrentClient->connectedPeerCount()
             << "/" << torrentClient->seedCount()
             << ", download/upload rate kb/s: " << m_downrate/1000
             << "/" << m_uprate/1000
             << torrentClient->stateString() << torrentClient->progress() << "% ";
    saveState();
}

void CoreApplication::saveState() {
    QSettings settings("Trolltech", "Torrent");
    settings.setValue("resumeState", torrentClient->dumpedState());
    settings.setValue("torrentName", torrent);
    settings.setValue("dstDirectory", dstDirectory);
    settings.setValue("uploadedBytes", torrentClient->uploadedBytes());
    settings.setValue("downloadedBytes", torrentClient->downloadedBytes());

    settings.sync();
}

void CoreApplication::finished(int state) {
    QCoreApplication::exit(state);
}
