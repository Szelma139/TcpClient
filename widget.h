#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpSocket>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    void updateGui(QAbstractSocket::SocketState state);
    void connectToServer();

    void disconnectFromServer();

public slots:
    void on_connect_clicked();
    void readyRead();
    void newMessage(const QString &message);
    void on_disconnect_clicked();

    void on_text_returnPressed();

private:
    Ui::Widget *ui;
    QTcpSocket * m_socket;
    QString m_user;
    QByteArray m_receivedData;
};

#endif // WIDGET_H
