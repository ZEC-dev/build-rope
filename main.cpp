#include "core.hpp"

void PrintHelp() {
    std::cout << "LenovoOnlineClassTrainer - 联想教学系统模块开关工具\n";
    std::cout << "\n用法:\n";
    std::cout << "  trainer [选项] [模块名] [on|off]\n";
    std::cout << "\n选项:\n";
    std::cout << "  -h, --help           显示此帮助信息\n";
    std::cout << "  -p, --path <路径>    设置联想教学系统安装目录（默认: C:\\Program Files (x86)\\Lenovo teaching system）\n";
    std::cout << "  -l, --list           列出所有模块及其当前状态\n";
    std::cout << "  --save [文件]        保存当前模块状态到配置文件（默认: trainer_config.txt）\n";
    std::cout << "  --load [文件]        从配置文件加载并应用状态（默认: trainer_config.txt）\n";
    std::cout << "  --export <文件>      导出当前状态到指定配置文件（同 --save 但必须指定路径）\n";
    std::cout << "  --import <文件>      从指定配置文件导入并应用状态（同 --load 但必须指定路径）\n";
    std::cout << "  --admin              检测当前是否以管理员权限运行\n";
    std::cout << "\n模块名:\n";
    std::cout << "  screen-monitor      屏幕监控\n";
    std::cout << "  screen-record       屏幕录制\n";
    std::cout << "  screen-sender       屏幕广播发送\n";
    std::cout << "  screen-broadcast    屏幕广播（录制+发送）\n";
    std::cout << "  audio-render        音频渲染\n";
    std::cout << "  audio-sender        音频发送\n";
    std::cout << "  audio-video         音视频广播\n";
    std::cout << "  file-transfer       文件传输\n";
    std::cout << "  hook                系统钩子\n";
    std::cout << "  lock-keyboard       键盘锁定\n";
    std::cout << "  black-silent        黑屏静音\n";
    std::cout << "  discuss-online      在线讨论\n";
    std::cout << "  batch-login         批量登录\n";
    std::cout << "  exam                在线考试（试卷下发）\n";
    std::cout << "  process-protect     进程保护\n";
    std::cout << "  inspect             驱动控制\n";
    std::cout << "  all                 一键全屏蔽/恢复\n";
    std::cout << "  ntsd                NTSD调试暂停（冻结/恢复学生端）\n";
    std::cout << "\n示例:\n";
    std::cout << "  trainer -p \"D:\\Lenovo teaching\" --list\n";
    std::cout << "  trainer screen-monitor on\n";
    std::cout << "  trainer all off\n";
    std::cout << "  trainer ntsd on\n";
    std::cout << "  trainer --save                      保存到 trainer_config.txt\n";
    std::cout << "  trainer --load                      从 trainer_config.txt 恢复\n";
    std::cout << "  trainer --export my_config.txt      导出到指定文件\n";
    std::cout << "  trainer --import my_config.txt      从指定文件导入\n";
    std::cout << "  trainer --admin                     检查管理员权限\n";
    std::cout << "\n说明:\n";
    std::cout << "  on  = 启用屏蔽（将模块重命名为 .bak 后缀）\n";
    std::cout << "  off = 关闭屏蔽（将 .bak 后缀还原）\n";
    std::cout << "  部分操作需要管理员权限\n";
    std::cout << "  配置文件格式：纯文本键值对，1=已屏蔽, 0=正常\n";
}

void PrintStatus() {
    std::cout << "当前安装路径: " << trainer::path << "\n";
    std::cout << "\n模块状态列表:\n";
    std::cout << "------------------------------------------------\n";
    std::cout << std::left << std::setw(20) << "模块" << std::setw(15) << "状态" << "说明\n";
    std::cout << "------------------------------------------------\n";

    auto printLine = [](const std::string& name, bool blocked, const std::string& desc) {
        std::cout << std::left << std::setw(20) << name 
                  << std::setw(15) << (blocked ? "已屏蔽" : "正常") 
                  << desc << "\n";
    };

    printLine("screen-monitor", trainer::IsDllBlocked("ScreenMonitorLib"), "屏幕监控");
    printLine("screen-record", trainer::IsDllBlocked("ScreenRecord"), "屏幕录制");
    printLine("screen-sender", trainer::IsDllBlocked("ScreenSender"), "屏幕广播发送");
    printLine("audio-render", trainer::IsDllBlocked("AudioRender"), "音频渲染");
    printLine("audio-sender", trainer::IsDllBlocked("AudioSender"), "音频发送");
    printLine("audio-video", trainer::IsExeBlocked("AudioOrVideoBroadcast"), "音视频广播");
    printLine("file-transfer", trainer::IsDllBlocked("FileTranferServer"), "文件传输");
    printLine("hook", trainer::IsDllBlocked("HookLib"), "系统钩子");
    printLine("lock-keyboard", trainer::IsDllBlocked("LockKeyboard"), "键盘锁定");
    printLine("black-silent", trainer::IsExeBlocked("BlackSlient"), "黑屏静音");
    printLine("discuss-online", trainer::IsDllBlocked("DiscussOnlineLogic"), "在线讨论");
    printLine("batch-login", trainer::IsDllBlocked("BatchLogin"), "批量登录");
    printLine("exam", trainer::IsDllBlocked("ExamSendExaminationWnd"), "在线考试");
    printLine("process-protect", trainer::IsExeBlocked("ProcessProtect"), "进程保护");
    printLine("inspect", trainer::IsSysBlocked("inspect"), "驱动控制");

    std::cout << "------------------------------------------------\n";
}

