#include <QApplication>
#include <QSystemTrayIcon>
#include <QThread>
#include <QTimer>
#include <QLocalServer>
#include <QLocalSocket>
#include <QDataStream>
#include <iostream>
#include "commands_list.h"
#include "database_manager.h"
#include "directory_manager.h"
#include "tray_icon_manager.h"
#include <QtDBus/QtDBus>

void forceLinkingQtDBus() {
    // Use any class or function from QtDBus to ensure it's linked
    QDBusConnection connection = QDBusConnection::sessionBus();
}

class CommandWorker : public QObject {
Q_OBJECT

public:
    CommandWorker(int argc, char **argv) : argc(argc), argv(argv) {}

public slots:
    void process() {
        if (argc < 2) {
            std::cerr << "No command specified. Use 'phoenix_cli help' for a list of commands." << std::endl;
            emit finished();
            return;
        }

        PhoenixCommandsList commands;

        std::string command = argv[1];
        std::vector<std::string> args;

        int argc_condition = command == "server" || command == "embedding" ? 0 : 2;
        for (int i = argc_condition; i < argc; ++i) {
            args.push_back(argv[i]);
        }

        commands.run_command(command, args);

        emit finished();  // Signal when the command is done
    }

signals:
    void finished();

private:
    int argc;
    char **argv;
};

void runCommandInThread(int argc, char **argv, QApplication &app) {
    QThread *thread = new QThread();
    CommandWorker *worker = new CommandWorker(argc, argv);

    worker->moveToThread(thread);

    // When the thread starts, begin processing the command
    QObject::connect(thread, &QThread::started, worker, &CommandWorker::process);

    // Cleanup after the command finishes
    QObject::connect(worker, &CommandWorker::finished, thread, &QThread::quit);
    QObject::connect(worker, &CommandWorker::finished, worker, &CommandWorker::deleteLater);
    QObject::connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    // Quit the application after the command finishes
    QObject::connect(worker, &CommandWorker::finished, &app, &QApplication::quit);

    // Start the thread
    thread->start();
}

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    // Ensure the application does not quit when the last window is closed
    QApplication::setQuitOnLastWindowClosed(false);

    // Initialize Single Instance Server
    const QString SERVER_NAME = "phoenix_cli_unique_server_identifier";
    QLocalServer *server = new QLocalServer(&app);

    // Remove any existing socket with the same name
    QLocalServer::removeServer(SERVER_NAME);

    if (!server->listen(SERVER_NAME)) {
        // If unable to listen, assume another instance is running
        QLocalSocket socket;
        socket.connectToServer(SERVER_NAME);

        if (socket.waitForConnected(1000)) {
            // Send the command to the running instance
            QByteArray byteArray;
            QDataStream out(&byteArray, QIODevice::WriteOnly);
            for (int i = 1; i < argc; ++i) {  // Skip the program name
                out << QString::fromUtf8(argv[i]);
            }
            socket.write(byteArray);
            socket.flush();
            socket.waitForBytesWritten(1000);
            socket.disconnectFromServer();
            return 0;  // Exit the new instance
        } else {
            std::cerr << "Unable to connect to the running instance." << std::endl;
            return 1;
        }
    }

    // If server is successfully listening, set up the tray icon and handle incoming commands
    TrayIconManager trayManager;

    // Database management
    auto db = DatabaseManager::open_database();
    DatabaseManager::create_tables(db.get());

    // Check if system tray is available
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        std::cerr << "System tray is not available!" << std::endl;
        return 1;
    }

    // Handle incoming connections (commands)
    QObject::connect(server, &QLocalServer::newConnection, [&app, server]() {
        QLocalSocket *clientConnection = server->nextPendingConnection();
        QObject::connect(clientConnection, &QLocalSocket::readyRead, [clientConnection]() {
            QDataStream in(clientConnection);
            QString command;
            QList<QString> args;

            // Read all incoming arguments
            while (!in.atEnd()) {
                QString arg;
                in >> arg;
                args.append(arg);
            }

            // Convert QString list to argc/argv
            int new_argc = args.size() + 1;
            char **new_argv = new char*[new_argc];
            new_argv[0] = strdup("phoenix_cli");  // Program name
            for (int i = 0; i < args.size(); ++i) {
                new_argv[i + 1] = strdup(args[i].toUtf8().constData());
            }

            // Run the command in a separate thread
            runCommandInThread(new_argc, new_argv, *(QApplication*)QCoreApplication::instance());

            // Clean up allocated memory after processing
            QTimer::singleShot(0, [new_argv, new_argc]() {
                for (int i = 0; i < new_argc; ++i) {
                    free(new_argv[i]);
                }
                delete[] new_argv;
            });

            clientConnection->disconnectFromServer();
        });
    });

    // If no commands are passed, just show the tray icon and run the event loop
    if (argc <= 1) {
        int result = app.exec();
        return result;
    }

    // If commands are passed, process them in the main instance
    runCommandInThread(argc, argv, app);

    // Start the event loop
    return app.exec();
}

#include "main.moc"
