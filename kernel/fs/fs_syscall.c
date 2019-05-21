/* This file use for NCTU OSDI course */


// It's handel the file system APIs 
#include <inc/stdio.h>
#include <inc/syscall.h>
#include <fs.h>
#include <fat/ff.h>

/*Lab7, file I/O system call interface.*/
/*Note: Here you need handle the file system call from user.
 *       1. When user open a new file, you can use the fd_new() to alloc a file object(struct fs_fd)
 *       2. When user R/W or seek the file, use the fd_get() to get file object.
 *       3. After get file object call file_* functions into VFS level
 *       4. Update the file objet's position or size when user R/W or seek the file.(You can find the useful marco in ff.h)
 *       5. Remember to use fd_put() to put file object back after user R/W, seek or close the file.
 *       6. Handle the error code, for example, if user call open() but no fd slot can be use, sys_open should return -STATUS_ENOSPC.
 *
 *  Call flow example:
 *        ┌──────────────┐
 *        │     open     │
 *        └──────────────┘
 *               ↓
 *        ╔══════════════╗
 *   ==>  ║   sys_open   ║  file I/O system call interface
 *        ╚══════════════╝
 *               ↓
 *        ┌──────────────┐
 *        │  file_open   │  VFS level file API
 *        └──────────────┘
 *               ↓
 *        ┌──────────────┐
 *        │   fat_open   │  fat level file operator
 *        └──────────────┘
 *               ↓
 *        ┌──────────────┐
 *        │    f_open    │  FAT File System Module
 *        └──────────────┘
 *               ↓
 *        ┌──────────────┐
 *        │    diskio    │  low level file operator
 *        └──────────────┘
 *               ↓
 *        ┌──────────────┐
 *        │     disk     │  simple ATA disk dirver
 *        └──────────────┘
 */

// Below is POSIX like I/O system call 
int sys_open(const char *file, int flags, int mode)
{
    //We dont care the mode.
    int fd = fd_new();
    if (fd < 0) return -STATUS_ENOSPC;

    extern struct fs_fd fd_table[];
    int ret = file_open(&(fd_table[fd]), file, flags);
    if (ret < 0) {
        fd_put(&(fd_table[fd]));
        return ret;
    }

    return fd;
}

int sys_close(int fd)
{
    if (fd >= FS_FD_MAX) return -STATUS_EINVAL;

    extern struct fs_fd fd_table[];
    int ret = file_close(&(fd_table[fd]));
    fd_put(&(fd_table[fd]));
    return ret;
}
int sys_read(int fd, void *buf, size_t len)
{
    struct fs_fd* file = fd_get(fd);
    if (file == NULL) return -STATUS_EBADF;

    int ret = file_read(file, buf, len);
    fd_put(file);
    return ret;
}
int sys_write(int fd, const void *buf, size_t len)
{
    if (len > 2147483647) return -STATUS_EINVAL;

    struct fs_fd* file = fd_get(fd);
    if (file == NULL) return -STATUS_EBADF;

    int ret = file_write(file, buf, len);
    fd_put(file);
    return ret;
}

/* Note: Check the whence parameter and calcuate the new offset value before do file_lseek() */
off_t sys_lseek(int fd, off_t offset, int whence)
{
    struct fs_fd* file = fd_get(fd);
    if (file == NULL) return -STATUS_EBADF;

    switch (whence) {
    case SEEK_SET:
        offset = 0;
        break;
    case SEEK_CUR:
        offset += file->pos;
        break;
    case SEEK_END:
        offset += file->size;
        break;
    }

    file_lseek(file, offset);
    fd_put(file);
    return offset;
}

int sys_unlink(const char *pathname)
{
    return file_unlink(pathname);
}

int sys_readdir(DIR *dp, FILINFO *fno)
{
    return file_readdir(dp, fno);
}

int sys_opendir(DIR *dp, const char *pathname)
{
    return file_opendir(dp, pathname);
}

int sys_closedir(DIR *dp)
{
    return file_closedir(dp);
}

int sys_stat(const char *pathname, FILINFO *fno) {
    return file_stat(pathname, fno);
}


              

