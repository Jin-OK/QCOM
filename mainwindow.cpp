#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSerialPortInfo>
#include <QMessageBox>
#include <QSettings>
#include <QFileDialog>
#include <QDateTime>
#include <QTimer>
#include <QFontDialog>
#include <QFile>
#include <QTextStream>
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_serialPort(new QSerialPort(this))
    , m_tcpClient(new QTcpSocket(this))
    , m_tcpServer(new QTcpServer(this))
    , m_tcpServerClient(nullptr)
    , m_udpSocket(new QUdpSocket(this))
    , m_autoSendTimer(new QTimer(this))
    , m_tcpMode(TcpMode::Client)
    , m_isSerialOpen(false)
    , m_isTcpClientConnected(false)
    , m_isTcpServerListening(false)
    , m_isUdpBound(false)
{
    ui->setupUi(this);
    initUI();
    initConnections();
    loadSettings();
    scanSerialPorts();
}

MainWindow::~MainWindow()
{
    saveSettings();
    if (m_isSerialOpen) closeSerialPort();
    if (m_isTcpClientConnected) disconnectTcp();
    if (m_isTcpServerListening) disconnectTcp();
    if (m_isUdpBound) unbindUdp();
    delete ui;
}

void MainWindow::initUI()
{
    setWindowTitle(tr("QCOM - Serial & Network Assistant"));
    
    QString styleSheet = QString(
        "QGroupBox {"
        "   font-weight: bold;"
        "   border: 1px solid #c0c0c0;"
        "   border-radius: 4px;"
        "   margin-top: 10px;"
        "   padding: 8px;"
        "}"
        "QGroupBox::title {"
        "   subcontrol-origin: margin;"
        "   left: 8px;"
        "   padding: 0 3px;"
        "}"
        "QPushButton {"
        "   padding: 6px 12px;"
        "}"
    );
    this->setStyleSheet(styleSheet);
    
    ui->rbSerial->setIcon(QIcon(":/icons/serial.svg"));
    ui->rbTcp->setIcon(QIcon(":/icons/tcp_client.svg"));
    ui->rbUdp->setIcon(QIcon(":/icons/udp.svg"));
    
    ui->btnScanSerial->setIcon(QIcon(":/icons/scan.svg"));
    ui->btnConnect->setIcon(QIcon(":/icons/connect.svg"));
    ui->btnSend->setIcon(QIcon(":/icons/send.svg"));
    ui->btnClearRecv->setIcon(QIcon(":/icons/clear.svg"));
    ui->btnClearSend->setIcon(QIcon(":/icons/clear.svg"));
    ui->btnFont->setIcon(QIcon(":/icons/font.svg"));
    ui->btnSave->setIcon(QIcon(":/icons/save.svg"));
    
    QStringList baudRates = {"1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200", "230400", "460800", "921600"};
    ui->cbBaudRate->addItems(baudRates);
    ui->cbBaudRate->setCurrentText("115200");
    
    QStringList dataBits = {"5", "6", "7", "8"};
    ui->cbDataBits->addItems(dataBits);
    ui->cbDataBits->setCurrentText("8");
    
    QStringList stopBits = {"1", "1.5", "2"};
    ui->cbStopBits->addItems(stopBits);
    ui->cbStopBits->setCurrentText("1");
    
    QStringList parity = {tr("None"), tr("Even"), tr("Odd"), tr("Mark"), tr("Space")};
    ui->cbParity->addItems(parity);
    
    QStringList flowControl = {tr("None"), tr("Hardware"), tr("Software")};
    ui->cbFlowControl->addItems(flowControl);
    
    ui->rbSerial->setChecked(true);
    
    QFont monoFont("Consolas", 10);
    monoFont.setStyleHint(QFont::Monospace);
    ui->teRecv->setFont(monoFont);
    ui->teSend->setFont(monoFont);
    
    connect(m_autoSendTimer, &QTimer::timeout, this, &MainWindow::onAutoSend);
    
    connect(ui->cbTcpMode, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        m_tcpMode = (index == 0) ? TcpMode::Client : TcpMode::Server;
        if (m_tcpMode == TcpMode::Client) {
            ui->label_7->setText(tr("Server IP:"));
        } else {
            ui->label_7->setText(tr("Listen Port:"));
            ui->leTcpIP->setEnabled(false);
        }
    });
}

