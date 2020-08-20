/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     LiLinling <lilinling_cm@deepin.com>
 *
 * Maintainer: LiLinling <lilinling_cm@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "defaultappsmodule.h"
#include "defappwidget.h"
#include "defappdetailwidget.h"
#include "modules/defapp/defappmodel.h"
#include "modules/defapp/defappworker.h"

#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::defapp;

DefaultAppsModule::DefaultAppsModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frame)
{

}

DefaultAppsModule::~DefaultAppsModule()
{
    if (m_defAppModel)
        m_defAppModel->deleteLater();
    m_defAppModel = nullptr;

    if (m_defAppWorker)
        m_defAppWorker->deleteLater();
    m_defAppWorker = nullptr;
}

void DefaultAppsModule::initialize()
{
    if (m_defAppModel) {
        delete m_defAppModel;
    }
    m_defAppModel  = new dcc::defapp::DefAppModel;
    m_defAppWorker = new dcc::defapp::DefAppWorker(m_defAppModel);
}

void DefaultAppsModule::active()
{
    DefaultAppsWidget* defaultappsWidget = new DefaultAppsWidget;

    connect(this, &DefaultAppsModule::requestSetDefappCategory, defaultappsWidget, &DefaultAppsWidget::setCurrentIndex);
    connect(defaultappsWidget, &DefaultAppsWidget::requestCategoryClicked, this, &DefaultAppsModule::showDetailWidget);
    m_frameProxy->pushWidget(this, defaultappsWidget);
    //显示默认页
    showDetailWidget(dcc::defapp::DefAppWorker::Browser);
    m_defAppWorker->onGetListApps();
}

const QString DefaultAppsModule::name() const
{
    return QStringLiteral("defapp");
}

const QString DefaultAppsModule::displayName() const
{
    return tr("Default Applications");
}

void DefaultAppsModule::contentPopped(QWidget *const w)
{
    Q_UNUSED(w);
}

int DefaultAppsModule::load(const QString &path)
{
    QMap<QString, dcc::defapp::DefAppWorker::DefaultAppsCategory> maps = {
        { QStringLiteral("Webpage"), dcc::defapp::DefAppWorker::Browser},
        { QStringLiteral("Mail"), dcc::defapp::DefAppWorker::Mail},
        { QStringLiteral("Text"), dcc::defapp::DefAppWorker::Text},
        { QStringLiteral("Music"), dcc::defapp::DefAppWorker::Music},
        { QStringLiteral("Video"), dcc::defapp::DefAppWorker::Video},
        { QStringLiteral("Picture"), dcc::defapp::DefAppWorker::Picture},
        { QStringLiteral("Terminal"), dcc::defapp::DefAppWorker::Terminal},
    };
    QString loadPath = path.split("/").at(0);
    dcc::defapp::DefAppWorker::DefaultAppsCategory currentCategory = maps.value(loadPath, dcc::defapp::DefAppWorker::Browser);

    showDetailWidget(currentCategory);
    Q_EMIT requestSetDefappCategory(currentCategory);
    return 0;
}

QStringList DefaultAppsModule::availPage() const
{
    QStringList sl;
    sl << "Webpage" << "Mail" << "Text" << "Music" << "Video" << "Picture" << "Terminal";

    return sl;
}

void DefaultAppsModule::showDetailWidget(dcc::defapp::DefAppWorker::DefaultAppsCategory category) {
    DefappDetailWidget* detailWidget = new DefappDetailWidget(category);

    detailWidget->setModel(m_defAppModel);
    connect(detailWidget, &DefappDetailWidget::requestSetDefaultApp, m_defAppWorker, &dcc::defapp::DefAppWorker::onSetDefaultApp); //设置默认程序
    connect(detailWidget, &DefappDetailWidget::requestDelUserApp, m_defAppWorker, &dcc::defapp::DefAppWorker::onDelUserApp);
    connect(detailWidget, &DefappDetailWidget::requestCreateFile, m_defAppWorker, &dcc::defapp::DefAppWorker::onCreateFile);
    m_frameProxy->pushWidget(this, detailWidget);
}
