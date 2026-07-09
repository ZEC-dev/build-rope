#pragma once
#include<bits/stdc++.h>
#include<windows.h>
#include<time.h>

// =============================================================================
//  trainer  ——  联想教学系统 (Lenovo Teaching System) 模块开关工具
//
//  原理：该教学软件把各项功能拆成独立的 .dll / .exe 模块，主程序在运行时按
//  文件名加载它们。把模块文件重命名为 .bak 后缀，主程序便找不到该模块，对应
//  功能即被“屏蔽”；改回原文件名即可恢复。
//
//  约定（与已有 ScreenMonitorLib 保持一致）：
//      on = true  -> 启用屏蔽：把  X.dll/X.exe  重命名为  X.bak.dll/X.bak.exe
//      on = false -> 关闭屏蔽：把  X.bak.*       还原为    X.dll/X.exe
//
//  使用前请先调用 SetPath() 设置软件安装目录（末尾带 '/'）。
// =============================================================================

namespace trainer {
    // 软件安装路径（末尾需带路径分隔符，如 "C:/Program Files/LenovoTeach/"）
    std::string path;

    // ---------------------------------------------------------------------------
    //  基础工具
    // ---------------------------------------------------------------------------

    // 设置安装路径
    void SetPath(std::string p){
        path = p;
    }

    // 重命名安装路径下的文件，返回 rename() 的结果（0 表示成功）
    int BasicRename(std::string OldName, std::string NewName){
        //重命名安装路径下的文件
        std::string OldPath = path + OldName;
        std::string NewPath = path + NewName;
        return rename(OldPath.c_str(), NewPath.c_str());
    }

    // 检查文件是否存在
    int FileExist(std::string Name){
        //检查文件是否存在
        std::ifstream file((path + Name).c_str());
        return file.good();
    }

    // 通用：屏蔽/恢复一个 .dll 模块
    int ToggleDll(std::string Name, bool on){
        if(on)
            return BasicRename(Name + ".dll", Name + ".bak.dll");
        else
            return BasicRename(Name + ".bak.dll", Name + ".dll");
    }

    // 通用：屏蔽/恢复一个 .exe 程序
    int ToggleExe(std::string Name, bool on){
        if(on)
            return BasicRename(Name + ".exe", Name + ".bak.exe");
        else
            return BasicRename(Name + ".bak.exe", Name + ".exe");
    }

    // ---------------------------------------------------------------------------
    //  1. 屏幕相关
    // ---------------------------------------------------------------------------

    int ScreenMonitorLib(bool on){
        //屏幕监控程序（教师远程看学生屏幕），启用屏蔽为on=true，否则为on=false
        return ToggleDll("ScreenMonitorLib", on);
    }

    int ScreenRecord(bool on){
        //屏幕录制模块
        return ToggleDll("ScreenRecord", on);
    }

    int ScreenSender(bool on){
        //屏幕广播发送模块
        return ToggleDll("ScreenSender", on);
    }

    // 屏幕广播：同时控制录制与发送
    int ScreenBroadcast(bool on){
        ScreenRecord(on);
        return ScreenSender(on);
    }

    // ---------------------------------------------------------------------------
    //  2. 音视频广播
    // ---------------------------------------------------------------------------

    int AudioRender(bool on){
        //音频渲染模块
        return ToggleDll("AudioRender", on);
    }

    int AudioSender(bool on){
        //音频发送模块
        return ToggleDll("AudioSender", on);
    }

    int AudioOrVideoBroadcast(bool on){
        //音视频广播程序（.exe）+ 音频渲染 + 音频发送，一键屏蔽/恢复
        ToggleExe("AudioOrVideoBroadcast", on);
        AudioRender(on);
        return AudioSender(on);
    }

    // ---------------------------------------------------------------------------
    //  3. 文件传输（收发作业 / 课件）
    // ---------------------------------------------------------------------------

