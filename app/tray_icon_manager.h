// TrayIconManager.h
#pragma once

#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QIcon>

class TrayIconManager : public QObject {
    Q_OBJECT

public:
    explicit TrayIconManager(QObject *parent = nullptr);
    ~TrayIconManager();

private slots:
            void onExit();

private:
    QSystemTrayIcon *trayIcon;
    QMenu *trayMenu;
    QAction *exitAction;
};
