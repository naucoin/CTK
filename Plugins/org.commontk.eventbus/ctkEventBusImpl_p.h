#ifndef CTKEVENTBUSIMPL_H
#define CTKEVENTBUSIMPL_H

#include <service/event/ctkEventBus.h>

#include "mafEventBus/mafEventBusManager.h"

#include <QList>
#include <QHash>
#include <QSet>

#define mafList QList
#define mafVariant QVariant

//class forward
class ctkEventHandlerWrapper;


class ctkEventBusImpl : public QObject,
                     public ctkEventBus
{
  Q_OBJECT
  Q_INTERFACES(ctkEventBus)

public:

  //static ctkEventBusImpl* instance();
          ctkEventBusImpl();

  void postEvent(const ctkEvent& event);
  void sendEvent(const ctkEvent& event);

  void publishSignal(const QObject* publisher, const char* signal, const QString& topic, Qt::ConnectionType type = Qt::QueuedConnection);

  QString subscribeSlot(const QObject* subscriber, const char* member, const QString& topic,const ctkDictionary& properties);

  void updateProperties(const QString& subscriptionId, const ctkDictionary& properties);
  
  /// Create the server for remote communication according to the given protocol and listen port.
  bool createServer(const QString &communication_protocol, unsigned int listen_port);

    /// Allow to start server listening.
  void startListen();

    /// Create the client for remote communication according to the given protocol, server host and port.
  bool createClient(const QString &communication_protocol, const QString &server_host, unsigned int port);

protected:

  typedef QList<ctkEventHandlerWrapper*> HandlerList;

  HandlerList globalWildcard;

  QHash<QString, HandlerList> topicName;

  void dispatchEvent(const ctkEvent& event, bool isAsync);

  void bucket(ctkEventHandlerWrapper* wrapper);

  QSet<ctkEventHandlerWrapper*> handlers(const QString& topic);

private:


  mafEventBus::mafEventBusManager *m_MafEventBusManager;
};

#endif // CTKEVENTBUSIMPL_H