    int FileTransferServer(bool on){
        //文件传输服务端（注意原文件名拼写为 Tranfer）
        return ToggleDll("FileTranferServer", on);
    }

    int FileTransferClient(bool on){
        //文件传输客户端程序
        return ToggleExe("FileTransferClient", on);
    }

    int FileTransferDll(bool on){
        //文件传输核心库
        return ToggleDll("filetransfer", on);
    }

    // 文件传输总开关：服务端 + 客户端 + 核心库
    int FileTransfer(bool on){
        FileTransferServer(on);
        FileTransferClient(on);
        return FileTransferDll(on);
    }

    // ---------------------------------------------------------------------------
    //  4. 系统钩子 / 键盘锁定（实现“锁屏”、“禁止学生操作”）
    // ---------------------------------------------------------------------------

    int HookLib(bool on){
        //系统钩子库
        return ToggleDll("HookLib", on);
    }

    int HookManager(bool on){
        //钩子管理库
        return ToggleDll("HookManager", on);
    }

    int HookProcDll(bool on){
        //钩子过程库
        return ToggleDll("HookProcDll", on);
    }

    // 钩子总开关：三个钩子库一并屏蔽/恢复
    int Hook(bool on){
        HookLib(on);
        HookManager(on);
        return HookProcDll(on);
    }

    int LockKeyboard(bool on){
        //锁键盘模块
        return ToggleDll("LockKeyboard", on);
    }

    // ---------------------------------------------------------------------------
    //  5. 黑屏静音
    // ---------------------------------------------------------------------------

    int BlackSlient(bool on){
        //黑屏静音程序，教师一键让学生机黑屏并静音
        return ToggleExe("BlackSlient", on);
    }

    // ---------------------------------------------------------------------------
    //  6. 在线讨论 / 互动教学
    // ---------------------------------------------------------------------------

    int DiscussOnline(bool on){
        //在线讨论模块
        return ToggleDll("DiscussOnlineLogic", on);
    }

    // ---------------------------------------------------------------------------
    //  7. 批量登录
    // ---------------------------------------------------------------------------

    int BatchLogin(bool on){
        //批量登录模块（学生机统一认证登录）
        return ToggleDll("BatchLogin", on);
    }

    // ---------------------------------------------------------------------------
    //  8. 在线考试 / 测验
    //    说明文档仅给出 Exam*.dll / Exam*.exe 系列（如 ExamSendExaminationWnd），
    //    具体文件名因版本而异，故提供按文件名(不含扩展名)屏蔽的接口。
    // ---------------------------------------------------------------------------

    int Exam(std::string Name, bool on){
        //在线考试模块：Name 为不含扩展名的文件名（如 "ExamSendExaminationWnd"）
        ToggleDll(Name, on);
        return ToggleExe(Name, on);
    }

    // 常见考试模块快捷开关（试卷下发窗口）
    int ExamSendExaminationWnd(bool on){
        return Exam("ExamSendExaminationWnd", on);
    }

    // ---------------------------------------------------------------------------
    //  9. 进程保护（防止学生强行关闭学生端）
    // ---------------------------------------------------------------------------

    int ProcessProtect(bool on){
        //进程保护程序
        return ToggleExe("ProcessProtect", on);
    }

    // ---------------------------------------------------------------------------
    //  10. 驱动（inspect.sys / inspect.inf）
    //      底层监控与键盘输入拦截，属于系统级保护。钩子(LockKeyboard 等)依赖它。
    //
    //      注意：仅重命名 .sys 文件只能阻止“下次开机”加载，对已加载到内存的
    //      驱动无效。要真正禁用一个正在运行的驱动，必须停止并禁用其 Windows
    //      服务。因此提供两个层次：
    //          InspectFile(on)    —— 仅重命名文件（下次开机生效）
    //          InspectService(on) —— 通过 sc 停止/禁用 或 启动/启用 服务（立即生效）
    //          Inspect(on)        —— 二者合一（推荐）
    //      驱动服务名默认为 "inspect"，若实际不同可通过参数覆盖。
    //      所有 sc 操作均需管理员权限运行。
    // ---------------------------------------------------------------------------

