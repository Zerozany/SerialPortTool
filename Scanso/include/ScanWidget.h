_Pragma("once");
#include <QDir>
#include <QFile>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QProcess>
#include <QPushButton>
#include <QTextStream>
#include <QVBoxLayout>
#include <QWidget>

#include "CreateLicense.hpp"
#include "ScanNetwork.h"

class ScanWidget : public QWidget
{
    Q_OBJECT
public:
    explicit(true) ScanWidget(QWidget* _parent = nullptr);
    ~ScanWidget() noexcept = default;

private:
    auto init_window() -> void;

    auto init_layout() -> void;

    auto signal2slots() -> void;

    auto add_device() -> void;

private:
    QPushButton* m_scan{new QPushButton{"扫描", this}};
    QPushButton* m_all_create{new QPushButton{"全部生成", this}};
    ScanNetwork* m_network{new ScanNetwork{this}};
    QGroupBox*   m_devices_box{new QGroupBox{"设备列表", this}};
    QVBoxLayout* m_devices_layout{new QVBoxLayout{m_devices_box}};
};