void MainWindow::initConnections()
{
    connect(m_serialPort, &QSerialPort::readyRead, this, &MainWindow::onSerialDataReceived);
    connect(m_serialPort, &QSerialPort::errorOccurred, this, &MainWindow::onSerialErrorOccurred);
    
    connect(m_tcpClient, &QTcpSocket::connected, this, &MainWindow::onTcpClientConnected);
    connect(m_tcpClient, &QTcpSocket::disconnected, this, &MainWindow::onTcpClientDisconnected);
    connect(m_tcpClient, &QTcpSocket::readyRead, this, &MainWindow::onTcpClientReadyRead);
    connect(m_tcpClient, &QTcpSocket::errorOccurred, this, &MainWindow::onTcpClientErrorOccurred);
    
    connect(m_tcpServer, &QTcpServer::newConnection, this, &MainWindow::onTcpServerNewConnection);
    
    connect(m_udpSocket, &QUdpSocket::readyRead, this, &MainWindow::onUdpReadyRead);
}

void MainWindow::loadSettings()
{
    QSettings settings("QCOM", "QCOM");
    
    restoreGeometry(settings.value("geometry").toByteArray());
    
    QString fontFamily = settings.value("font/family", "Consolas").toString();
    int fontSize = settings.value("font/size", 10).toInt();
    QFont font(fontFamily, fontSize);
    ui->teRecv->setFont(font);
    ui->teSend->setFont(font);
    
    ui->cbBaudRate->setCurrentText(settings.value("serial/baudRate", "115200").toString());
    ui->cbDataBits->setCurrentText(settings.value("serial/dataBits", "8").toString());
    ui->cbStopBits->setCurrentText(settings.value("serial/stopBits", "1").toString());
    ui->cbParity->setCurrentIndex(settings.value("serial/parity", 0).toInt());
    ui->cbFlowControl->setCurrentIndex(settings.value("serial/flowControl", 0).toInt());
    
    ui->cbTcpMode->setCurrentIndex(settings.value("tcp/mode", 0).toInt());
    ui->leTcpIP->setText(settings.value("tcp/ip", "127.0.0.1").toString());
    ui->leTcpPort->setText(settings.value("tcp/port", "8080").toString());
    
    ui->leUdpLocalIP->setText(settings.value("udp/localIP", "0.0.0.0").toString());
    ui->leUdpLocalPort->setText(settings.value("udp/localPort", "8080").toString());
    ui->leUdpRemoteIP->setText(settings.value("udp/remoteIP", "127.0.0.1").toString());
    ui->leUdpRemotePort->setText(settings.value("udp/remotePort", "8081").toString());
    
    ui->chkTimestamp->setChecked(settings.value("display/timestamp", true).toBool());
    ui->chkHexRecv->setChecked(settings.value("display/hexRecv", false).toBool());
    ui->chkHexSend->setChecked(settings.value("display/hexSend", false).toBool());
    ui->chkAppendNewline->setChecked(settings.value("display/appendNewline", true).toBool());
}

