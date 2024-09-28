#include <QApplication>
#include <QSystemTrayIcon>
#include <iostream>
#include <QThread>

#include "commands_list.h"
#include "database_manager.h"
#include "directory_manager.h"
#include "tray_icon_manager.h"

class CommandWorker : public QObject {
Q_OBJECT

public:
    CommandWorker(int argc, char **argv) : argc(argc), argv(argv) {}

public slots:
    void process() {
        show_commands(argc, argv);
        emit finished();
    }

signals:
    void finished();

private:
    int argc;
    char **argv;
};


int main(int argc, char **argv) {
    QApplication app(argc, argv);

    // Ensure the application does not quit when the last window is closed
    QApplication::setQuitOnLastWindowClosed(false);

    // Database management
    sqlite3 *db;
    const std::string db_path = DirectoryManager::get_app_home_path() + "/phoenix.db";
    if (sqlite3_open(db_path.c_str(), &db) == SQLITE_OK) {
        DatabaseManager::create_tables(db);
        sqlite3_close(db);
    } else {
        std::cerr << "Can't open database" << std::endl;
    }

    // Check if system tray is available
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        std::cerr << "System tray is not available!" << std::endl;
        return 1;
    }

    // Initialize TrayIconManager
    TrayIconManager trayManager;

    CommandWorker *worker = new CommandWorker(argc, argv);
    QThread *thread = new QThread();

    worker->moveToThread(thread);
    QObject::connect(thread, &QThread::started, worker, &CommandWorker::process);
    QObject::connect(worker, &CommandWorker::finished, thread, &QThread::quit);
    QObject::connect(worker, &CommandWorker::finished, worker, &CommandWorker::deleteLater);
    QObject::connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    thread->start();

    return app.exec();
}

// **Include the MOC-generated file**
#include "main.moc"
