#include "application.h"

#include <QSettings>

int main(int argc, char *argv[])
{
    QSettings::setDefaultFormat(QSettings::IniFormat);

    QCoreApplication::setApplicationName(APP_NAME);
    QCoreApplication::setApplicationVersion(APP_VERSION);
    QCoreApplication::setOrganizationName(QStringLiteral("Flipper Devices Inc"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("flipperdevices.com"));
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    Application app(argc, argv);
    return app.exec();
}