void MainWindow::saveSettings()
{
    QSettings settings("QCOM", "QCOM");
    
    settings.setValue("geometry", saveGeometry());
    
    settings.setValue("font/family", ui->teRecv->font().family());
    settings.setValue("font/size", ui->teRecv->font().pointSize());
    
    settings.setValue("serial/baudRate", ui->cbBaudRate->currentText());
    settings.setValue("serial/dataBits", ui->cbDataBits->currentText());
    settings.setValue("serial/stopBits", ui->cbStopBits->currentText());
    settings.setValue("serial/parity", ui->cbParity->currentIndex());
    settings.setValue("serial/flowControl", ui->cbFlowControl->currentIndex());
    
    settings.setValue("tcp/mode", ui->cbTcpMode->currentIndex());
    settings.setValue("tcp/ip", ui->leTcpIP->text());
    settings.setValue("tcp/port", ui->leTcpPort->text());
    
    settings.setValue("udp/localIP", ui->leUdpLocalIP->text());
    settings.setValue("udp/localPort", ui->leUdpLocalPort->text());
    settings.setValue("udp/remoteIP", ui->leUdpRemoteIP->text());
    settings.setValue("udp/remotePort", ui->leUdpRemotePort->text());
    
    settings.setValue("display/timestamp", ui->chkTimestamp->isChecked());
    settings.setValue("display/hexRecv", ui->chkHexRecv->isChecked());
    settings.setValue("display/hexSend", ui->chkHexSend->isChecked());
    settings.setValue("display/appendNewline", ui->chkAppendNewline->isChecked());
}

void MainWindow::on_btnScanSerial_clicked()
{
    scanSerialPorts();
}

void MainWindow::scanSerialPorts()
{
    ui->cbSerialPort->clear();
    const auto ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &port : ports) {
        ui->cbSerialPort->addItem(port.portName());
    }
    if (ui->cbSerialPort->count() == 0) {
        ui->cbSerialPort->addItem(tr("No Port"));
    }
}

void MainWindow::openSerialPort()
{
    QString portName = ui->cbSerialPort->currentText();
    if (portName.isEmpty() || portName == tr("No Port")) {
        QMessageBox::warning(this, tr("Error"), tr("Please select a serial port"));
        return;
    }
    
    m_serialPort->setPortName(portName);
    m_serialPort->setBaudRate(ui->cbBaudRate->currentText().toInt());
    m_serialPort->setDataBits(static_cast<QSerialPort::DataBits>(ui->cbDataBits->currentText().toInt()));
    
    int stopBitsIndex = ui->cbStopBits->currentIndex();
    QSerialPort::StopBits stopBits = QSerialPort::OneStop;
    if (stopBitsIndex == 1) stopBits = QSerialPort::OneAndHalfStop;
    else if (stopBitsIndex == 2) stopBits = QSerialPort::TwoStop;
    m_serialPort->setStopBits(stopBits);
    
    int parityIndex = ui->cbParity->currentIndex();
    QSerialPort::Parity parity = QSerialPort::NoParity;
    if (parityIndex == 1) parity = QSerialPort::EvenParity;
    else if (parityIndex == 2) parity = QSerialPort::OddParity;
    else if (parityIndex == 3) parity = QSerialPort::MarkParity;
    else if (parityIndex == 4) parity = QSerialPort::SpaceParity;
    m_serialPort->setParity(parity);
    
    int flowControlIndex = ui->cbFlowControl->currentIndex();
    QSerialPort::FlowControl flowControl = QSerialPort::NoFlowControl;
    if (flowControlIndex == 1) flowControl = QSerialPort::HardwareControl;
    else if (flowControlIndex == 2) flowControl = QSerialPort::SoftwareControl;
    m_serialPort->setFlowControl(flowControl);
    
    if (m_serialPort->open(QIODevice::ReadWrite)) {
        m_isSerialOpen = true;
        ui->btnConnect->setText(tr("Close"));
        ui->btnConnect->setIcon(QIcon(":/icons/disconnect.svg"));
        ui->cbSerialPort->setEnabled(false);
        ui->cbBaudRate->setEnabled(false);
        ui->cbDataBits->setEnabled(false);
        ui->cbStopBits->setEnabled(false);
        ui->cbParity->setEnabled(false);
        ui->cbFlowControl->setEnabled(false);
        ui->btnScanSerial->setEnabled(false);
        statusBar()->showMessage(tr("Serial port opened: %1").arg(portName));
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Failed to open serial port: %1\n%2")
                             .arg(portName, m_serialPort->errorString()));
    }
}

