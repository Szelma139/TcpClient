#include "widget.h"
#include "ui_widget.h"
#include <QDebug>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    m_socket(new QTcpSocket(this))
{
    ui->setupUi(this);
    connect(m_socket, &QTcpSocket::readyRead, this, &Widget::readyRead);
    connect(m_socket, &QTcpSocket::connected,
            this, &Widget::connectToServer);
    connect(m_socket, &QTcpSocket::disconnected, this, &Widget::disconnectFromServer);
    ui->spinBox->setValue(52693);
}

Widget::~Widget()
{
    delete ui;
}


void Widget::connectToServer()
{
    updateGui(QAbstractSocket::ConnectedState); //ustaw state ConnectedState na true
}

void Widget::disconnectFromServer()
{
    updateGui(QAbstractSocket::UnconnectedState); //ustaw state UnconnectedState na true
}
void Widget::on_connect_clicked()
{

    const QString user = ui->userText->text().simplified(); //wywal podwojne spacje
    if(user.isEmpty()) //jesli nie podano usera nie polaczy sie
    {
            ui->chat->appendPlainText(tr("== Unable to connect to server"));
            return;

    }
    m_user = user; //pobiera usera z pola user


    if(m_socket->state() !=QAbstractSocket::ConnectedState)
    {
//jesli connected state false to polacz bo nie jestesmy polaczenie
        //quint16 port = 52693;
        m_socket->connectToHost(ui->addressText->text(),ui->spinBox->value()); //podlacz sie na
        //adres i port
       updateGui(QAbstractSocket::ConnectingState);
    }
}


void Widget::updateGui(QAbstractSocket::SocketState state)
{
    const bool connected = (state == QAbstractSocket::ConnectedState); //true gdy polaczono TRUE
    const bool unconnected = (state == QAbstractSocket::UnconnectedState); //true gdy rozlaczono FALSe
    ui->connect->setEnabled(unconnected); // FALSE
    ui->addressText->setEnabled(unconnected); //FALSE
    ui->userText->setEnabled(unconnected); //jesli niepolaczony to aktywuj intefejs FALSE
    ui->spinBox->setEnabled(unconnected); //FALSE

    ui->disconnect->setEnabled(!unconnected); //enable connect button
    ui->chat->setEnabled(connected); //jesli polaczony to aktywuj chat
    ui->text->setEnabled(connected);

}

void Widget::readyRead()
{
    if (m_socket->state() !=QAbstractSocket::ConnectedState) //jesli socket nie jest polaczony powrot
    {
       return;
    }

    m_receivedData.append(m_socket->readAll()); //zczytaj info z socketu do qbytearray
    while(true)
    { //sprawdz czy ma znak 23 (koniec pakietu )
        int endIndex = m_receivedData.indexOf(23);
        if(endIndex < 0)
        {
            qDebug() << "Konic pakietu";
            break;
        }
        QString message = QString::fromUtf8(m_receivedData.left(endIndex));
               m_receivedData.remove(0, endIndex + 1); // usun dwie cyfry z konca z qbytearraya
               QString t = QString::fromStdString((m_receivedData.toStdString()));
               qDebug() << "ReceivedData " << t;
               newMessage(message); //czytaj wiadomosc bez znaku 23
               qDebug() << "newMessage(message)";
    }
    qDebug() << " Wywoluje Readyread";
}


void Widget::newMessage(const QString &message)
{
    ui->chat->appendPlainText(message);
    //dopisz message to pola chatu
    qDebug() << "New Message";
}



void Widget::on_disconnect_clicked()
{
if (m_socket->state() !=QAbstractSocket::ConnectingState)
{    //anuluj polaczenie
    ui->chat->appendPlainText(tr("==Abort Connecting."));
}
m_socket->abort(); //usuwa polaczenie socketa bez czekania na zakonczenie
updateGui(QAbstractSocket::UnconnectedState);
}

void Widget::on_text_returnPressed()
{
    if(m_socket->state() !=QAbstractSocket::ConnectedState) return;
//jesli nie polaczono to powrot
    QString message = QStringLiteral("%1: %2") //Radek:tekst wiadomosci
            .arg(m_user).arg(ui->text->text());

    QByteArray messageArray = message.toUtf8(); //wrzuc stringa do messageArray
    messageArray.append(23);  //dopisz znak 23
    m_socket->write(messageArray); //wyslij wiadomosc i katywuj readyRead
   // emit readyRead();
    ui->text->clear(); //wyczysc pole
    qDebug() << "Probuje wyslac wiadomosc";
   // qDebug() << "Data written " << m_socket->write(messageArray.data());
    //qDebug() << "Bytes available " << m_socket->bytesAvailable();
}
