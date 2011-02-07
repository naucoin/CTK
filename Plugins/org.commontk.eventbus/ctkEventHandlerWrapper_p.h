#ifndef CTKEVENTHANDLERWRAPPER_P_H
#define CTKEVENTHANDLERWRAPPER_P_H

#include <QStringList>

#include <service/event/ctkEventAdmin.h>
#include <service/event/ctkEventConstants.h>
#include <ctkLDAPSearchFilter.h>

#include <iostream>

class ctkEventHandlerWrapper : public QObject {

  Q_OBJECT

private:

  ctkProperties properties;
  QStringList topicList;
  ctkLDAPSearchFilter filter;

public:

  ctkEventHandlerWrapper(const QObject* subscriber, const char* handler, const ctkProperties& properties)
    : properties(properties)
  {
    connect(this, SIGNAL(notifySubscriber(ctkEvent)), subscriber, handler);
  }

  QStringList topics() const
  {
    return topicList;
  }

  bool init()
  {
    topicList.clear();

    // Get topic names
    QVariant v = properties[ctkEventConstants::EVENT_TOPIC];
    topicList = v.toStringList();

    if (topicList.empty())
    {
      return false;
    }

    v = properties[ctkEventConstants::EVENT_FILTER];
    filter = ctkLDAPSearchFilter(v.toString());
    return true;
  }

  void handleEvent(const ctkEvent& event /*, const Permission& perm */)
  {
    if (!event.matches(filter)) return;

    // should do permissions checks now somehow
    // ...

    try {
      emit notifySubscriber(event);
    }
    catch (const std::exception& e)
    {
      // TODO logging
      std::cerr << "Exception occured during publishing " << qPrintable(event.getTopic()) << ": " << e.what() << std::endl;
    }

  }

signals:

  void notifySubscriber(const ctkEvent&);

};

#endif // CTKEVENTHANDLERWRAPPER_P_H
