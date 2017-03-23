#include "bluetoothdelegate.h"
#include "bluetoothlistmodel.h"
#include "bluetooth/device.h"

#include <QPainter>
#include <QDateTime>
#include <QDebug>

BluetoothDelegate::BluetoothDelegate(QObject *parent) : QAbstractItemDelegate(parent)
{

}

void BluetoothDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    painter->setPen(Qt::white);
    painter->setBrush(Qt::red);

    const bool isSetting = index.data(BluetoothListModel::ItemIsSettingRole).toBool();
    const bool isHeader = index.data(BluetoothListModel::ItemIsHeaderRole).toBool();
    const bool isHovered = index.data(BluetoothListModel::ItemHoveredRole).toBool();
    const bool isNext = index.data(BluetoothListModel::ItemNextRole).toBool();
    const bool isPowerOff = index.data(BluetoothListModel::ItemIsPowerOffRole).toBool();

    if (isHovered && !isHeader)
        painter->fillRect(option.rect, QColor(255, 255, 255, 0.1 * 255));

    if (isHeader)
        painter->fillRect(option.rect, QColor(255, 255, 255, 0.15 * 255));

    if (index.row())
    {
        if (!isNext && !isHovered) {
            painter->setPen(QColor(255, 255, 255, 255 * 0.05));
            painter->drawLine(QPoint(10, option.rect.top()), QPoint(option.rect.right() - 10, option.rect.top()));
        }
    }
    else
    {
        painter->setPen(QColor(255, 255, 255, 255 * 0.1));
        painter->drawLine(QPoint(0, option.rect.top()), QPoint(option.rect.right(), option.rect.top()));
    }

    QFont f(painter->font());
    f.setWeight(isHeader ? 440 : 300);
    painter->setFont(f);

    painter->setPen(Qt::white);

    if (isPowerOff)
        painter->drawText(option.rect, Qt::AlignCenter, index.data(Qt::DisplayRole).toString());
    else
        painter->drawText(option.rect.marginsRemoved(QMargins(24, 0, 0, 0)), Qt::AlignVCenter | Qt::AlignLeft, index.data(Qt::DisplayRole).toString());

    const int x = option.rect.right() - 24;
    const int y = option.rect.top() + (option.rect.height() - 16) / 2;

    if (isHeader && !isSetting) {
        const bool isRefresh = index.data(BluetoothListModel::ItemRefreshRole).toBool();

        if (!isRefresh) {
            QPen pen(QColor("#01bdff"));
            painter->setPen(pen);

            QFont font(painter->font());
            font.setWeight(400);
            font.setPixelSize(13);
            painter->setFont(font);

            painter->drawText(QRect(option.rect.left(), option.rect.top(), option.rect.width() - 10, option.rect.height()), Qt::AlignVCenter | Qt::AlignRight, tr("Refresh"));

            painter->setFont(f);
        } else {
            const quint64 index = QDateTime::currentMSecsSinceEpoch() / 20;
            const QString pix = QString(":/frame/themes/dark/icons/dark_loading/loading_0%1.png").arg((index % 90), 2, 10, QChar('0'));
            painter->drawPixmap(x, y, QPixmap(pix));
        }
    }

    if (!isHeader && !isSetting)
    {
        const bool connecting = index.data(BluetoothListModel::ItemConnectingRole).toBool();

        // connecting , connected, no connect
        if (connecting) {
            const quint64 index = QDateTime::currentMSecsSinceEpoch() / 20;
            const QString pix = QString(":/frame/themes/dark/icons/dark_loading/loading_0%1.png").arg((index % 90), 2, 10, QChar('0'));
            painter->drawPixmap(x, y, QPixmap(pix));
        } else {
            const bool state = index.data(BluetoothListModel::ItemConnectedRole).toBool();
            if (state) {
                // draw connected icon
                if (isHovered)
                    painter->drawPixmap(x, y, QPixmap(":/frame/themes/dark/icons/disconnect.png"));
                else
                    painter->drawPixmap(x, y, QPixmap(":/frame/themes/dark/icons/select.png"));
            }
        }
    }
}

QSize BluetoothDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);

    return index.data(Qt::SizeHintRole).toSize();
}
