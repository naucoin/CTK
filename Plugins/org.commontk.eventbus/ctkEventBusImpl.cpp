#include "ctkEventBusImpl_p.h"

#include <QSetIterator>

#include "ctkEventHandlerWrapper_p.h"
#include "ctkBusEvent.h"

#define ctkEventArgument(type,data) QArgument<type >(#type, data)

ctkEventBusImpl::ctkEventBusImpl()
{
    m_EventBusManager = ctkEventBus::ctkEventBusManager::instance();
}

void ctkEventBusImpl::postEvent(const ::ctkEvent& event)
{
  dispatchEvent(event, true);
}

void ctkEventBusImpl::sendEvent(const ::ctkEvent& event)
{
  dispatchEvent(event, false);
}

void ctkEventBusImpl::publishSignal(const QObject* publisher, const char* signal, const QString& topic,
                                    Qt::ConnectionType type)
{
    ctkBusEvent *mesbEvent = new ctkBusEvent(topic, ctkEventBus::ctkEventTypeLocal, ctkEventBus::mafSignatureTypeSignal, const_cast<QObject *>(publisher), signal);
    m_EventBusManager->addEventProperty(*mesbEvent);
}

void ctkEventBusImpl::publishSignal(const QObject* publisher, const char* signal, Qt::ConnectionType type)
{
    qDebug() << "Not implemented in this class";
}

qlonglong ctkEventBusImpl::subscribeSlot(const QObject* subscriber, const char* member, const ctkDictionary& properties)
{
    QString topic = properties.value("EventTopic").toString();
    ctkBusEvent *mesbEvent = new ctkBusEvent(topic, ctkEventBus::ctkEventTypeLocal, ctkEventBus::mafSignatureTypeCallback, const_cast<QObject *>(subscriber), member);
    m_EventBusManager->addEventProperty(*mesbEvent);

    return topic.toLongLong();

  /*ctkEventHandlerWrapper* wrapper = new ctkEventHandlerWrapper(subscriber, member, properties);
  if (wrapper->init())
  {
    bucket(wrapper);
  }

  // TODO return id
  return QString();*/
}

void ctkEventBusImpl::unsubscribeSlot(const QString& topic) {
      // @@@@to be implemented
}

bool ctkEventBusImpl::updateProperties(const QString& topic, const ctkDictionary& properties)
{
  Q_UNUSED(topic)
  Q_UNUSED(properties)
  // @@@@to be implemented
}

void ctkEventBusImpl::dispatchEvent(const ctkEvent& event, bool isAsync)
{
  Q_UNUSED(isAsync)

  QString topic = event.getTopic(); //may contains widlcards

  ctkBusEvent *mebEvent = new ctkBusEvent(topic,ctkEventBus::ctkEventTypeLocal,ctkEventBus::mafSignatureTypeSignal, this, "no");
  //cycle for all other elements

  mebEvent->setEventTopic(topic);
  mebEvent->setEventType(ctkEventBus::ctkEventTypeLocal);
  //mebEvent->setEventFilter(NULL);
  //need to think about the arguments inside the event

  typedef QList<QGenericArgument> ctkEventArgumentList;

  ctkEventArgumentList list;
  list.append(Q_ARG(QVariantList,event.getProperty("localEvent").toList()));
  list.append(Q_ARG(QVariantList,event.getProperty("localData").toList()));

  m_EventBusManager->notifyEvent(topic, ctkEventBus::ctkEventTypeRemote, &list);
  m_EventBusManager->notifyEvent(*mebEvent);
}

void ctkEventBusImpl::bucket(ctkEventHandlerWrapper* wrapper)
{
  // TODO bucket logic
  globalWildcard.push_back(wrapper);
}

QSet<ctkEventHandlerWrapper*> ctkEventBusImpl::handlers(const QString& topic)
{
  Q_UNUSED(topic)

  // TODO
  return globalWildcard.toSet();
}

bool ctkEventBusImpl::createServer(const QString &communication_protocol, unsigned int listen_port) {
        return m_EventBusManager->createServer(communication_protocol,listen_port);
}

void ctkEventBusImpl::startListen() {
        m_EventBusManager->startListen();
}

bool ctkEventBusImpl::createClient(const QString &communication_protocol, const QString &server_host, unsigned int port) {
        return m_EventBusManager->createClient(communication_protocol,server_host,port);
}