    // 通用：屏蔽/恢复一个 .sys 驱动文件
    int ToggleSys(std::string Name, bool on){
        if(on)
            return BasicRename(Name + ".sys", Name + ".bak.sys");
        else
            return BasicRename(Name + ".bak.sys", Name + ".sys");
    }

    // 通用：屏蔽/恢复一个 .inf 信息文件
    int ToggleInf(std::string Name, bool on){
        if(on)
            return BasicRename(Name + ".inf", Name + ".bak.inf");
        else
            return BasicRename(Name + ".bak.inf", Name + ".inf");
    }

    // 查询 .dll 模块状态：返回 true 表示已被屏蔽（.bak.dll 存在），false 表示正常
    bool IsDllBlocked(std::string Name){
        return FileExist(Name + ".bak.dll");
    }

    // 查询 .exe 程序状态：返回 true 表示已被屏蔽（.bak.exe 存在），false 表示正常
    bool IsExeBlocked(std::string Name){
        return FileExist(Name + ".bak.exe");
    }

    // 查询 .sys 驱动状态：返回 true 表示已被屏蔽（.bak.sys 存在），false 表示正常
    bool IsSysBlocked(std::string Name){
        return FileExist(Name + ".bak.sys");
    }

    // 仅重命名驱动文件：inspect.sys / inspect.inf
    int InspectFile(bool on){
        ToggleInf("inspect", on);
        return ToggleSys("inspect", on);
    }

    // 通过 Windows 服务控制驱动：on=true 停止并禁用，on=false 启用并启动
    //   svc      驱动服务名，默认 "inspect"
    //   startType 恢复时设置的启动方式，默认 "boot"（开机启动，内核驱动常用）
    //             可选 "boot" / "system" / "demand" / "auto"
    int InspectService(bool on, std::string svc = "inspect",std::string startType = "boot"){
        std::string cmd;
        if(on){
            //停止运行中的驱动，并设置为禁用（立即生效且重启后不再加载）
            system(("sc stop "    + svc                    ).c_str());
            cmd = "sc config " + svc + " start= disabled";
        }else{
            //恢复启动方式并启动
            cmd = "sc config " + svc + " start= " + startType;
            system(cmd.c_str());
            return system(("sc start "  + svc               ).c_str());
        }
        return system(cmd.c_str());
    }

    // 驱动总开关：先停服务（立即生效），再重命名文件（防止重启后被自动恢复）
    int Inspect(bool on, std::string svc = "inspect",
                std::string startType = "boot"){
        InspectService(on, svc, startType);
        return InspectFile(on);
    }

    // ---------------------------------------------------------------------------
    //  11. 一键全屏蔽 / 全恢复（除核心 Student.exe / Teacher.exe 外的功能模块）
    // ---------------------------------------------------------------------------

    int All(bool on){
        Inspect(on);                    //驱动必须最先处理（底层依赖）
        ScreenBroadcast(on);
        AudioOrVideoBroadcast(on);
        FileTransfer(on);
        Hook(on);
        LockKeyboard(on);
        BlackSlient(on);
        DiscussOnline(on);
        BatchLogin(on);
        ProcessProtect(on);
        ExamSendExaminationWnd(on);
        return ScreenMonitorLib(on);
    }

