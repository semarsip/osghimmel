
// Copyright (c) 2011, Daniel M�ller <dm@g4t3.de>
// Computer Graphics Systems Group at the Hasso-Plattner-Institute, Germany
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
//   * Redistributions of source code must retain the above copyright notice, 
//     this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright 
//     notice, this list of conditions and the following disclaimer in the 
//     documentation and/or other materials provided with the distribution.
//   * Neither the name of the Computer Graphics Systems Group at the 
//     Hasso-Plattner-Institute (HPI), Germany nor the names of its 
//     contributors may be used to endorse or promote products derived from 
//     this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGE.

#include "mainwindow.h"
#include "apps/skybox/ui_mainwindow.h"

#include "utils/log.h"
#include "mainlogdispatcher.h"

#include "collapsibledockwidget.h"
#include "glsleditor.h"
#include "logoutputlabel.h"
#include "logoutputwidget.h"
#include "qosgviewer.h"

#include "scenewidget.h"

#include "scenes/scene_spheremappedhimmel.h"
#include "scenes/scene_cubemappedhimmel.h"

#include "utils/import.h"


#include <QFileInfo>


#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/TerrainManipulator>
#include <osgGA/StateSetManipulator>

#include <osgUtil/Optimizer>

#include <osgDB/ReadFile>

#include <osgViewer/View>
#include <osgViewer/ViewerEventHandlers>

#include <assert.h>


MainWindow::MainWindow(QWidget *parent)
:   QMainWindow(parent)
,   m_ui(new Ui::MainWindow)
,   m_himmel(NULL)
,   m_scene(NULL)

,   m_glslEditor(NULL)
,   m_glslEditorDockWidget(NULL)
,   m_sceneWidget(NULL)
,   m_sceneDockWidget(NULL)
{
    QCoreApplication::setOrganizationName("dm@g4t3.de");
    QCoreApplication::setApplicationName(APPLICATION_NAME);

    m_ui->setupUi(this);

    setWindowTitle(APPLICATION_NAME " " APPLICATION_VERSION);

    initializeLog();

    const QSize size(m_ui->centralWidget->size());

    initializeScene();
    initializeManipulator(m_ui->centralWidget);

    initializeDockWidgets();

	connect(m_ui->centralWidget, SIGNAL(mouseDrop(QList<QUrl>)),
		this, SLOT(mouseDroped(QList<QUrl>)));
}


MainWindow::~MainWindow()
{
    uninitializeLog();

    delete m_glslEditor;
    delete m_glslEditorDockWidget;

    delete m_sceneWidget;
    delete m_sceneDockWidget;
}


void MainWindow::initializeLog()
{
    MainLogDispatcher *logDispatcher = dynamic_cast<MainLogDispatcher*>(_LOG->dispatcher());
    assert(logDispatcher);

    m_logWidget = new LogOutputWidget();
    m_logWidget->setObjectName("Log");
    m_logWidget->setWindowTitle("Log");
    m_logDockWidget = new CollapsibleDockWidget(*m_logWidget , this);
    this->addDockWidget(Qt::BottomDockWidgetArea, m_logDockWidget );

    logDispatcher->setOutputWidget(m_logWidget);

    m_logStatusLabel = new LogOutputLabel(m_logDockWidget);
    logDispatcher->setOutputLabel(m_logStatusLabel);

    m_ui->statusBar->addWidget(m_logStatusLabel, 8);
}


void MainWindow::uninitializeLog()
{
    MainLogDispatcher *logDispatcher = dynamic_cast<MainLogDispatcher*>(_LOG->dispatcher());
    assert(logDispatcher);

    logDispatcher->setOutputWidget();
    logDispatcher->setOutputLabel();

    delete m_logWidget;
    delete m_logDockWidget;

    delete m_logStatusLabel;
}


void MainWindow::initializeScene()
{
    assert(!m_scene);

    m_scene = new osg::Group();
}

