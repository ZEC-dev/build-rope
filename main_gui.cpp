// =============================================================================
//  LenovoOnlineClassTrainer GUI - 基于 Qt5/Qt6 的图形界面
//
//  依赖：Qt5Widgets（Qt6 也兼容）
//  构建（Windows + MinGW，Qt 安装在 C:\Qt）：
//      C:\Qt\5.15.2\mingw1310_64\bin\qmake.exe -project
//      C:\Qt\5.15.2\mingw1310_64\bin\qmake.exe
//      mingw32-make release
//      windeployqt trainer_gui.exe
//
//  核心：调用 core.hpp 中的 trainer::* 函数，不修改现有逻辑。
//  与 main.cpp（CLI 入口）并存，构建时只编译其中之一。
// =============================================================================

#include "core.hpp"

#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QTabWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QFileDialog>
#include <QStatusBar>
#include <QDateTime>
#include <QMessageBox>
#include <QHash>

// -----------------------------------------------------------------------------
//  模块描述
// -----------------------------------------------------------------------------
struct ModuleInfo {
    QString name;          // 显示名（中文）
    QString key;           // 唯一 key（与 trainer::* 函数对应）
    QString type;          // "dll" / "exe" / "sys" / "ntsd"
    QString fileBase;      // core.hpp 中的实际文件名（用于状态查询）
    QString desc;          // 简短描述
};

static const QList<ModuleInfo> kScreenModules = {
    {"屏幕监控",     "screen-monitor",  "dll", "ScreenMonitorLib",       "教师远程查看学生屏幕"},
    {"屏幕录制",     "screen-record",   "dll", "ScreenRecord",           "录制学生端屏幕"},
    {"屏幕广播发送", "screen-sender",   "dll", "ScreenSender",           "发送广播屏幕"},
    {"屏幕广播",     "screen-broadcast","dll", "",                       "录制+发送 一并控制"},
};

static const QList<ModuleInfo> kAudioVideoModules = {
    {"音频渲染",   "audio-render", "dll", "AudioRender",            "播放广播音频"},
    {"音频发送",   "audio-sender", "dll", "AudioSender",            "发送学生端麦克风"},
    {"音视频广播", "audio-video",  "exe", "AudioOrVideoBroadcast",  "音视频广播主程序"},
};

static const QList<ModuleInfo> kFileTransferModules = {
    {"文件传输", "file-transfer", "dll", "FileTranferServer", "服务端+客户端+核心库"},
};

static const QList<ModuleInfo> kSystemControlModules = {
    {"系统钩子",  "hook",          "dll", "HookLib",      "全局钩子库"},
    {"键盘锁定",  "lock-keyboard", "dll", "LockKeyboard", "锁住学生端键盘"},
    {"黑屏静音",  "black-silent",  "exe", "BlackSlient",  "一键让学生机黑屏+静音"},
    {"驱动控制",  "inspect",       "sys", "inspect",      "inspect.sys/inf + 服务控制"},
};

static const QList<ModuleInfo> kProcessExamModules = {
    {"在线讨论",   "discuss-online",  "dll",  "DiscussOnlineLogic",       "互动讨论模块"},
    {"批量登录",   "batch-login",     "dll",  "BatchLogin",               "学生机统一认证"},
    {"在线考试",   "exam",            "dll",  "ExamSendExaminationWnd",   "试卷下发窗口"},
    {"进程保护",   "process-protect", "exe",  "ProcessProtect",           "防止关闭学生端"},
    {"NTSD暂停",   "ntsd",            "ntsd", "",                          "冻结运行中的学生端"},
};