void MainWindow::closeSerialPort()
{
    m_serialPort->close();
    m_isSerialOpen = false;
    ui->btnConnect->setText(tr("Open"));
    ui->btnConnect->setIcon(QIcon(":/icons/connect.svg"));
    ui->cbSerialPort->setEnabled(true);
    ui->cbBaudRate->setEnabled(true);
    ui->cbDataBits->setEnabled(true);
    ui->cbStopBits->setEnabled(true);
    ui->cbParity->setEnabled(true);
    ui->cbFlowControl->setEnabled(true);
    ui->btnScanSerial->setEnabled(true);
    statusBar()->showMessage(tr("Serial port closed"));
}

void MainWindow::on_btnSend_clicked()
{
    QString text = ui->teSend->toPlainText();
    if (text.isEmpty()) return;
    
    QByteArray data;
    if (ui->chkHexSend->isChecked()) {
        data = hexStringToByteArray(text);
    } else {
        data = text.toUtf8();
        if (ui->chkAppendNewline->isChecked()) {
            data.append("\r\n");
        }
    }
    
    sendData(data);
}

void MainWindow::sendData(const QByteArray &data)
{
    bool sent = false;
    
    if (m_isSerialOpen) {
        sent = (m_serialPort->write(data) == data.size());
    } else if (m_isTcpClientConnected) {
        sent = (m_tcpClient->write(data) == data.size());
    } else if (m_isTcpServerListening && m_tcpServerClient) {
        sent = (m_tcpServerClient->write(data) == data.size());
    } else if (m_isUdpBound) {
        QString remoteIP = ui->leUdpRemoteIP->text();
        quint16 remotePort = ui->leUdpRemotePort->text().toUShort();
        sent = (m_udpSocket->writeDatagram(data, QHostAddress(remoteIP), remotePort) == data.size());
    }
    
    if (sent) {
        QString displayData = ui->chkHexSend->isChecked() ? byteArrayToHexString(data) : QString::fromUtf8(data);
        QString formatted = formatDataWithTimestamp(displayData, true);
        ui->teRecv->append(formatted);
    } else {
        statusBar()->showMessage(tr("Failed to send data"));
    }
}

void MainWindow::on_btnClearRecv_clicked()
{
    ui->teRecv->clear();
}

void MainWindow::on_btnClearSend_clicked()
{
    ui->teSend->clear();
}

void MainWindow::on_rbSerial_toggled(bool checked)
{
    if (checked) {
        ui->stackedWidget->setCurrentIndex(0);
    }
}

void MainWindow::on_rbTcp_toggled(bool checked)
{
    if (checked) {
        ui->stackedWidget->setCurrentIndex(1);
    }
}

void MainWindow::on_rbUdp_toggled(bool checked)
{
    if (checked) {
        ui->stackedWidget->setCurrentIndex(2);
    }
}

void MainWindow::on_btnConnect_clicked()
{
    if (ui->rbSerial->isChecked()) {
        if (m_isSerialOpen) {
            closeSerialPort();
        } else {
            openSerialPort();
        }
    } else if (ui->rbTcp->isChecked()) {
        if (m_tcpMode == TcpMode::Client) {
            if (m_isTcpClientConnected) {
                disconnectTcp();
            } else {
                connectTcp();
            }
        } else {
            if (m_isTcpServerListening) {
                disconnectTcp();
            } else {
                connectTcp();
            }
        }
    } else if (ui->rbUdp->isChecked()) {
        if (m_isUdpBound) {
            unbindUdp();
        } else {
            bindUdp();
        }
    }
}

void MainWindow::connectTcp()
{
    if (m_tcpMode == TcpMode::Client) {
        QString ip = ui->leTcpIP->text();
        quint16 port = ui->leTcpPort->text().toUShort();
        m_tcpClient->connectToHost(QHostAddress(ip), port);
        ui->btnConnect->setEnabled(false);
        statusBar()->showMessage(tr("Connecting to %1:%2...").arg(ip, QString::number(port)));
    } else {
        quint16 port = ui->leTcpPort->text().toUShort();
        if (m_tcpServer->listen(QHostAddress::Any, port)) {
            m_isTcpServerListening = true;
            ui->btnConnect->setText(tr("Stop"));
            ui->btnConnect->setIcon(QIcon(":/icons/disconnect.svg"));
            ui->leTcpPort->setEnabled(false);
            ui->cbTcpMode->setEnabled(false);
            statusBar()->showMessage(tr("TCP Server listening on port %1").arg(QString::number(port)));
        } else {
            QMessageBox::critical(this, tr("Error"), tr("Failed to start TCP server: %1").arg(m_tcpServer->errorString()));
        }
    }
}

