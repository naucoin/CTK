#ifndef CTKEVENTBUSIMPL_H
#define CTKEVENTBUSIMPL_H

#include "ctkEventAdminBus.h"

#include "mafEventBusManager.h"

#include <QList>
#include <QHash>
#include <QSet>

//class forward
class ctkEventHandlerWrapper;


class ctkEventBusImpl : public QObject,
                     public ctkEventAdminBus
{
  Q_OBJECT
  Q_INTERFACES(ctkEventAdminBus)

public:

  //static ctkEventBusImpl* instance();
          ctkEventBusImpl();

  void postEvent(const ctkEvent& event);
  void sendEvent(const ctkEvent& event);

  void publishSignal(const QObject* publisher, const char* signal, const QString& topic, Qt::ConnectionType type = Qt::QueuedConnection);

  void publishSignal(const QObject* publisher, const char* signal, Qt::ConnectionType type = Qt::QueuedConnection);

  QString subscribeSlot(const QObject* subscriber, const char* member, const QString& topic,const ctkDictionary& properties);

  virtual void unsubscribeSlot(const QString& topic);

  virtual bool updateProperties(const QString& topic, const ctkDictionary& properties);
  
  /// Create the server for remote communication according to the given protocol and listen port.
  virtual bool createServer(const QString &communication_protocol, unsigned int listen_port);

    /// Allow to start server listening.
  virtual void startListen();

    /// Create the client for remote communication according to the given protocol, server host and port.
  virtual bool createClient(const QString &communication_protocol, const QString &server_host, unsigned int port);

protected:

  typedef QList<ctkEventHandlerWrapper*> HandlerList;

  HandlerList globalWildcard;

  QHash<QString, HandlerList> topicName;

  void dispatchEvent(const ctkEvent& event, bool isAsync);

  void bucket(ctkEventHandlerWrapper* wrapper);

  QSet<ctkEventHandlerWrapper*> handlers(const QString& topic);

private:


  ctkEventBus::ctkEventBusManager *m_EventBusManager;
};

#endif // CTKEVENTBUSIMPL_H
