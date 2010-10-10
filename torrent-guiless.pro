DEFINES += QT_NO_CAST_FROM_BYTEARRAY
DEFINES += QT_NO_CAST_TO_ASCII

HEADERS += coreapplication.h \
           bencodeparser.h \
           connectionmanager.h \
           metainfo.h \
           peerwireclient.h \
           ratecontroller.h \
           filemanager.h \  
           torrentclient.h \
           torrentserver.h \
           trackerclient.h

SOURCES += main-nogui.cpp \
           coreapplication.cpp \
           bencodeparser.cpp \
           connectionmanager.cpp \
           metainfo.cpp \
           peerwireclient.cpp \
           ratecontroller.cpp \
           filemanager.cpp \
           torrentclient.cpp \
           torrentserver.cpp \
           trackerclient.cpp


RESOURCES += icons.qrc

QT += network