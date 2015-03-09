/**
 * cockpit - Visualization environment
 * Copyright (C) 2012 - 2015 Christian Berger
 * Copyright (C) 2008 - 2011 (as monitor component) Christian Berger, Bernhard Rumpe
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef COCKPIT_PLUGINS_MODULESTATISTICSVIEWER_MODULESTATISTICSVIEWERWIDGET_H_
#define COCKPIT_PLUGINS_MODULESTATISTICSVIEWER_MODULESTATISTICSVIEWERWIDGET_H_

#include <deque>
#include <map>

#include "QtIncludes.h"

#include "core/SharedPointer.h"
#include "core/io/ContainerListener.h"
#include "core/data/dmcp/ModuleDescriptor.h"
#include "core/data/dmcp/ModuleDescriptorComparator.h"
#include "core/data/dmcp/ModuleStatistics.h"

#include "plugins/PlugIn.h"

#include "plugins/modulestatisticsviewer/LoadPerModule.h"
#include "plugins/modulestatisticsviewer/LoadPlot.h"

namespace cockpit {
    namespace plugins {
      namespace modulestatisticsviewer {

        using namespace std;

        /**
         * This class is the container for the shared image viewer widget.
         */
        class ModuleStatisticsViewerWidget : public QWidget,
            public core::io::ContainerListener
        {

        Q_OBJECT

        private:
          /**
           * "Forbidden" copy constructor. Goal: The compiler should warn
           * already at compile time for unwanted bugs caused by any misuse
           * of the copy constructor.
           */
          ModuleStatisticsViewerWidget(const ModuleStatisticsViewerWidget &/*obj*/);

          /**
           * "Forbidden" assignment operator. Goal: The compiler should warn
           * already at compile time for unwanted bugs caused by any misuse
           * of the assignment operator.
           */
          ModuleStatisticsViewerWidget&
          operator=(const ModuleStatisticsViewerWidget &/*obj*/);

        public:
          /**
           * Constructor.
           *
           * @param plugIn Reference to the plugin to which this widget belongs.
           * @param prnt Pointer to the parental widget.
           */
          ModuleStatisticsViewerWidget(const PlugIn &plugIn, QWidget *prnt);

          virtual
          ~ModuleStatisticsViewerWidget();

          virtual void
          nextContainer(core::data::Container &c);

        private:
          LoadPlot *m_plot;
          deque<core::data::dmcp::ModuleStatistics> m_moduleStatistics;
          map<core::data::dmcp::ModuleDescriptor, core::SharedPointer<
              LoadPerModule>, core::data::dmcp::ModuleDescriptorComparator>
              m_loadPerModule;
          uint32_t m_color;
        };
    }
  }
} // cockpit::plugins::modulestatisticsviewer

#endif /*COCKPIT_PLUGINS_MODULESTATISTICSVIEWER_MODULESTATISTICSVIEWERWIDGET_H_*/