// -----------------------------------------------------------------------------
//  MainWindow
// -----------------------------------------------------------------------------
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("LenovoOnlineClassTrainer - 联想教学系统模块开关");
        resize(820, 640);

        // ---- 路径设置栏 ----
        QWidget* pathBar = new QWidget;
        QHBoxLayout* pathLayout = new QHBoxLayout(pathBar);
        pathLayout->setContentsMargins(8, 8, 8, 4);
        QLabel* pathLabel = new QLabel("安装路径:");
        pathEdit_ = new QLineEdit;
        pathEdit_->setText("C:/Program Files (x86)/Lenovo teaching system/");
        pathEdit_->setPlaceholderText("选择或粘贴联想教学系统安装目录");
        QPushButton* browseBtn    = new QPushButton("浏览...");
        QPushButton* applyPathBtn = new QPushButton("应用");
        QPushButton* refreshBtn   = new QPushButton("刷新状态");
        pathLayout->addWidget(pathLabel);
        pathLayout->addWidget(pathEdit_, 1);
        pathLayout->addWidget(browseBtn);
        pathLayout->addWidget(applyPathBtn);
        pathLayout->addWidget(refreshBtn);
        connect(browseBtn,    &QPushButton::clicked, this, &MainWindow::onBrowsePath);
        connect(applyPathBtn, &QPushButton::clicked, this, &MainWindow::onApplyPath);
        connect(refreshBtn,   &QPushButton::clicked, this, &MainWindow::refreshAll);

        // ---- 选项卡 ----
        tabWidget_ = new QTabWidget;
        tabWidget_->addTab(buildModuleTab(kScreenModules),        "屏幕相关");
        tabWidget_->addTab(buildModuleTab(kAudioVideoModules),    "音视频广播");
        tabWidget_->addTab(buildModuleTab(kFileTransferModules),  "文件传输");
        tabWidget_->addTab(buildModuleTab(kSystemControlModules), "系统控制");
        tabWidget_->addTab(buildModuleTab(kProcessExamModules),   "进程与考试");

        // ---- 底部一键控制 ----
        QWidget* bottomBar = new QWidget;
        QHBoxLayout* bottomLayout = new QHBoxLayout(bottomBar);
        bottomLayout->setContentsMargins(8, 4, 8, 4);
        QPushButton* blockAllBtn   = new QPushButton("一键全屏蔽");
        QPushButton* restoreAllBtn = new QPushButton("一键全恢复");
        bottomLayout->addWidget(blockAllBtn);
        bottomLayout->addWidget(restoreAllBtn);
        bottomLayout->addStretch();
        connect(blockAllBtn,   &QPushButton::clicked, this, &MainWindow::onBlockAll);
        connect(restoreAllBtn, &QPushButton::clicked, this, &MainWindow::onRestoreAll);

        logEdit_ = new QPlainTextEdit;
        logEdit_->setReadOnly(true);
        logEdit_->setMaximumBlockCount(1000);
        logEdit_->setPlaceholderText("操作日志将显示在这里...");

        // ---- 整体布局 ----
        QWidget* central = new QWidget;
        QVBoxLayout* main = new QVBoxLayout(central);
        main->setContentsMargins(0, 0, 0, 0);
        main->setSpacing(0);
        main->addWidget(pathBar);
        main->addWidget(tabWidget_, 1);
        main->addWidget(bottomBar);
        main->addWidget(new QLabel("操作日志:"));
        main->addWidget(logEdit_, 1);
        setCentralWidget(central);

        statusBar()->showMessage("就绪");
        log("程序启动。请先在顶部设置正确的安装路径，然后点击「刷新状态」。");

        // 应用初始路径
        trainer::SetPath(pathEdit_->text().toStdString());
    }

