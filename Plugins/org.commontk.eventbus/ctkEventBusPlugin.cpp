/*
 * ctkEventBusPlugin.cxx
 *
 *  Created on: Mar 29, 2010
 *      Author: zelzer
 */

#include "ctkEventBusPlugin_p.h"

#include <QtPlugin>

#include "ctkEventBusImpl_p.h"

#include <iostream>

ctkEventBusPlugin* ctkEventBusPlugin::instance = 0;

void ctkEventBusPlugin::start(ctkPluginContext* context)
{
  instance = this;
  this->context = context;
  qDebug() << "ctkEventBus Plugin starting";
  m_Bus = new ctkEventBusImpl;
  qDebug() << "ctkEventBus created";
  context->registerService(QStringList("ctkEventAdmin"),m_Bus);
  qDebug() << "ctkEventBus Plugin started";
}

void ctkEventBusPlugin::stop(ctkPluginContext* context)
{
  Q_UNUSED(context)
  std::cout << "org.commontk.eventbus Plugin stopped\n";
  delete m_Bus;
}

ctkEventBusPlugin* ctkEventBusPlugin::getInstance()
{
  return instance;
}

ctkPluginContext* ctkEventBusPlugin::getPluginContext() const
{
  return context;
}

Q_EXPORT_PLUGIN2(org_commontk_eventbus, ctkEventBusPlugin)
