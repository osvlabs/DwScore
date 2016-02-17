#include <QApplication>
#include <QLineEdit>
#include <QFormLayout>
#include <QPushButton>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QHostAddress>
#include <QHostInfo>
#include <QLabel>
#include <QtXml/QDomElement>
#include "logindialog.h"

QString getConfigStr(QString);

LoginDialog::LoginDialog(QDialog *parent) :
    QDialog(parent, Qt::Widget | Qt::WindowMinimizeButtonHint)
{
    setupUi();
}

void LoginDialog::setupUi()
{
    editPort = new QLineEdit(tr("8888"));
    editPwd = new QLineEdit(tr("123456"));
    editPwd->setEchoMode(QLineEdit::Password);
    btnConnect = new QPushButton(tr("��¼"));
    connect(btnConnect, SIGNAL(clicked()), this, SLOT(login()));

    QFormLayout* layoutForm = new QFormLayout;
    layoutForm->addRow(tr("������IP��"), new QLabel(QHostInfo::fromName(QHostInfo::localHostName()).addresses().first().toString()));
    layoutForm->addRow(tr("�����˿ڣ�"), editPort);
    layoutForm->addRow(tr("�������룺"), editPwd);
    layoutForm->addWidget(btnConnect);

    this->setLayout(layoutForm);

    this->resize(260, 120);
    QDesktopWidget* desktop = QApplication::desktop();
    move((desktop->width() - this->width())/2, (desktop->height() - this->height())/2);

    this->setWindowTitle(tr("DevWing���ϵͳ��������"));
    this->setModal(true);
}

void LoginDialog::login()
{
    QString strPort = editPort->text().trimmed();
    if(strPort == "")
    {
        QMessageBox::critical(this, tr("����"), tr("������˿ںš�"));
        return;
    }
    QString strPwd = editPwd->text().trimmed();
    if(strPwd == "")
    {
        QMessageBox::critical(this, tr("����"), tr("�������¼���롣"));
        return;
    }
    this->setEnabled(false);
    if(strPwd.compare(getConfigStr(tr("adminPwd"))) == 0)
    {
        this->accept();
    }
    else
    {
        QMessageBox::information(this, tr("����"), tr("�������"));
        this->setEnabled(true);
        this->editPwd->setFocus();
        this->editPwd->selectAll();
    }
}

quint16 LoginDialog::getPort()
{
    return editPort->text().trimmed().toUShort();
}
