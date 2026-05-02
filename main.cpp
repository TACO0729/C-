#include "MBT_tester.h"
#include <QApplication>
#include <QFont>
#include <QFontDatabase>

static QString chooseAppFontFamily()
{
    const QStringList families = QFontDatabase::families();

    if (families.contains("YouYuan")) return "YouYuan";
    if (families.contains("幼圆")) return "幼圆";
    if (families.contains("Microsoft YaHei UI")) return "Microsoft YaHei UI";
    if (families.contains("Microsoft YaHei")) return "Microsoft YaHei";
    if (families.contains("微软雅黑")) return "微软雅黑";
    if (families.contains("SimHei")) return "SimHei";

    return QApplication::font().family();
}

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    QFont appFont(chooseAppFontFamily());
    appFont.setPointSize(11);
    QApplication::setFont(appFont);

    MBT_tester w;
    w.show();

    return a.exec();
}