void MainWindow::disconnectTcp()
{
    if (m_tcpMode == TcpMode::Client) {
        m_tcpClient->disconnectFromHost();
    } else {
        if (m_tcpServerClient) {
            m_tcpServerClient->disconnectFromHost();
            m_tcpServerClient = nullptr;
        }
        m_tcpServer->close();
        m_isTcpServerListening = false;
        ui->btnConnect->setText(tr("Start"));
        ui->btnConnect->setIcon(QIcon(":/icons/connect.svg"));
        ui->leTcpPort->setEnabled(true);
        ui->cbTcpMode->setEnabled(true);
        statusBar()->showMessage(tr("TCP Server stopped"));
    }
}

void MainWindow::onTcpClientConnected()
{
    m_isTcpClientConnected = true;
    ui->btnConnect->setEnabled(true);
    ui->btnConnect->setText(tr("Disconnect"));
    ui->btnConnect->setIcon(QIcon(":/icons/disconnect.svg"));
    ui->leTcpIP->setEnabled(false);
    ui->leTcpPort->setEnabled(false);
    ui->cbTcpMode->setEnabled(false);
    statusBar()->showMessage(tr("TCP Client connected"));
}

void MainWindow::onTcpClientDisconnected()
{
    m_isTcpClientConnected = false;
    ui->btnConnect->setEnabled(true);
    ui->btnConnect->setText(tr("Connect"));
    ui->btnConnect->setIcon(QIcon(":/icons/connect.svg"));
    ui->leTcpIP->setEnabled(true);
    ui->leTcpPort->setEnabled(true);
    ui->cbTcpMode->setEnabled(true);
    statusBar()->showMessage(tr("TCP Client disconnected"));
}

void MainWindow::onTcpClientReadyRead()
{
    QByteArray data = m_tcpClient->readAll();
    appendReceivedData(data);
}

void MainWindow::onTcpClientErrorOccurred(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError)
    ui->btnConnect->setEnabled(true);
    statusBar()->showMessage(tr("TCP Client error: %1").arg(m_tcpClient->errorString()));
}

void MainWindow::onTcpServerNewConnection()
{
    m_tcpServerClient = m_tcpServer->nextPendingConnection();
    connect(m_tcpServerClient, &QTcpSocket::readyRead, this, &MainWindow::onTcpServerReadyRead);
    connect(m_tcpServerClient, &QTcpSocket::disconnected, this, &MainWindow::onTcpClientDisconnectedFromServer);
    statusBar()->showMessage(tr("Client connected: %1:%2")
                            .arg(m_tcpServerClient->peerAddress().toString())
                            .arg(m_tcpServerClient->peerPort()));
}

void MainWindow::onTcpServerReadyRead()
{
    if (m_tcpServerClient) {
        QByteArray data = m_tcpServerClient->readAll();
        appendReceivedData(data);
    }
}

void MainWindow::onTcpClientDisconnectedFromServer()
{
    if (m_tcpServerClient) {
        m_tcpServerClient->deleteLater();
        m_tcpServerClient = nullptr;
        statusBar()->showMessage(tr("Client disconnected"));
    }
}

void MainWindow::bindUdp()
{
    QString localIP = ui->leUdpLocalIP->text();
    quint16 localPort = ui->leUdpLocalPort->text().toUShort();
    
    if (m_udpSocket->bind(QHostAddress(localIP), localPort)) {
        m_isUdpBound = true;
        ui->btnConnect->setText(tr("Unbind"));
        ui->btnConnect->setIcon(QIcon(":/icons/disconnect.svg"));
        ui->leUdpLocalIP->setEnabled(false);
        ui->leUdpLocalPort->setEnabled(false);
        statusBar()->showMessage(tr("UDP bound to %1:%2").arg(localIP, QString::number(localPort)));
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Failed to bind UDP socket: %1").arg(m_udpSocket->errorString()));
    }
}

