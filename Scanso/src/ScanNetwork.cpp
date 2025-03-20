#include "ScanNetwork.h"

ScanNetwork::ScanNetwork(QObject* _parent) : QObject{_parent}
{
    std::invoke(&ScanNetwork::init_socket, this);
    std::invoke(&ScanNetwork::request, this);
    std::invoke(&ScanNetwork::signal2slots, this);
}

ScanNetwork::~ScanNetwork() noexcept
{
    if (m_upd_socket)
    {
        m_upd_socket->close();
    }
}

void ScanNetwork::init_socket()
{
    m_upd_socket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    if (m_upd_socket->bind(QHostAddress::AnyIPv4, UDP_PORT))
    {
        m_upd_socket->joinMulticastGroup(QHostAddress{GROUP_ADDRESS});
    }
}

void ScanNetwork::signal2slots()
{
    connect(m_upd_socket, &QUdpSocket::readyRead, this, &ScanNetwork::handle_data);
}

void ScanNetwork::request()
{
    m_exist_ip.clear();
    for (const QNetworkInterface& interface : QNetworkInterface::allInterfaces())
    {
        if (!interface.flags().testFlag(QNetworkInterface::IsUp))
        {
            continue;
        }
        if (interface.flags().testFlag(QNetworkInterface::IsLoopBack))
        {
            continue;
        }
        for (const QNetworkAddressEntry& entry : interface.addressEntries())
        {
            if (entry.broadcast() == QHostAddress::Null)
            {
                continue;
            }
            QNetworkDatagram datagram{};
            datagram.setData(m_send_byte);
            datagram.setSender(entry.ip());
            datagram.setDestination(QHostAddress::Broadcast, UDP_PORT);
            m_upd_socket->writeDatagram(datagram);
        }
    }
}

std::map<QString, QString> ScanNetwork::getDevices()
{
    return m_devices;
}

void ScanNetwork::setDevices(const QString& _ip, const QString& _id)
{
    {
        std::lock_guard<std::mutex> lock{_mtx};
        m_devices.emplace(_ip, _id);
    }
    emit devicesChanged();
}

void ScanNetwork::handle_data()
{
    while (m_upd_socket->hasPendingDatagrams())
    {
        QByteArray   data_gram{};
        QHostAddress sender_address{};
        data_gram.resize(m_upd_socket->pendingDatagramSize());
        m_upd_socket->readDatagram(data_gram.data(), data_gram.size(), &sender_address, nullptr);
        // 如果获取的数据
        if (data_gram == m_read_byte)
        {
            if (m_exist_ip.contains(sender_address))
            {
                return;
            }
            m_exist_ip.append(sender_address);
            std::jthread([this, sender_address]() -> void {
                QTcpSocket tcp_socket{};
                // 设置无代理
                tcp_socket.setProxy(QNetworkProxy::NoProxy);
                // 设置超时时间（例如 5 秒）
                tcp_socket.connectToHost(sender_address, TCP_PORT);
                // 等待连接成功
                tcp_socket.waitForConnected(3000);
                if (tcp_socket.state() != QAbstractSocket::ConnectedState)
                {
                    return;
                }
                if (QJsonParseError error{}; tcp_socket.waitForReadyRead(500))
                {
                    QJsonDocument json_doc{QJsonDocument::fromJson(tcp_socket.readAll(), &error)};
                    if (error.error != QJsonParseError::NoError)
                    {
                        return;
                    }
                    // 检查 JSON 是否为对象
                    if (json_doc.isObject())
                    {
                        // qDebug() << json_doc.toJson();
                        QJsonObject json_obj{json_doc.object()};
                        QString     dev_ip{};
                        QString     soc_sn{};
                        if (json_obj.contains("dev_ip") && json_obj["dev_ip"].isString())
                        {
                            dev_ip = json_obj["dev_ip"].toString();
                            qDebug() << "dev_ip:" << dev_ip;
                        }
                        if (json_obj.contains("soc_sn") && json_obj["soc_sn"].isString())
                        {
                            soc_sn = json_obj["soc_sn"].toString();
                            qDebug() << "soc_sn:" << soc_sn;
                        }
                        else
                        {
                            soc_sn = "none";
                        }
                        this->setDevices(dev_ip, soc_sn);
                        tcp_socket.close();
                        return;
                    }
                }
                tcp_socket.close();
            }).detach();
        }
        else if (data_gram == m_prepared_byte)
        {
            request();
        }
    }
}