bool ParseBool(const std::string& value) {
    return value == "on" || value == "1" || value == "true" || value == "yes";
}

int ExecuteModule(const std::string& module, bool on) {
    if (module == "screen-monitor") return trainer::ScreenMonitorLib(on);
    if (module == "screen-record") return trainer::ScreenRecord(on);
    if (module == "screen-sender") return trainer::ScreenSender(on);
    if (module == "screen-broadcast") return trainer::ScreenBroadcast(on);
    if (module == "audio-render") return trainer::AudioRender(on);
    if (module == "audio-sender") return trainer::AudioSender(on);
    if (module == "audio-video") return trainer::AudioOrVideoBroadcast(on);
    if (module == "file-transfer") return trainer::FileTransfer(on);
    if (module == "hook") return trainer::Hook(on);
    if (module == "lock-keyboard") return trainer::LockKeyboard(on);
    if (module == "black-silent") return trainer::BlackSlient(on);
    if (module == "discuss-online") return trainer::DiscussOnline(on);
    if (module == "batch-login") return trainer::BatchLogin(on);
    if (module == "exam") return trainer::ExamSendExaminationWnd(on);
    if (module == "process-protect") return trainer::ProcessProtect(on);
    if (module == "inspect") return trainer::Inspect(on);
    if (module == "all") return trainer::All(on);
    if (module == "ntsd") return trainer::NTSD_Stop(on);
    
    std::cerr << "未知模块: " << module << "\n";
    std::cerr << "使用 --help 查看可用模块列表\n";
    return -1;
}

int main(int argc, char* argv[]) {
    SetConsoleOutputCP(65001);
    
    std::string defaultPath = "C:\\Program Files (x86)\\Lenovo teaching system";
    trainer::SetPath(defaultPath);

    if (argc == 1) {
        PrintHelp();
        return 0;
    }

    std::string module, action;
    bool showHelp = false;
    bool showList = false;
    bool showAdmin = false;
    // 配置操作: ""=无, "save"/"load"/"export"/"import"
    std::string configOp;
    std::string configFile;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            showHelp = true;
        } else if (arg == "-l" || arg == "--list") {
            showList = true;
        } else if ((arg == "-p" || arg == "--path") && i + 1 < argc) {
            trainer::SetPath(argv[++i]);
        } else if (arg == "--admin") {
            showAdmin = true;
        } else if (arg == "--save" || arg == "--load"
                || arg == "--export" || arg == "--import") {
            configOp = arg.substr(2);   // 去掉 "--"
            // export/import 必须带文件参数；save/load 可选
            if ((configOp == "export" || configOp == "import") && i + 1 < argc) {
                configFile = argv[++i];
            } else if ((configOp == "save" || configOp == "load") && i + 1 < argc
                       && argv[i + 1][0] != '-') {
                // 下一个参数若不以 '-' 开头则视为文件名
                configFile = argv[++i];
            }
        } else if (module.empty()) {
            module = arg;
        } else if (action.empty()) {
            action = arg;
        }
    }

    if (showHelp) {
        PrintHelp();
        return 0;
    }

    if (showAdmin) {
        if (trainer::IsAdmin()) {
            std::cout << "当前以管理员权限运行 ✓\n";
        } else {
            std::cout << "当前未以管理员权限运行 ✗\n";
            std::cout << "提示: 部分操作（如驱动控制、NTSD）需要管理员权限\n";
        }
        return 0;
    }

    if (showList) {
        PrintStatus();
        return 0;
    }

    // 配置保存/加载
    if (!configOp.empty()) {
        if (configFile.empty()) {
            configFile = "trainer_config.txt";
        }
        if (configOp == "save" || configOp == "export") {
            int rc = trainer::SaveConfig(configFile);
            if (rc == 0) {
                std::cout << "配置已保存到: " << configFile << "\n";
            } else {
                std::cerr << "错误: 无法写入配置文件 " << configFile << "\n";
            }
            return rc;
        } else { // load / import
            int rc = trainer::LoadConfig(configFile);
            if (rc == 0) {
                std::cout << "配置已从 " << configFile << " 加载并应用\n";
            } else {
                std::cerr << "错误: 无法读取配置文件 " << configFile << "\n";
            }
            return rc;
        }
    }

    if (!module.empty() && !action.empty()) {
        bool on = ParseBool(action);
        std::cout << (on ? "屏蔽" : "恢复") << "模块: " << module << "\n";
        int result = ExecuteModule(module, on);
        if (result == 0) {
            std::cout << "操作成功\n";
        } else if (result == -2) {
            std::cerr << "错误: ntsd.exe 未找到，请放置到工作目录或使用正确路径\n";
        } else if (result == -1) {
            std::cerr << "错误: 操作失败或模块不存在\n";
        } else {
            std::cerr << "操作完成，返回码: " << result << "\n";
        }
        return result;
    }

    std::cerr << "参数错误！使用 --help 查看用法\n";
    return 1;
}
