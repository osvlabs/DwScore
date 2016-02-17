#include <QtGui/QApplication>
#include <QtGui/QMessageBox>
#include <QTextCodec>
#include "logindialog.h"
#include "dialog.h"

QSqlDatabase db;

bool openDatabase()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QCoreApplication::applicationDirPath().append("/score.db"));
    if(!db.open())
    {
        QMessageBox::critical(NULL,QObject::tr("���ش���"),QObject::tr("�޷��򿪻��ߴ������ݿ⣡"),QMessageBox::Close);
        return false;
    }
    else
    {
        QSqlQuery query = db.exec("select count(name) from sqlite_master");
        if(query.next()){
            int tableCount = query.value(0).toInt();
            if(tableCount < 7){
                QMessageBox::warning(NULL,QObject::tr("���ش���"),
                                     QObject::tr("���ݿ��ļ�����ȷ!"),
                                     QMessageBox::Ok);
                return false;
            }
        }else{
            QMessageBox::critical(NULL,QObject::tr("���ش���"),
                                  QObject::tr("��ѯ���ݿ�ʧ�ܣ����ݿ��ļ��𻵻��߲��ܷ��ʣ�"),
                                  QMessageBox::Close);
            return false;
        }
    }
    return true;
}

int getConfigInt(QString name)
{
    QSqlQuery query = db.exec(QObject::tr("select [value] from [config] where [name]='").append(name).append("'"));
    if(query.next())
    {
        return query.value(0).toInt();
    }
    else
    {
        qDebug()<<"error occurred when select int value from config:"<<name<<endl;
        return -1;
    }
}

QString getConfigStr(QString name)
{
    QSqlQuery query = db.exec(QObject::tr("select [value] from [config] where [name]='").append(name).append("'"));
    if(query.next())
    {
        return query.value(0).toString();
    }
    else
    {
        qDebug()<<"error occurred when select string value from config:"<<name<<endl;
        return QString("");
    }
}

bool setConfig(QString name, QString value)
{
    QSqlQuery query(db);
    query.prepare("update [config] set [value]=:value where [name]=:name");
    query.bindValue(":value", value);
    query.bindValue(":name", name);
    return query.exec();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("GB18030"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB18030"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB18030"));

    if(!openDatabase())
    {
        return -1;
    }

    LoginDialog* login = new LoginDialog;
    if(login->exec() == QDialog::Accepted)
    {
        Dialog *w = new Dialog;
        w->setDatabase(&db);
        w->listen(login->getPort());
        w->show();
        delete login;
    }
    else
    {
        return 0;
    }

    return a.exec();
}
