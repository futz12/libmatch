#include <opencv2/opencv.hpp>
#include "library.h"

const char* version = "0.9.5";

LIBMATCH_API const char* libmatch_version()
{
    // cv::getBuildInformation();
    return version;
}

const char readme[] = "-------------------------------------LibMatch------------------------------------\n"
                      "                                     免责声明\n"
                      "一. 本软件完全出于个人兴趣爱好，由本人在业余时间开发.是一款安全，绿色，可靠，免费的软件产品.\n"
                      "二. 利用本软件所做出的任何软件作品，和本人无关.\n"
                      "三. 本软件绝对不针对任何一款游戏或者游戏开发商，如果有人利用本软件开发非法游戏辅助，本人将保留追究其民事以及刑事责任的权利.\n"
                      "四. 因使用本软件而引致的任何意外、疏忽、合约毁坏、诽谤、版权或知识产权侵犯及其所造成的任何损失，本人概不负责，亦概不承担任何民事或刑事法律责任。\n"
                      "五. 本软件的功能描述以易语言这款软件的语法来描述，但与易语言官方无任何关系.\n"
                      "六. 当你第一次开始使用本人所提供的任何软件及资源的那一刻起就将被视为对本声明全部内容的认可。同时您必须认可上述免责条款，方可使用本软件及资源。如有任何异议，建议立刻删除本软件及资源并且停止使用.\n"
                      "七. 以上内容，本人保留最终解释权。";




#ifdef _WIN32
#include "bmpWnd.h"
#include <thread>

BOOL WINAPI DllMain(
        HINSTANCE hinstDLL,  // handle to DLL module
        DWORD fdwReason,     // reason for calling function
        LPVOID lpvReserved )  // reserved
{
    // Perform actions based on the reason for calling.
    switch( fdwReason )
    {
        case DLL_PROCESS_ATTACH:
            // Initialize once for each new process.
            // Return FALSE to fail DLL load.
            std::thread([] {
                TextWindow textWnd(readme, 10);
                textWnd.show();
                }).detach();
            break;

        case DLL_THREAD_ATTACH:
            // Do thread-specific initialization.
            break;

        case DLL_THREAD_DETACH:
            // Do thread-specific cleanup.
            break;

        case DLL_PROCESS_DETACH:

            if (lpvReserved != nullptr)
            {
                break; // do not do cleanup if process termination scenario
            }

            // Perform any necessary cleanup.
            break;
    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}

#endif