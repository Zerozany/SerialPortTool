#include "ScanWidget.h"

ScanWidget::ScanWidget(QWidget* _parent) : QWidget{_parent}
{
    std::invoke(&ScanWidget::init_window, this);
    std::invoke(&ScanWidget::init_layout, this);
    std::invoke(&ScanWidget::signal2slots, this);
}

auto ScanWidget::init_window() -> void
{
    this->resize(900, 600);
    this->setWindowTitle("License Generation Tool");
}

auto ScanWidget::signal2slots() -> void
{
    connect(m_network, &ScanNetwork::devicesChanged, this, &ScanWidget::add_device);

    connect(m_scan, &QPushButton::clicked, m_network, [this] {
        QList<QWidget*> widgets = m_devices_box->findChildren<QWidget*>();
        for (QWidget* widget : widgets)
        {
            widget->deleteLater();
        }
        m_network->request();
    });
    connect(m_all_create, &QPushButton::clicked, this, [this] {
        QList<QWidget*> widgets = m_devices_box->findChildren<QWidget*>();
        for (QWidget* widget : widgets)
        {
            QPushButton* button = qobject_cast<QPushButton*>(widget);
            if (button)
            {
                button->click();
            }
        }
    });
}

auto ScanWidget::init_layout() -> void
{
    QVBoxLayout* win_layout{new QVBoxLayout{this}};
    QHBoxLayout* btn_layout{new QHBoxLayout{}};
    m_scan->setMinimumHeight(35);
    m_all_create->setMinimumHeight(35);
    btn_layout->addStretch(1);
    btn_layout->addWidget(m_scan, 1);
    btn_layout->addStretch(2);
    btn_layout->addWidget(m_all_create, 1);
    btn_layout->addStretch(1);
    win_layout->addWidget(m_devices_box, 3);
    win_layout->addLayout(btn_layout, 1);
}

auto ScanWidget::add_device() -> void
{
    QList<QWidget*> widgets = m_devices_box->findChildren<QWidget*>();
    for (QWidget* widget : widgets)
    {
        widget->deleteLater();
    }
    const std::map<QString, QString> _devices{m_network->getDevices()};
    for (const auto& [key, value] : _devices)
    {
        if (value == "none")
        {
            continue;
        }
        QWidget*     page{new QWidget{m_devices_box}};
        QHBoxLayout* layout{new QHBoxLayout{page}};
        QLabel*      tip{new QLabel{key, page}};
        QPushButton* create{new QPushButton{"创建密钥", page}};
        tip->setFixedSize(200, 40);
        create->setFixedSize(100, 40);
        layout->addStretch(1);
        layout->addWidget(tip);
        layout->addStretch(1);
        layout->addWidget(create);
        layout->addStretch(1);
        m_devices_layout->addWidget(page);
        m_devices_layout->setContentsMargins(0, 0, 0, 0);
        connect(create, &QPushButton::clicked, this, [=, this] {
            QDir dir{R"(./Licenses/)"};
            if (!dir.exists())
            {
                dir.mkpath("./");
            }
            dir.mkdir(value);
            dir.mkdir("../RarDirs");
            dir.mkpath(value + R"(/upgrade/)" + R"(xc/)");
            dir.mkpath(value + R"(/upgrade/)" + R"(linux/)" + R"(fs/)" + R"(software/)");

            QFile public_file{":/pem/public.pem"};
            if (!public_file.open(QIODevice::ReadOnly))
            {
                return;
            }
            QFile value_targte{R"(./Licenses/)" + value + R"(/public.pem)"};
            if (!value_targte.open(QIODevice::WriteOnly))
            {
                return;
            }
            value_targte.write(public_file.readAll());
            value_targte.close();

            QFile upgrade_targte{R"(./Licenses/)" + value + R"(/upgrade/linux/fs/software/public.pem)"};
            if (!upgrade_targte.open(QIODevice::WriteOnly))
            {
                return;
            }
            upgrade_targte.write(public_file.readAll());
            upgrade_targte.close();
            public_file.close();

            QFile private_file{":/pem/private.pem"};
            if (!private_file.open(QIODevice::ReadOnly))
            {
                return;
            }
            QFile value_targte_private{R"(./Licenses/)" + value + R"(/private.pem)"};
            if (!value_targte_private.open(QIODevice::WriteOnly))
            {
                return;
            }
            value_targte_private.write(private_file.readAll());
            value_targte_private.close();
            private_file.close();
            if (create_license(value.toStdString(), QString{R"(./Licenses/)" + value + R"(/upgrade/linux/fs/software/license.dat)"}.toStdString()) == -1)
            {
                return;
            }
            QProcess process{};

            // QString program{R"(C:\Users\QJZero\Desktop\Scanso\pem\Rar.exe)"};
            QString program{"./pem/Rar.exe"};

            QStringList arguments{};
            arguments << "a" << "-ep1" << "-pqjkj***" << R"(./RarDirs/)" + value + R"(.rar)" << R"(./Licenses/)" + value + R"(/upgrade/linux)" << R"(./Licenses/)" + value + R"(/upgrade/xc)";

            qDebug() << arguments.join(" ");

            process.start(program, arguments);

            if (!process.waitForStarted())
            {
                qDebug() << "Failed to start program:" << process.errorString();
                return;
            }
            if (!process.waitForFinished())
            {
                qDebug() << "Program failed to finish:" << process.errorString();
                return;
            }
            QDir remove_dir{R"(./Licenses/)" + value};
            dir.removeRecursively();
            delete page;
        });
    }
}
