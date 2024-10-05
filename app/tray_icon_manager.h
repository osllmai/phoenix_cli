#ifndef TRAY_ICON_MANAGER_H
#define TRAY_ICON_MANAGER_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>

class TrayIconManager : public QObject {
Q_OBJECT

public:
    TrayIconManager(QObject *parent = nullptr);
    void showMessage(const QString &title, const QString &message);

private:
    QSystemTrayIcon trayIcon;
    QMenu *trayMenu;
    QAction *exitAction;

    void setupTrayIcon();
};

#endif // TRAY_ICON_MANAGER_H
