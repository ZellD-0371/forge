#include "forge/fs.h"
#include "forge/log.h"
#include "forge/mem.h"
#include "forge/proc.h"
#include "forge/singleton.h"

#include <nn/account.h>
#include <nn/fs.h>
#include <nn/os.h>

constexpr size_t kMountSaveDataOffset = 0x878DA0;
constexpr size_t kJobSafeOffset = 0x212041C;
constexpr auto kSaveDataMountPoint = "savedata";
constexpr auto kMhxxSaveDataMountPoint = "forge_savedata";
constexpr auto kRomFsMountPoint = "app";

namespace {
bool s_mhxxSaveDataMounted = false;

bool isJobSafe()
{
    return *(bool*)(g_mainTextAddr + kJobSafeOffset);
}

bool isThreadSafe(void* saveData)
{
    return *(bool*)((char*)saveData + 0x18);
}

nn::os::MutexType* getMutex(void* saveData)
{
    return (nn::os::MutexType*)((char*)saveData + 0x4);
}

int& mountCount(void* saveData)
{
    return *(int*)((char*)saveData + 0xF1C);
}

bool mountMhxxSaveData()
{
    if (s_mhxxSaveDataMounted) {
        return true;
    }

    nn::account::Uid userId {};
    const auto userResult = nn::account::GetLastOpenedUser(&userId);
    if (userResult.IsFailure() || !userId.IsValid()) {
        forge_log_error(
            "[forge] MHXX save: GetLastOpenedUser failed (result=0x%08X, valid=%d)",
            static_cast<unsigned int>(userResult.GetInnerValueForDebug()), userId.IsValid());
        return false;
    }

    const auto mountResult = nn::fs::MountSaveData(kMhxxSaveDataMountPoint, userId);
    if (mountResult.IsFailure()) {
        forge_log_error("[forge] MHXX save: MountSaveData failed (result=0x%08X)",
            static_cast<unsigned int>(mountResult.GetInnerValueForDebug()));
        return false;
    }

    s_mhxxSaveDataMounted = true;
    return true;
}

}

extern "C" bool forge_fs_mountSaveData(void)
{
    if (forge_proc_isMhxxJpn()) {
        return mountMhxxSaveData();
    }

    const auto mount = (int (*)(void*))(g_mainTextAddr + kMountSaveDataOffset);
    const auto saveData = forge_singleton_getInstanceByName("sSavedata");
    if (saveData == nullptr) {
        forge_log_error("[forge] MHGU save: sSavedata singleton was not resolved");
        return false;
    }

    const int result = mount(saveData);
    if (result != 0) {
        forge_log_error("[forge] MHGU save: game mount function failed (result=%d)", result);
        return false;
    }

    return true;
}

extern "C" void forge_fs_unmountSaveData(void)
{
    if (forge_proc_isMhxxJpn()) {
        return;
    }

    const bool jobSafe = isJobSafe();
    const auto saveData = forge_singleton_getInstanceByName("sSavedata");
    if (saveData == nullptr) {
        return;
    }

    auto& mounts = mountCount(saveData);

    // Wasn't mounted in the first place
    if (mounts == 0) {
        return;
    }

    nn::fs::CommitSaveData(kSaveDataMountPoint);

    const bool threadSafe = jobSafe || isThreadSafe(saveData);
    const auto mutex = getMutex(saveData);

    if (threadSafe) {
        nn::os::LockMutex(mutex);
    }

    mounts -= 1;
    if (mounts == 0) {
        nn::fs::Unmount(kSaveDataMountPoint);
    }

    if (threadSafe) {
        nn::os::UnlockMutex(mutex);
    }
}

extern "C" const char* forge_fs_getSaveDataMountPoint(void)
{
    return forge_proc_isMhxxJpn() ? kMhxxSaveDataMountPoint : kSaveDataMountPoint;
}

extern "C" const char* forge_fs_getRomFsMountPoint(void)
{
    return kRomFsMountPoint;
}

extern "C" bool forge_fs_fileExists(const char* path)
{
    nn::fs::FileHandle handle;
    if (nn::fs::OpenFile(&handle, path, nn::fs::OpenMode_Read).IsFailure()) {
        return false;
    }

    nn::fs::CloseFile(handle);
    return true;
}
