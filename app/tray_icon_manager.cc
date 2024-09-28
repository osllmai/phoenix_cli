#include "tray_icon_manager.h"

#include <QApplication>
#include <QDebug>

TrayIconManager::TrayIconManager(QObject *parent) : QObject(parent) {
    // Initialize tray icon
    trayIcon = new QSystemTrayIcon(this);
    QIcon icon(":/icon.png"); // Ensure this path is correct
    if (icon.isNull()) {
        qDebug() << "Failed to load tray icon!";
    }
    trayIcon->setIcon(icon);
    trayIcon->setVisible(true);
    trayIcon->setToolTip("Phoenix Application");

    // Initialize tray menu
    trayMenu = new QMenu();

    // Exit action
    exitAction = new QAction("Exit", trayMenu);
    connect(exitAction, &QAction::triggered, this, &TrayIconManager::onExit);
    trayMenu->addAction(exitAction);

    // Set context menu
    trayIcon->setContextMenu(trayMenu);

    // Optional: Connect activation signal (e.g., double-click)
    connect(trayIcon, &QSystemTrayIcon::activated, this, [&](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::Trigger) { // Single click
            // Implement if needed
        } else if (reason == QSystemTrayIcon::DoubleClick) { // Double click
            // Implement if needed
        }
    });
}

TrayIconManager::~TrayIconManager() {
    delete trayMenu;
    // trayIcon is deleted automatically as its parent is `this`
}

void TrayIconManager::onExit() {
    QApplication::quit();
}
