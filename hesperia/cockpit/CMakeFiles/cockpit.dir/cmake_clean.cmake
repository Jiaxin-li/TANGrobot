FILE(REMOVE_RECURSE
  "cockpit_de_DE.qm"
  "cockpit_en_US.qm"
  "cockpit_de_DE.ts"
  "cockpit_en_US.ts"
  "include/moc_ContainerObserver.cxx"
  "include/moc_FIFOMultiplexer.cxx"
  "include/moc_CockpitWindow.cxx"
  "include/moc_MdiPlugIn.cxx"
  "include/moc_QtIncludes.cxx"
  "include/moc_Cockpit.cxx"
  "include/plugins/modulestatisticsviewer/moc_LoadPerModule.cxx"
  "include/plugins/modulestatisticsviewer/moc_ModuleStatisticsViewerPlugIn.cxx"
  "include/plugins/modulestatisticsviewer/moc_LoadPlot.cxx"
  "include/plugins/modulestatisticsviewer/moc_ModuleStatisticsViewerWidget.cxx"
  "include/plugins/objxviewer/moc_OBJXViewerPlugIn.cxx"
  "include/plugins/objxviewer/moc_OBJXViewerWidget.cxx"
  "include/plugins/objxviewer/moc_OBJXGLWidget.cxx"
  "include/plugins/iruscharts/moc_IrUsChartData.cxx"
  "include/plugins/iruscharts/moc_IrUsChartsWidget.cxx"
  "include/plugins/iruscharts/moc_IrUsChartsPlugIn.cxx"
  "include/plugins/moc_PlugInProvider.cxx"
  "include/plugins/moc_AbstractGLWidget.cxx"
  "include/plugins/scnxviewer/moc_SCNXViewerWidget.cxx"
  "include/plugins/scnxviewer/moc_SCNXGLWidget.cxx"
  "include/plugins/scnxviewer/moc_SCNXViewerPlugIn.cxx"
  "include/plugins/player/moc_PlayerPlugIn.cxx"
  "include/plugins/player/moc_PlayerWidget.cxx"
  "include/plugins/spy/moc_SpyWidget.cxx"
  "include/plugins/spy/moc_SpyPlugIn.cxx"
  "include/plugins/birdseyemap/moc_SelectableNodeDescriptorTreeListener.cxx"
  "include/plugins/birdseyemap/moc_BirdsEyeMapPlugIn.cxx"
  "include/plugins/birdseyemap/moc_CameraAssignableNodesListener.cxx"
  "include/plugins/birdseyemap/moc_BirdsEyeMapWidget.cxx"
  "include/plugins/birdseyemap/moc_SelectableNodeDescriptor.cxx"
  "include/plugins/birdseyemap/moc_BirdsEyeMapRenderer.cxx"
  "include/plugins/birdseyemap/moc_BirdsEyeMapMapWidget.cxx"
  "include/plugins/birdseyemap/moc_TreeNodeVisitor.cxx"
  "include/plugins/forcecontrolviewer/moc_ForceControlViewerWidget.cxx"
  "include/plugins/forcecontrolviewer/moc_ForceControlData.cxx"
  "include/plugins/forcecontrolviewer/moc_ForceControlViewerPlugIn.cxx"
  "include/plugins/controller/moc_ControllerPlugIn.cxx"
  "include/plugins/controller/moc_ControllerWidget.cxx"
  "include/plugins/cutter/moc_CutterWidget.cxx"
  "include/plugins/cutter/moc_CutterPlugIn.cxx"
  "include/plugins/environmentviewer/moc_SelectableNodeDescriptorTreeListener.cxx"
  "include/plugins/environmentviewer/moc_EnvironmentViewerPlugIn.cxx"
  "include/plugins/environmentviewer/moc_CameraAssignableNodesListener.cxx"
  "include/plugins/environmentviewer/moc_SelectableNodeDescriptor.cxx"
  "include/plugins/environmentviewer/moc_EnvironmentViewerWidget.cxx"
  "include/plugins/environmentviewer/moc_EnvironmentViewerGLWidget.cxx"
  "include/plugins/environmentviewer/moc_TreeNodeVisitor.cxx"
  "include/plugins/moc_GLControlFrame.cxx"
  "include/plugins/configurationviewer/moc_ConfigurationViewerWidget.cxx"
  "include/plugins/configurationviewer/moc_ConfigurationViewerPlugIn.cxx"
  "include/plugins/irusmap/moc_IrUsMapWidgetControl.cxx"
  "include/plugins/irusmap/moc_PointSensor.cxx"
  "include/plugins/irusmap/moc_IrUsMapPlugIn.cxx"
  "include/plugins/irusmap/moc_IrUsMapWidget.cxx"
  "include/plugins/sharedimageviewer/moc_SharedImageViewerWidget.cxx"
  "include/plugins/sharedimageviewer/moc_SharedImageViewerPlugIn.cxx"
  "include/plugins/moc_ControlPlugIn.cxx"
  "include/plugins/moc_PlugIn.cxx"
  "CMakeFiles/cockpit.dir/apps/MainModule.cpp.o"
  "cockpit.pdb"
  "cockpit"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang CXX)
  INCLUDE(CMakeFiles/cockpit.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)