private:
    QLineEdit*       pathEdit_   = nullptr;
    QTabWidget*      tabWidget_  = nullptr;
    QPlainTextEdit*  logEdit_    = nullptr;

    // 每行控件，按 key 索引
    struct ModuleRow {
        QLabel*      statusLabel = nullptr;
        QPushButton* toggleBtn   = nullptr;
        QLabel*      descLabel   = nullptr;
        ModuleInfo   info;
    };
    QHash<QString, ModuleRow> rows_;

    // -------------------------------------------------------------------------
    //  构建分类选项卡的一页
    // -------------------------------------------------------------------------
    QWidget* buildModuleTab(const QList<ModuleInfo>& modules) {
        QWidget* page = new QWidget;
        QVBoxLayout* root = new QVBoxLayout(page);
        root->setContentsMargins(8, 8, 8, 8);
        root->setSpacing(6);

        for (const ModuleInfo& mi : modules) {
            QHBoxLayout* row = new QHBoxLayout;
            row->setSpacing(8);

            QLabel* nameLabel = new QLabel(mi.name);
            nameLabel->setMinimumWidth(110);
            nameLabel->setStyleSheet("font-weight: bold;");

            QLabel* statusLabel = new QLabel("?");
            statusLabel->setFixedSize(80, 22);
            statusLabel->setAlignment(Qt::AlignCenter);
            statusLabel->setStyleSheet(
                "background:#9E9E9E; color:white; border-radius:3px;");

            QLabel* descLabel = new QLabel(mi.desc);
            descLabel->setStyleSheet("color:#666;");

            QPushButton* toggleBtn = new QPushButton("屏蔽");
            toggleBtn->setMinimumWidth(90);
            toggleBtn->setFixedHeight(28);

            row->addWidget(nameLabel);
            row->addWidget(statusLabel);
            row->addWidget(descLabel, 1);
            row->addWidget(toggleBtn);
            root->addLayout(row);

            ModuleRow mr{statusLabel, toggleBtn, descLabel, mi};
            rows_.insert(mi.key, mr);

            // 用 lambda 捕获 key，再触发对应的处理
            connect(toggleBtn, &QPushButton::clicked, this, [this, key = mi.key]() {
                onToggleClicked(key);
            });
        }
        root->addStretch();
        return page;
    }

    // -------------------------------------------------------------------------
    //  日志
    // -------------------------------------------------------------------------
    void log(const QString& msg) {
        QString ts = QDateTime::currentDateTime().toString("HH:mm:ss");
        logEdit_->appendPlainText(QString("[%1] %2").arg(ts, msg));
    }

    // -------------------------------------------------------------------------
    //  路径设置
    // -------------------------------------------------------------------------
    void onBrowsePath() {
        QString dir = QFileDialog::getExistingDirectory(
            this, "选择联想教学系统安装目录", pathEdit_->text());
        if (!dir.isEmpty()) {
            if (!dir.endsWith('/') && !dir.endsWith('\\')) dir += '/';
            pathEdit_->setText(dir);
        }
    }

    void onApplyPath() {
        QString p = pathEdit_->text();
        if (p.isEmpty()) {
            QMessageBox::warning(this, "路径错误", "路径不能为空");
            return;
        }
        if (!p.endsWith('/') && !p.endsWith('\\')) p += '/';
        trainer::SetPath(p.toStdString());
        log(QString("已设置安装路径: %1").arg(p));
        statusBar()->showMessage("路径已应用", 3000);
        refreshAll();
    }

    // -------------------------------------------------------------------------
    //  刷新所有模块状态
    // -------------------------------------------------------------------------
    void refreshAll() {
        if (trainer::path.empty()) {
            log("错误：未设置安装路径");
            return;
        }

        int updated = 0, missing = 0;
        for (auto it = rows_.begin(); it != rows_.end(); ++it) {
            const ModuleRow& mr = it.value();
            QString state, color;
            const QString& t = mr.info.type;
            std::string base = mr.info.fileBase.toStdString();

            if (t == "dll") {
                bool blocked = trainer::IsDllBlocked(base);
                bool exists  = trainer::FileExist(base + ".dll") || blocked;
                if (!exists)        { state = "未找到"; color = "#9E9E9E"; missing++; }
                else if (blocked)   { state = "已屏蔽"; color = "#F44336"; }
                else                { state = "正常";   color = "#4CAF50"; }
            } else if (t == "exe") {
                bool blocked = trainer::IsExeBlocked(base);
                bool exists  = trainer::FileExist(base + ".exe") || blocked;
                if (!exists)        { state = "未找到"; color = "#9E9E9E"; missing++; }
                else if (blocked)   { state = "已屏蔽"; color = "#F44336"; }
                else                { state = "正常";   color = "#4CAF50"; }
            } else if (t == "sys") {
                bool blocked = trainer::IsSysBlocked(base);
                bool exists  = trainer::FileExist(base + ".sys") || blocked;
                if (!exists)        { state = "未找到"; color = "#9E9E9E"; missing++; }
                else if (blocked)   { state = "已屏蔽"; color = "#F44336"; }
                else                { state = "正常";   color = "#4CAF50"; }
            } else if (t == "ntsd") {
                int pid = trainer::FindPID("ntsd.exe");
                if (pid > 0)        { state = "已冻结"; color = "#FF9800"; }
                else                { state = "未运行"; color = "#2196F3"; }
            } else {
                state = "未知"; color = "#9E9E9E";
            }

            mr.statusLabel->setText(state);
            mr.statusLabel->setStyleSheet(
                QString("background:%1; color:white; border-radius:3px;").arg(color));
            updated++;
        }
        log(QString("状态刷新完成：共 %1 项，%2 项文件不存在")
                .arg(updated).arg(missing));
    }

    // -------------------------------------------------------------------------
    //  单击某行的「屏蔽/恢复」按钮
    // -------------------------------------------------------------------------
    void onToggleClicked(const QString& key) {
        if (!rows_.contains(key)) return;
        ModuleRow& mr = rows_[key];

        // 根据当前状态决定本次操作方向
        bool currentlyBlocked = false;
        const QString& t = mr.info.type;
        std::string base = mr.info.fileBase.toStdString();
        if (t == "dll")       currentlyBlocked = trainer::IsDllBlocked(base);
        else if (t == "exe")  currentlyBlocked = trainer::IsExeBlocked(base);
        else if (t == "sys")  currentlyBlocked = trainer::IsSysBlocked(base);
        else if (t == "ntsd") {
            currentlyBlocked = trainer::FindPID("ntsd.exe") > 0;
        }

        bool wantBlock = !currentlyBlocked;
        QString op = wantBlock ? "屏蔽" : "恢复";

        if (mr.info.type != "ntsd" && mr.info.type != "sys") {
            // 简单确认（驱动和 NTSD 影响较大，跳过弹窗以保持流畅）
            // 跳过弹窗以避免每行都点 Yes
        }

        bool ok = executeModule(key, wantBlock);
        if (ok) {
            mr.toggleBtn->setText(currentlyBlocked ? "屏蔽" : "恢复");
        }
        refreshAll();
    }

    // -------------------------------------------------------------------------
    //  调用 core.hpp 的对应函数
    // -------------------------------------------------------------------------
    bool executeModule(const QString& key, bool on) {
        std::string k = key.toStdString();
        int rc = 0;
        if      (k == "screen-monitor")   rc = trainer::ScreenMonitorLib(on);
        else if (k == "screen-record")    rc = trainer::ScreenRecord(on);
        else if (k == "screen-sender")    rc = trainer::ScreenSender(on);
        else if (k == "screen-broadcast") rc = trainer::ScreenBroadcast(on);
        else if (k == "audio-render")     rc = trainer::AudioRender(on);
        else if (k == "audio-sender")     rc = trainer::AudioSender(on);
        else if (k == "audio-video")      rc = trainer::AudioOrVideoBroadcast(on);
        else if (k == "file-transfer")    rc = trainer::FileTransfer(on);
        else if (k == "hook")             rc = trainer::Hook(on);
        else if (k == "lock-keyboard")    rc = trainer::LockKeyboard(on);
        else if (k == "black-silent")     rc = trainer::BlackSlient(on);
        else if (k == "discuss-online")   rc = trainer::DiscussOnline(on);
        else if (k == "batch-login")      rc = trainer::BatchLogin(on);
        else if (k == "exam")             rc = trainer::ExamSendExaminationWnd(on);
        else if (k == "process-protect")  rc = trainer::ProcessProtect(on);
        else if (k == "inspect")          rc = trainer::Inspect(on);
        else if (k == "ntsd")             rc = trainer::NTSD_Stop(on);
        else { log("未知模块: " + key); return false; }

        QString op = on ? "屏蔽" : "恢复";
        if (rc == 0) {
            log(QString("%1 %2 成功").arg(op).arg(key));
            return true;
        } else if (rc == -2) {
            log(QString("%1 %2 失败：ntsd.exe 未找到").arg(op).arg(key));
        } else if (rc == -1) {
            log(QString("%1 %2 失败：目标未运行或文件不存在").arg(op).arg(key));
        } else {
            log(QString("%1 %2 完成，rc=%3").arg(op).arg(key).arg(rc));
        }
        return false;
    }

    // -------------------------------------------------------------------------
    //  一键控制
    // -------------------------------------------------------------------------
    void onBlockAll() {
        if (QMessageBox::question(this, "确认", "确定要一键屏蔽所有模块？")
            != QMessageBox::Yes) return;
        log("===== 一键全屏蔽开始 =====");
        int rc = trainer::All(true);
        log(QString("===== 一键全屏蔽结束，rc=%1 =====").arg(rc));
        refreshAll();
    }

    void onRestoreAll() {
        if (QMessageBox::question(this, "确认", "确定要一键恢复所有模块？")
            != QMessageBox::Yes) return;
        log("===== 一键全恢复开始 =====");
        int rc = trainer::All(false);
        log(QString("===== 一键全恢复结束，rc=%1 =====").arg(rc));
        refreshAll();
    }
};

#include "main_gui.moc"
