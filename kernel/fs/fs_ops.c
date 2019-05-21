/* This file use for NCTU OSDI course */
/* It's contants fat file system operators */

#include <inc/stdio.h>
#include <fs.h>
#include <fat/ff.h>
#include <diskio.h>
#include <inc/assert.h>

extern struct fs_dev fat_fs;

int fresult_to_posix(int fresult) {
    int ret;
    switch (fresult) {
    case FR_OK:
        ret = STATUS_OK;
        break;
    case FR_NO_FILE:
        ret = STATUS_ENOENT;
        break;
    case FR_EXIST:
        ret = STATUS_EEXIST;
        break;
    }
    return ret;
}

void updateinfo(struct fs_fd *file) {
    file->pos = f_tell((FIL *)file->data);
    file->size = f_size((FIL *)file->data);
}

/*  Lab7, fat level file operator.
 *       Implement below functions to support basic file system operators by using the elmfat's API(f_xxx).
 *       Reference: http://elm-chan.org/fsw/ff/00index_e.html (or under doc directory (doc/00index_e.html))
 *
 *  Call flow example:
 *        ┌──────────────┐
 *        │     open     │
 *        └──────────────┘
 *               ↓
 *        ┌──────────────┐
 *        │   sys_open   │  file I/O system call interface
 *        └──────────────┘
 *               ↓
 *        ┌──────────────┐
 *        │  file_open   │  VFS level file API
 *        └──────────────┘
 *               ↓
 *        ╔══════════════╗
 *   ==>  ║   fat_open   ║  fat level file operator
 *        ╚══════════════╝
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

/* Note: 1. Get FATFS object from fs->data
*        2. Check fs->path parameter then call f_mount.
*/
int fat_mount(struct fs_dev *fs, const void* data)
{
    return f_mount(fs->data, fs->path, 1);
}

/* Note: Just call f_mkfs at root path '/' */
int fat_mkfs(const char* device_name)
{
    return f_mkfs("/", 0, 0);
}

/* Note: Convert the POSIX's open flag to elmfat's flag.
*        Example: if file->flags == O_RDONLY then open_mode = FA_READ
*                 if file->flags & O_APPEND then f_seek the file to end after f_open
*/
int fat_open(struct fs_fd* file)
{
    BYTE mode = 0;
    if (file->flags == O_RDONLY) {
        mode |= FA_READ;
    }
    if (file->flags & O_WRONLY) {
        mode |= FA_WRITE;
    }
    if (file->flags & O_RDWR) {
        mode |= FA_READ | FA_WRITE;
    }
    if (file->flags & O_CREAT) {
        mode |= FA_CREATE_NEW;
    }
    if (file->flags & O_APPEND) {
        mode |= FA_OPEN_ALWAYS;
    }
    if (file->flags & O_TRUNC) {
        mode &= ~FA_CREATE_NEW;
        mode |= FA_CREATE_ALWAYS;
    }
    if (file->flags & (O_EXCL | O_DIRECTORY)) {
        panic("Not implemented");
    }

    int fret = f_open((FIL *)file->data, file->path, mode);
    if (fret == RES_OK && file->flags & O_APPEND) {
        fret = f_lseek((FIL *)file->data, f_size((FIL *)file->data));
    }

    if (fret == RES_OK) {
        updateinfo(file);
    }

    return -fresult_to_posix(fret);
}

int fat_close(struct fs_fd* file)
{
    return f_close((FIL *)file->data);
}
int fat_read(struct fs_fd* file, void* buf, size_t count)
{
    int ret;
    int fret = f_read((FIL *)file->data, buf, count, &ret);
    if (fret > 0) return -fresult_to_posix(fret);

    updateinfo(file);
    return ret;
}
int fat_write(struct fs_fd* file, const void* buf, size_t count)
{
    int ret;
    int fret = f_write((FIL *)file->data, buf, count, &ret);
    if (fret > 0) return -fresult_to_posix(fret);

    updateinfo(file);
    return ret;
}
int fat_lseek(struct fs_fd* file, off_t offset)
{
    int fret = f_lseek((FIL *)file->data, offset);
    updateinfo(file);
    return -fresult_to_posix(fret);
}
int fat_unlink(struct fs_fd* file, const char *pathname)
{
    int fret = f_unlink(pathname);
    return -fresult_to_posix(fret);
}
int fat_readdir(DIR *dp, FILINFO *fno) {
    int fret = f_readdir(dp, fno);
    return -fresult_to_posix(fret);
}
int fat_opendir(DIR *dp, const char *pathname) {
    int fret = f_opendir(dp, pathname);
    return -fresult_to_posix(fret);
}
int fat_closedir(DIR *dp) {
    int fret = f_closedir(dp);
    return -fresult_to_posix(fret);
}

struct fs_ops elmfat_ops = {
    .dev_name = "elmfat",
    .mount = fat_mount,
    .mkfs = fat_mkfs,
    .open = fat_open,
    .close = fat_close,
    .read = fat_read,
    .write = fat_write,
    .lseek = fat_lseek,
    .unlink = fat_unlink,
    .readdir = fat_readdir,
    .opendir = fat_opendir,
    .closedir = fat_closedir
};