void MainWindow::unbindUdp()
{
    m_udpSocket->abort();
    m_isUdpBound = false;
    ui->btnConnect->setText(tr("Bind"));
    ui->btnConnect->setIcon(QIcon(":/icons/connect.svg"));
    ui->leUdpLocalIP->setEnabled(true);
    ui->leUdpLocalPort->setEnabled(true);
    statusBar()->showMessage(tr("UDP unbound"));
}

void MainWindow::onUdpReadyRead()
{
    while (m_udpSocket->hasPendingDatagrams()) {
        QByteArray data;
        data.resize(m_udpSocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        m_udpSocket->readDatagram(data.data(), data.size(), &sender, &senderPort);
        
        appendReceivedData(data);
    }
}

void MainWindow::on_chkHexRecv_toggled(bool checked)
{
    Q_UNUSED(checked)
}

void MainWindow::on_chkHexSend_toggled(bool checked)
{
    Q_UNUSED(checked)
}

void MainWindow::on_chkTimestamp_toggled(bool checked)
{
    Q_UNUSED(checked)
}

void MainWindow::on_chkAutoSend_toggled(bool checked)
{
    if (checked) {
        int interval = ui->sbAutoSendInterval->value();
        m_autoSendTimer->start(interval);
    } else {
        m_autoSendTimer->stop();
    }
}

void MainWindow::onAutoSend()
{
    on_btnSend_clicked();
}

void MainWindow::on_btnFont_clicked()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, ui->teRecv->font(), this);
    if (ok) {
        ui->teRecv->setFont(font);
        ui->teSend->setFont(font);
    }
}

void MainWindow::on_btnSave_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Log"), "", tr("Text Files (*.txt);;All Files (*)"));
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << ui->teRecv->toPlainText();
            file.close();
            statusBar()->showMessage(tr("Log saved to %1").arg(fileName));
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Failed to save file"));
        }
    }
}

void MainWindow::onSerialDataReceived()
{
    QByteArray data = m_serialPort->readAll();
    appendReceivedData(data);
}

void MainWindow::appendReceivedData(const QByteArray &data)
{
    QString displayData;
    if (ui->chkHexRecv->isChecked()) {
        displayData = byteArrayToHexString(data);
    } else {
        displayData = QString::fromUtf8(data);
    }
    
    QString formatted = formatDataWithTimestamp(displayData, false);
    ui->teRecv->append(formatted);
    
    QScrollBar *sb = ui->teRecv->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void MainWindow::onSerialErrorOccurred(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Error"), tr("Serial port error: %1").arg(m_serialPort->errorString()));
        closeSerialPort();
    }
}

QByteArray MainWindow::hexStringToByteArray(const QString &hexString)
{
    QString cleanHex = hexString;
    cleanHex.remove(' ').remove('\n').remove('\r').remove('\t');
    
    QByteArray result;
    for (int i = 0; i < cleanHex.length(); i += 2) {
        QString byteString = cleanHex.mid(i, 2);
        bool ok;
        char byte = static_cast<char>(byteString.toUInt(&ok, 16));
        if (ok) {
            result.append(byte);
        }
    }
    return result;
}

QString MainWindow::byteArrayToHexString(const QByteArray &byteArray)
{
    QString result;
    for (char byte : byteArray) {
        result.append(QString("%1 ").arg(static_cast<unsigned char>(byte), 2, 16, QChar('0')).toUpper());
    }
    return result.trimmed();
}

QString MainWindow::formatDataWithTimestamp(const QString &data, bool isSend)
{
    if (ui->chkTimestamp->isChecked()) {
        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
        QString direction = isSend ? tr("Send") : tr("Recv");
        return QString("[%1] [%2] %3").arg(timestamp, direction, data);
    } else {
        QString direction = isSend ? tr("[Send] ") : tr("[Recv] ");
        return direction + data;
    }
}