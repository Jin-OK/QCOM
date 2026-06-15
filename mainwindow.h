#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QTcpServer>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_btnScanSerial_clicked();
    void on_btnSend_clicked();
    void on_btnClearRecv_clicked();
    void on_btnClearSend_clicked();
    
    void on_rbSerial_toggled(bool checked);
    void on_rbTcp_toggled(bool checked);
    void on_rbUdp_toggled(bool checked);
    
    void on_btnConnect_clicked();
    
    void on_chkHexRecv_toggled(bool checked);
    void on_chkHexSend_toggled(bool checked);
    void on_chkTimestamp_toggled(bool checked);
    void on_chkAutoSend_toggled(bool checked);
    
    void on_btnFont_clicked();
    void on_btnSave_clicked();
    
    void onSerialDataReceived();
    void onSerialErrorOccurred(QSerialPort::SerialPortError error);
    
    void onTcpClientConnected();
    void onTcpClientDisconnected();
    void onTcpClientReadyRead();
    void onTcpClientErrorOccurred(QAbstractSocket::SocketError socketError);
    
    void onTcpServerNewConnection();
    void onTcpServerReadyRead();
    void onTcpClientDisconnectedFromServer();
    
    void onUdpReadyRead();
    
    void onAutoSend();
    void onFlushRecvBuffer();

private:
    Ui::MainWindow *ui;
    
    enum class TcpMode { Client, Server };
    
    QSerialPort *m_serialPort;
    QTcpSocket *m_tcpClient;
    QTcpServer *m_tcpServer;
    QTcpSocket *m_tcpServerClient;
    QUdpSocket *m_udpSocket;
    
    QTimer *m_autoSendTimer;
    QTimer *m_flushTimer;
    
    QByteArray m_recvBuffer;
    
    TcpMode m_tcpMode;
    bool m_isSerialOpen;
    bool m_isTcpClientConnected;
    bool m_isTcpServerListening;
    bool m_isUdpBound;
    
    void initUI();
    void initConnections();
    void loadSettings();
    void saveSettings();
    
    void scanSerialPorts();
    void openSerialPort();
    void closeSerialPort();
    
    void connectTcp();
    void disconnectTcp();
    
    void bindUdp();
    void unbindUdp();
    
    void sendData(const QByteArray &data);
    void appendReceivedData(const QByteArray &data);
    
    QByteArray hexStringToByteArray(const QString &hexString);
    QString byteArrayToHexString(const QByteArray &byteArray);
    
    QString formatDataWithTimestamp(const QString &data, bool isSend);
};
#endif // MAINWINDOW_H