void MainWindow::initializeManipulator(osgViewer::View *view)
{
    // set up the camera manipulators.
    osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keyswitchManipulator = new osgGA::KeySwitchMatrixManipulator;

    keyswitchManipulator->addMatrixManipulator('1', "Trackball", new osgGA::TrackballManipulator());
    keyswitchManipulator->addMatrixManipulator('2', "Flight",    new osgGA::FlightManipulator());
    keyswitchManipulator->addMatrixManipulator('3', "Drive",     new osgGA::DriveManipulator());
    keyswitchManipulator->addMatrixManipulator('4', "Terrain",   new osgGA::TerrainManipulator());

    m_ui->centralWidget->setCameraManipulator(keyswitchManipulator.get());

    m_ui->centralWidget->addEventHandler(new osgViewer::StatsHandler);
    m_ui->centralWidget->addEventHandler(new osgViewer::ThreadingHandler);
}


void MainWindow::initializeToolBars()
{
}


//QTextDocument *g_document;

void MainWindow::initializeDockWidgets()
{
    m_glslEditor = new GLSLEditor();
    m_glslEditorDockWidget = new CollapsibleDockWidget(*m_glslEditor, this);

    addDockWidget(Qt::RightDockWidgetArea, m_glslEditorDockWidget);


    m_sceneWidget = new SceneWidget();
    m_sceneDockWidget = new CollapsibleDockWidget(*m_sceneWidget, this);

    addDockWidget(Qt::RightDockWidgetArea, m_sceneDockWidget);


    //g_document = new QTextDocument();

    //m_glslEditor->setDocument(g_document, GLSL_FRAGMENT);
}


void MainWindow::changeEvent(QEvent *event)
{
    QMainWindow::changeEvent(event);

    switch(event->type())
    {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;

    default:
        break;
    }
}


void MainWindow::himmelChanged()
{
    m_ui->cubeMappedHimmelAction->setChecked(false);
    m_ui->sphereMappedHimmelAction->setChecked(false);
    m_ui->proceduralHimmelAction->setChecked(false);

    m_himmel->addChild(m_scene.get());
}


void MainWindow::clearHimmel()
{
    if(m_himmel)
        m_himmel->removeChild(m_scene.get());

    delete m_himmel;
    m_himmel = NULL;
}



void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
}


void MainWindow::on_quitAction_triggered(bool)
{
    close();
}


void MainWindow::on_aboutAction_triggered(bool)
{
}


void MainWindow::on_sphereMappedHimmelAction_triggered(bool)
{
    clearHimmel();

    m_himmel = new Scene_SphereMappedHimmel(
        m_ui->centralWidget, m_ui->centralWidget->size());

    himmelChanged();

    m_ui->sphereMappedHimmelAction->setChecked(true);
}


void MainWindow::on_cubeMappedHimmelAction_triggered(bool)
{
    clearHimmel();

    m_himmel = new Scene_CubeMappedHimmel(
        m_ui->centralWidget, m_ui->centralWidget->size());

    himmelChanged();

    m_ui->cubeMappedHimmelAction->setChecked(true);
}


void MainWindow::on_proceduralHimmelAction_triggered(bool)
{
    clearHimmel();

    himmelChanged();

    m_ui->proceduralHimmelAction->setChecked(true);
}


void MainWindow::mouseDroped(QList<QUrl> urlList)
{
	for(int i = 0; i < urlList.size(); ++i)
	{
		QFileInfo fileInfo(urlList.at(i).path().right(urlList.at(i).path().length() - 1));
		bool test = fileInfo.exists();

		QString source = fileInfo.absoluteFilePath();
		const std::string c_source = source.toLatin1();

		osg::ref_ptr<osg::Node> loadedScene = osgDB::readNodeFile(c_source);
		if(!loadedScene) 
            _LOG_WARNING("Load Drop Data failed", "Loading " + source + " has failed.");

		else
		{
			// optimize the scene graph, remove redundant nodes and state etc.
			osgUtil::Optimizer optimizer;
			optimizer.optimize(loadedScene.get());

			m_scene->removeChildren(0, m_scene->getNumChildren());
			m_scene->addChild(loadedScene.get());
		}
	}
}
