#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <QtCore/QString>
namespace QUpdate{
static const QLatin1String RePositories("Repositories");
static const QLatin1String Packages("Packages");
static const QLatin1String Updates("Updates");
static const QLatin1String rePositoriesXml("/repositories.xml");
static const QLatin1String componentsXml("/components.xml");
static const QLatin1String UpdatesXmlPathName("/tmp/Updates.xml");
static const QLatin1String UpdatesXmlName("Updates.xml");
static const QLatin1String changeLogPathName("/tmp/changelog.txt");
static const QLatin1String maintenancetoolName("/maintenancetool");
static const QLatin1String qssPathName(":/update.css");
static const QLatin1String changeLogName("changelog.txt");
static const QLatin1String URL("Url");
static const QLatin1String NAME("Name");
static const QLatin1String VERSION("Version");
static const QLatin1String Package("Package");
static const QLatin1String PackageUpdate("PackageUpdate");
static const QLatin1String AppVersion("ApplicationVersion");
static const QLatin1String NoTipFlag("NoTipFlag");
static const QLatin1String NullString("");
}

#endif // CONSTANTS_H
