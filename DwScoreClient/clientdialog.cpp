#include <QtGui/QMessageBox>
#include <QtGui/QLineEdit>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QDesktopWidget>
#include <QtGui/QApplication>
#include <QtNetwork/QHostAddress>
#include <QtXml/QDomDocument>
#include <QtXml/QDomNodeList>
#include "clientdialog.h"
#include "doubledelegate.h"

ClientDialog::ClientDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi();
}

void ClientDialog::setupUi()
{
    setWindowTitle(tr("DevWing���ϵͳ"));

    table = new QTableWidget();
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::CurrentChanged);
    table->setMouseTracking(true);
    connect(table, SIGNAL(currentCellChanged(int,int,int,int)), this, SLOT(showModifyButton(int,int,int,int)));
    QFont font;
    font.setPixelSize(12);
    table->setFont(font);

    QVBoxLayout* tableLayout = new QVBoxLayout;
    tableLayout->addWidget(table);

    setLayout(tableLayout);

    this->resize(800, 650);
    QDesktopWidget* desktop = QApplication::desktop();
    move((desktop->width() - this->width())/2, (desktop->height() - this->height())/2);

    setWindowFlags(Qt::Dialog | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
}

void ClientDialog::submit()
{
    int row = btnSubmit->property("row").toInt();
    int col = table->columnCount()-1;
    QStringList values;
    for(int i=2; i<col; i++)
    {
        QTableWidgetItem* item = table->item(row, i);
        if(item != 0)
        {
            values<<item->text();
        }
    }
    if(QMessageBox::Ok == QMessageBox::question(this, tr("�ύ"),
                                                tr("����ķ���Ϊ%1��ȷ��Ҫ�ύ��").arg(values.join(tr("��"))),
                                                QMessageBox::Ok,
                                                QMessageBox::Cancel))
    {
        QString str = tr("<submit id='%1' player='%2'>").arg(idClient).arg(table->item(row, 0)->text());
        for(int i=0; i<values.size(); i++)
        {
            QTableWidgetItem* item = table->horizontalHeaderItem(2+i);
            str.append(tr("<huanjie order='%1'>%2</huanjie>").arg(item->whatsThis()).arg(values.at(i)));
        }
        str.append("</submit>");
        socket->write(str.toAscii());
        table->removeCellWidget(row, col);
        QAbstractItemModel* model = table->model();
        model->setData(model->index(row, col), tr("���ύ��Submitted��"));
        setItemEditable(row, col, false);
        for(int i=2; i<col; i++)
        {
            this->setItemEditable(row, i, false);
        }
    }
    else
    {
        table->editItem(table->item(row, 2));
    }
}

void ClientDialog::showModifyButton(int row, int, int prevRow, int)
{
    if(row == prevRow)
    {
        return;
    }
    int col = table->columnCount() - 1;
    QTableWidgetItem* item = table->item(row, col);
    QWidget* widgetPrev = table->cellWidget(prevRow, col);
    if(widgetPrev == 0 || ((QPushButton*)widgetPrev)->text() != tr("�ύ��Submit��"))
    {
        table->removeCellWidget(prevRow, col);
        if(widgetPrev != 0)
        {
            for(int i=2; i<table->columnCount()-1; i++)
            {
                this->setItemEditable(prevRow, i, false);
            }
        }
    }
    if(item != 0)
    {
        btnModify = new QPushButton(tr("�޸ģ�Modify��"));
        btnModify->setProperty("row", row);
        connect(btnModify, SIGNAL(clicked()), this, SLOT(modify()));
        table->setCellWidget(row, col, btnModify);
        btnModify->setFocus();
    }
}

void ClientDialog::modify()
{
    int row = btnModify->property("row").toInt();
    for(int i=2; i<table->columnCount()-1; i++)
    {
        this->setItemEditable(row, i, true);
    }
    table->editItem(table->item(row, 2));
    btnModify->setText(tr("�ύ���ģ�Alteration��"));
    table->resizeColumnsToContents();
    btnModify->disconnect(this);
    connect(btnModify, SIGNAL(clicked()), this, SLOT(submitModify()));
}

void ClientDialog::submitModify()
{
    int row = btnModify->property("row").toInt();
    int col = table->columnCount()-1;
    QStringList values;
    for(int i=2; i<col; i++)
    {
        QTableWidgetItem* item = table->item(row, i);
        if(item != 0)
        {
            values<<item->text();
        }
    }
    if(QMessageBox::Ok == QMessageBox::question(this, tr("�ύ�޸�"),
                                                tr("�޸ĺ�ķ���Ϊ%1��ȷ��Ҫ�ύ��").arg(values.join(tr("��"))),
                                                QMessageBox::Ok,
                                                QMessageBox::Cancel))
    {
        QString str = tr("<submitModify id='%1' player='%2'>").arg(idClient).arg(table->item(row, 0)->text());
        for(int i=0; i<values.size(); i++)
        {
            QTableWidgetItem* item = table->horizontalHeaderItem(2+i);
            str.append(tr("<huanjie order='%1'>%2</huanjie>").arg(item->whatsThis()).arg(values.at(i)));
        }
        str.append("</submitModify>");
        socket->write(str.toAscii());
        table->removeCellWidget(row, col);
        QAbstractItemModel* model = table->model();
        model->setData(model->index(row, col), tr("���ύ��Submitted��"));
        setItemEditable(row, col, false);
        for(int i=2; i<col; i++)
        {
            this->setItemEditable(row, i, false);
        }
    }
    else
    {
        table->editItem(table->item(row, 2));
    }
}

