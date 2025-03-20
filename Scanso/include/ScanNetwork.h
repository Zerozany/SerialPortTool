_Pragma("once");

#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkDatagram>
#include <QNetworkInterface>
#include <QNetworkProxy>
#include <QObject>
#include <QStringList>
#include <QTcpSocket>
#include <QThread>
#include <QUdpSocket>
#include <QtConcurrent>
#include <functional>
#include <map>
#include <mutex>

constexpr int  UDP_PORT{6000};
constexpr int  TCP_PORT{6001};
constexpr char GROUP_ADDRESS[]{"224.1.1.10"};

class ScanNetwork : public QObject
{
    Q_OBJECT
public:
    explicit(true) ScanNetwork(QObject* _parent = nullptr);
    ~ScanNetwork() noexcept;

    void request();

    [[nodiscard]] std::map<QString, QString> getDevices();

signals:
    void devicesChanged();

private:
    void init_socket();

    void signal2slots();

    void handle_data();

    void setDevices(const QString& _ip, const QString& _id);

private:
    QUdpSocket*                m_upd_socket{new QUdpSocket{this}};
    QList<QHostAddress>        m_exist_ip{};
    QByteArray                 m_send_byte{"QJ shakehand"};
    QByteArray                 m_read_byte{"QJ ready to send file"};
    QByteArray                 m_prepared_byte{"QJ socket prepared ready!"};
    std::map<QString, QString> m_devices{};
    std::mutex                 _mtx{};
};
