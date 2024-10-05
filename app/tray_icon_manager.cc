#include "tray_icon_manager.h"
#include <QIcon>
#include <QApplication>

TrayIconManager::TrayIconManager(QObject *parent) : QObject(parent), trayIcon(this) {
    setupTrayIcon();
}

void TrayIconManager::setupTrayIcon() {
    trayMenu = new QMenu();

    // Example actions
    QAction *showCommandsAction = new QAction("Show Commands", this);
    // Connect to appropriate slots or lambdas if needed
    trayMenu->addAction(showCommandsAction);

    exitAction = new QAction("Exit", this);
    connect(exitAction, &QAction::triggered, qApp, &QApplication::quit);
    trayMenu->addAction(exitAction);

    trayIcon.setContextMenu(trayMenu);
    trayIcon.setIcon(QIcon(":/icon.png")); // Ensure you have an icon resource
    trayIcon.setToolTip("Phoenix CLI");
    trayIcon.show();
}

void TrayIconManager::showMessage(const QString &title, const QString &message) {
    trayIcon.showMessage(title, message, QSystemTrayIcon::Information, 3000);
}