void ClientDialog::readInfo()
{
    QString str(socket->readAll());
    QDomDocument dom;
    dom.setContent(str);
    QDomElement node = dom.documentElement();
    QString name =  node.nodeName();
    if(name == "initInfo")
    {
        initData(node);
    }
    else if(name == "mark")
    {
        int order = node.attribute("order").toInt();
        QString player = node.attribute("name");
        mark(order, player);
    }
}

void ClientDialog::mark(int order, QString player)
{
    for(int i=table->rowCount()-1; i>=0; i--)
    {
        if(table->item(i, 0)->text().toInt() == order)
        {
            return;
        }
    }
    insertRow(order, player);
    table->editItem(table->item(table->rowCount()-1, 2));
    QAbstractItemModel* model = table->model();
    for(int i=2; i<table->columnCount()-1; i++)
    {
        model->setData(model->index(table->rowCount()-1, i), 0.00);
    }
}

void ClientDialog::initData(QDomElement node)
{
    QStringList header;
    header<<tr("ѡ�֣�Player��")<<tr("������Name��");
    QDomNodeList huanjies = node.elementsByTagName("huanjie");
    for(int i=0; i<huanjies.size(); i++)
    {
        header<<huanjies.at(i).childNodes().at(0).nodeValue();
    }
    header.append(tr("������Actions��"));
    table->setColumnCount(header.size());
    table->setHorizontalHeaderLabels(header);
    table->resizeColumnsToContents();
    QTableWidgetItem* item;
    for(int i=0; i<huanjies.size(); i++)
    {
        if(huanjies.at(i).nodeType() == QDomNode::ElementNode)
        {
            QDomElement element = huanjies.at(i).toElement();
            DoubleDelegate* delegate = new DoubleDelegate(element.attribute("min").toDouble(),
                                                          element.attribute("max").toDouble());
            table->setItemDelegateForColumn(2+i, delegate);
            item = table->horizontalHeaderItem(2+i);
            item->setWhatsThis(element.attribute("order"));
        }
    }
    QString title(node.firstChildElement("systemName").text());
    title.append(tr("���ϵͳ ��ίID��%1 ��%2�� ��%3��")
                 .arg(idClient)
                 .arg(node.firstChildElement("currentLunci").text())
                 .arg(node.firstChildElement("currentChangci").text()));
    this->setWindowTitle(title);

    QDomNodeList scores = node.firstChildElement("scoreData").childNodes();
    int order;
    for(int i=0; i<scores.size(); i++)
    {
        if(scores.at(i).nodeType() == QDomNode::ElementNode)
        {
            QDomElement element = scores.at(i).toElement();
            order = element.attribute("order").toInt();
            QString name = element.attribute("name");
            QString value = element.text();
            insertRow(order, name, value);
        }
    }

    QDomElement domMark = node.firstChildElement("mark");
    if(!domMark.isNull())
    {
        int order = domMark.attribute("order").toInt();
        QString player = domMark.attribute("name");
        mark(order, player);
    }
}

void ClientDialog::insertRow(int order, QString name, QString value)
{
    int row = table->rowCount();
    int col = table->columnCount() - 1;
    table->insertRow(row);
    QAbstractItemModel* model = table->model();
    model->setData(model->index(row, 0), order);
    model->setData(model->index(row, 1), name);
    QStringList list = value.split('|');
    for(int i=0; i<list.size(); i++)
    {
        model->setData(model->index(row, 2 + i), list.at(i));
    }
    if(value == "")
    {
        btnSubmit = new QPushButton(tr("�ύ��Submit��"));
        connect(btnSubmit, SIGNAL(clicked()), this, SLOT(submit()));
        table->setCellWidget(row, col, btnSubmit);
        btnSubmit->setProperty("row", row);
        setItemEditable(row, 0, false);
        setItemEditable(row, 1, false);
    }
    else
    {
        model->setData(model->index(row, col), tr("���ύ��Submitted��"));
        setRowEditable(row, false);
    }
    table->resizeColumnsToContents();
}

void ClientDialog::setRowEditable(int row, bool on)
{
    for(int i=0; i < table->columnCount(); i++)
    {
        setItemEditable(row, i, on);
    }
}

void ClientDialog::setItemEditable(int row, int col, bool on)
{
    QTableWidgetItem* item = table->item(row, col);
    if(on)
    {
        item->setFlags(item->flags() | Qt::ItemIsEditable);
    }
    else
    {
        item->setFlags(item->flags() & (~Qt::ItemIsEditable));
    }
}

void ClientDialog::setSocket(QTcpSocket *pSocket)
{
    this->socket = pSocket;
    connect(socket, SIGNAL(readyRead()), this, SLOT(readInfo()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(error(QAbstractSocket::SocketError)));

    socket->write(tr("<getInitInfo id='%1'/>").arg(idClient).toAscii());
}

void ClientDialog::error(QAbstractSocket::SocketError err)
{
    QString strErr = tr("���ӷ�����ʧ�ܣ�������룺%1��").arg(err);
    if(QAbstractSocket::HostNotFoundError == err)
    {
        QMessageBox::critical(this,tr("����"),strErr.append(tr("�ڵ�ǰ������δ�ҵ�����������ȷ���������Ѿ����������ڼ���״̬��")));
    }
    else if(QAbstractSocket::ConnectionRefusedError == err)
    {
        QMessageBox::critical(this,tr("����"),strErr.append(tr("���ӱ��ܾ���������δ��Ӧ�����ʳ�ʱ��")));
    }
    else if(QAbstractSocket::RemoteHostClosedError == err)
    {
        QMessageBox::critical(this,tr("����"),strErr.append(tr("�������ѹرգ�")));
    }
    else
    {
        QMessageBox::critical(this,tr("����"),strErr);
    }
}

void ClientDialog::keyPressEvent(QKeyEvent *)
{

}
