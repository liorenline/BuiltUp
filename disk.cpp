#include "disk.h"

#include <fstream>
#include <string>
#include <set>
#include <sys/statvfs.h>

static bool isRealFs(const std::string& fstype) {
    if (fstype == "ext4")    return true;
    if (fstype == "ext3")    return true;
    if (fstype == "ext2")    return true;
    if (fstype == "xfs")     return true;
    if (fstype == "btrfs")   return true;
    if (fstype == "vfat")    return true;
    if (fstype == "ntfs")    return true;
    if (fstype == "fuseblk") return true;
    if (fstype == "overlay") return true;
    return false;
}

std::vector<DiskInfo> DiskReader::read() {
    std::ifstream file("/proc/mounts");

    std::vector<DiskInfo> result;
    std::set<std::string> seen;

    std::string device, path, fstype, options;
    int dump, pass;

    while (file >> device >> path >> fstype >> options >> dump >> pass) {
        if (!isRealFs(fstype)) continue;
        if (seen.count(path))  continue;
        seen.insert(path);

        struct statvfs st;
        if (statvfs(path.c_str(), &st) != 0) continue;

        DiskInfo d;
        d.device = device;
        d.path = path;
        d.fstype = fstype;

        uint64_t block = st.f_frsize;
        d.total_bytes = block * st.f_blocks;
        d.free_bytes = block * st.f_bfree;
        d.used_bytes = d.total_bytes - d.free_bytes;

        if (d.total_bytes > 0) {
            d.percent = 100.0 * d.used_bytes / d.total_bytes;
        }

        result.push_back(d);
    }

    return result;
}