#ifndef CTKEVENTBUSPLUGIN_H
#define CTKEVENTBUSPLUGIN_H

#include <ctkPluginActivator.h>
class ctkEventBusImpl;

class ctkEventBusPlugin : public QObject,
                   public ctkPluginActivator
                   
{
  Q_OBJECT
  Q_INTERFACES(ctkPluginActivator)

public:
  ctkEventBusPlugin(){};
  ~ctkEventBusPlugin(){};

  void start(ctkPluginContext* context);
  void stop(ctkPluginContext* context);

  static ctkEventBusPlugin* getInstance();

  ctkPluginContext* getPluginContext() const;

private:

  static ctkEventBusPlugin* instance;
  ctkPluginContext* context;
  ctkEventBusImpl *m_Bus;
};


#endif // CTKEVENTBUSPLUGIN_H
