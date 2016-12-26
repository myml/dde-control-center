/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "createpage.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>

#include "translucentframe.h"

using namespace dcc::widgets;

namespace dcc {
namespace accounts {

CreatePage::CreatePage(QWidget *parent) :
    ContentWidget(parent),
    m_avatar(new AvatarWidget),
    m_group(new SettingsGroup),
    m_username(new LineEditWidget),
    m_password(new LineEditWidget),
    m_repeatpass(new LineEditWidget),
    m_confirmBtn(new QPushButton(tr("Create"))),
    m_cancelBtn(new QPushButton(tr("Cancel"))),
    m_errorTip(new ErrorTip)
{
    m_avatar->setFixedSize(90, 90);

    m_username->setTitle(tr("Username"));
    m_password->setTitle(tr("Password"));
    m_password->textEdit()->setEchoMode(QLineEdit::Password);
    m_repeatpass->setTitle(tr("Repeat password"));
    m_repeatpass->textEdit()->setEchoMode(QLineEdit::Password);

    m_errorTip->setWindowFlags(Qt::ToolTip);
    m_errorTip->hide();

    m_group->appendItem(m_username);
    m_group->appendItem(m_password);
    m_group->appendItem(m_repeatpass);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->setContentsMargins(0, 20, 0, 20);
    buttonLayout->addWidget(m_cancelBtn);
    buttonLayout->addWidget(m_confirmBtn);

    TranslucentFrame *container = new TranslucentFrame;
    setContent(container);

    QVBoxLayout *layout = new QVBoxLayout(container);
    layout->addWidget(m_avatar, 0, Qt::AlignHCenter);
    layout->addSpacing(10);
    layout->addWidget(m_group);
    layout->addLayout(buttonLayout);

    connect(m_confirmBtn, &QPushButton::clicked, this, &CreatePage::createUser);
    connect(m_cancelBtn, &QPushButton::clicked, this, &CreatePage::cancelCreation);

    connect(m_username->textEdit(), &QLineEdit::textChanged, [this] { m_errorTip->hide(); });
    connect(m_repeatpass->textEdit(), &QLineEdit::textChanged, [this] { m_errorTip->hide(); });
    connect(m_password->textEdit(), &QLineEdit::textChanged, [this] { m_errorTip->hide(); });
}

void CreatePage::setModel(User *user)
{
    m_user = user;

    connect(user, &User::currentAvatarChanged, m_avatar, &AvatarWidget::setAvatarPath);
}

void CreatePage::setCreationResult(CreationResult *result)
{
    switch (result->type()) {
    case CreationResult::NoError:
        emit back();
        break;
    case CreationResult::UserNameError:
        showUsernameErrorTip(result->message());
        break;
    case CreationResult::PasswordMatchError:
        showPasswordMatchErrorTip(result->message());
        break;
    case CreationResult::PasswordError:
        break; // reserved for future server edition feature.
    case CreationResult::UnknownError:
        qWarning() << "error encountered creating user: " << result->message();
        emit back();
        break;
    default:
        break;
    }

    result->deleteLater();
}

void CreatePage::createUser() const
{
    m_user->setName(m_username->text());
    m_user->setPassword(m_password->text());
    m_user->setRepeatPassword(m_repeatpass->text());

    emit requestCreateUser(m_user);
}

void CreatePage::cancelCreation() const
{
    m_errorTip->hide();

    emit back();
}

void CreatePage::showUsernameErrorTip(QString error)
{
    QPoint globalStart = m_username->mapToGlobal(QPoint(0, 0));
    m_errorTip->setText(error);
    m_errorTip->show(globalStart.x() + m_username->width() / 2,
                     globalStart.y() + m_username->height() / 2 + 10);
}

void CreatePage::showPasswordMatchErrorTip(QString error)
{
    QPoint globalStart = m_repeatpass->mapToGlobal(QPoint(0, 0));
    m_errorTip->setText(error);
    m_errorTip->show(globalStart.x() + m_repeatpass->width() / 2,
                     globalStart.y() + m_repeatpass->height() / 2 + 10);
}

ErrorTip::ErrorTip() :
    DArrowRectangle(DArrowRectangle::ArrowTop),
    m_label(new QLabel)
{
    m_label->setStyleSheet("padding: 5px 10px; color: #f9704f");
    setContent(m_label);
}

void ErrorTip::setText(QString text)
{
    m_label->setText(text);
    m_label->adjustSize();
    resizeWithContent();
}

} // namespace accounts
} // namespace dcc
