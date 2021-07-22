//
// Created by 蒋澳然 on 2021/7/22.
// email: 875691208@qq.com
// $desc
//

#include "ErrorMap.h"

ErrorMap *ErrorMap::instance_ = nullptr;
std::mutex ErrorMap::mutex_{};

ErrorMap::ErrorMap() {
    error_map_.insert(std::make_pair(EPERM, "EPERM: Operation not permitted"));
    error_map_.insert(std::make_pair(EPERM, "EPERM: Operation not permitted"));
    error_map_.insert(std::make_pair(ENOENT, "ENOENT: No such file or directory"));
    error_map_.insert(std::make_pair(ESRCH, "ESRCH: No suchprocess"));
    error_map_.insert(std::make_pair(EINTR, "EINTR: Interrupted system call"));
    error_map_.insert(std::make_pair(EIO, "EIO: I/O error"));
    error_map_.insert(std::make_pair(ENXIO, "ENXIO: No such device or address"));
    error_map_.insert(std::make_pair(E2BIG, "E2BIG: Argument list too long"));
    error_map_.insert(std::make_pair(ENOEXEC, "ENOEXEC: Exec format error"));
    error_map_.insert(std::make_pair(EBADF, "EBADF: Bad file number"));
    error_map_.insert(std::make_pair(ECHILD, "ECHILD: No child processes"));
    error_map_.insert(std::make_pair(EAGAIN, "EAGAIN: Try again"));
    error_map_.insert(std::make_pair(ENOMEM, "ENOMEM: Out of memory"));
    error_map_.insert(std::make_pair(EACCES, "EACCES: Permission denied"));
    error_map_.insert(std::make_pair(EFAULT, "EFAULT: Bad address"));
    error_map_.insert(std::make_pair(ENOTBLK, "ENOTBLK: Block device required"));
    error_map_.insert(std::make_pair(EBUSY, "EBUSY: Device or resource busy"));
    error_map_.insert(std::make_pair(EEXIST, "EEXIST: File exists"));
    error_map_.insert(std::make_pair(EXDEV, "EXDEV: Cross-device link"));
    error_map_.insert(std::make_pair(ENODEV, "ENODEV: No such device"));
    error_map_.insert(std::make_pair(ENOTDIR, "ENOTDIR: Not a directory"));
    error_map_.insert(std::make_pair(EISDIR, "EISDIR: Is a directory"));
    error_map_.insert(std::make_pair(EINVAL, "EINVAL: Invalid argument"));
    error_map_.insert(std::make_pair(ENFILE, "ENFILE: File table overflow"));
    error_map_.insert(std::make_pair(EMFILE, "EMFILE: Too many open files"));
    error_map_.insert(std::make_pair(ENOTTY, "ENOTTY: Not a typewriter"));
    error_map_.insert(std::make_pair(ETXTBSY, "ETXTBSY: Text file busy"));
    error_map_.insert(std::make_pair(EFBIG, "EFBIG: File too large"));
    error_map_.insert(std::make_pair(ENOSPC, "ENOSPC: No space left on device"));
    error_map_.insert(std::make_pair(ESPIPE, "ESPIPE: Illegal seek"));
    error_map_.insert(std::make_pair(EROFS, "EROFS: Read-only file system"));
    error_map_.insert(std::make_pair(EMLINK, "EMLINK: Too many links"));
    error_map_.insert(std::make_pair(EPIPE, "EPIPE: Broken pipe"));
    error_map_.insert(std::make_pair(EDOM, "EDOM: Math argument out of domain of func"));
    error_map_.insert(std::make_pair(ERANGE, "ERANGE: Math result not representable"));
}