    // ---------------------------------------------------------------------------
    //  12. ntsd 调试暂停
    //      原理同 hack.bat：用 Windows 自带的用户态调试器 ntsd.exe 附加到目标
    //      进程，使其进入"被调试"状态而立即挂起；ntsd 退出时调试器 detach，
    //      目标进程恢复运行。
    //
    //      相比重命名 .dll / .exe 文件，ntsd 方式对"已经在内存中运行"的进程
    //      立即生效，无需重启软件；缺点是：
    //        ① Win10/11 已不再默认内置 ntsd.exe，需自行放置到工作目录或通过
    //           ntsdPath 参数指定路径；
    //        ② 需管理员权限；
    //        ③ force-kill ntsd 时被调试进程也可能被一并终止（Win32 调试子系统
    //           默认行为）。如需"干净解冻"，建议在弹出的 ntsd 窗口内手动输入
    //           q 命令退出，再调用 NTSD_Stop(false) 兜底。
    //
    //      典型用途：直接冻住学生端 Student.exe，让屏幕监控 / 键盘钩子等
    //      所有依赖主进程的功能立刻失效。
    //
    //      on=true   附加调试器，冻结目标进程
    //      on=false  结束 ntsd.exe，恢复目标进程
    //      name      目标进程映像名（含 .exe 后缀），默认 "Student.exe"
    //      ntsdPath  ntsd.exe 路径，默认 ".\\ntsd.exe"（即工作目录下）
    // ---------------------------------------------------------------------------

    // 查找指定映像名对应的进程 PID，找不到返回 -1
    //   实现：调用 tasklist | findstr 把结果写入临时文件再读取第 2 列（PID）
    int FindPID(std::string name){
        std::string tmpFile = "ntsd_pid.tmp";
        std::string cmd = "tasklist | findstr /i \"" + name + "\" > \"" + tmpFile + "\" 2>nul";
        system(cmd.c_str());

        std::ifstream fin(tmpFile.c_str());
        std::string imgName, pidStr;
        // tasklist 输出格式：映像名  PID  会话名  会话#  内存
        // ifstream 的 >> 自动按空白分割，取前两列
        fin >> imgName >> pidStr;
        fin.close();
        remove(tmpFile.c_str());

        if(pidStr.empty()) return -1;
        return atoi(pidStr.c_str());
    }

    int NTSD_Stop(bool on, std::string name = "Student.exe",std::string ntsdPath = ".\\ntsd.exe"){
        if(on){
            // 1. 已有 ntsd 在运行则不重复附加（避免多实例互相干扰）
            if(FindPID("ntsd.exe") > 0) return 0;

            // 2. 检查 ntsd.exe 是否存在
            std::ifstream ntsdCheck(ntsdPath.c_str());
            if(!ntsdCheck.good()) return -2;   //ntsd 缺失
            ntsdCheck.close();

            // 3. 查找目标进程 PID
            int pid = FindPID(name);
            if(pid <= 0) return -1;            //目标进程未运行

            // 4. 在新窗口启动 ntsd 附加到目标进程（附加即冻结）
            //    使用 start 命令在新控制台窗口启动，便于用户查看 / 手动 q 退出
            std::string cmd = "start \"ntsd - " + name + "\" \""
                            + ntsdPath + "\" -p " + std::to_string(pid);
            return system(cmd.c_str());
        } else {
            // 结束所有 ntsd 进程；ntsd 退出时调试器 detach，目标进程恢复运行
            // 注：force-kill 路径下被调试进程可能被一同终止（见上方注释③）
            return system("taskkill /IM ntsd.exe /F >nul 2>&1");
        }
    }

    int Wait_NTSD_Stop(bool on, std::string name = "Student.exe",std::string ntsdPath = ".\\ntsd.exe"){
        Sleep(20000);
        while(NTSD_Stop(on, name ,ntsdPath)==-1){
            std::cout<<"尝试控制中...（20秒一次）\n";
            system("sleep 20");
        }
        std::cout<<"控制成功！解冻关闭此程序再重新启动客户端即可\n";
        return 0;
    }

    // ---------------------------------------------------------------------------
    //  自测
    // ---------------------------------------------------------------------------

    int test(){
        trainer::SetPath("C:\\Program Files (x86)\\Lenovo teaching system");
        trainer::Wait_NTSD_Stop(true);
        return 0;
    }
}
