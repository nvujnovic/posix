#pragma once

//          Copyright Nebojsa Vujnovic 2018 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "config.h"
#include "assert.h"
#include "unique_handle.h"
#include "system_error.h"
#include "file.h"
#include "concatenate.h"
#include "util.h"

#include <stddef.h>
#include <stdlib.h>

#include <system_error>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <utime.h>
#include <dirent.h>
#include <ftw.h>
#include <libgen.h>
#include <pwd.h>
#include <grp.h>

#include <chrono>
#include <ctime>

#ifndef CP_REMOVE_DEPRECATED
#define CP_REMOVE_DEPRECATED 1
#endif


// TODO( nebojsa ) find the way to fix non nul terminating functions like readlink , readlinkat

namespace cp {

struct file_descriptor_traits 
{
  constexpr static int  invalid(void) noexcept { return -1; }
  static void close(int fd) noexcept { 
    CP_ASSERT_MSG(fd != invalid(), "must be a valid file descriptor");
    ::close(fd); 
  }
};

using file_descriptor = ::cp::unique_handle<int, ::cp::file_descriptor_traits >;

inline std::string to_string(::cp::file_descriptor const& fd)
{
  return fd ? ::cp::to_string(fd.get()): "invalid";
}

CP_DEFINE_SERIALIZATION_SPECIALIZATION(::cp::file_descriptor);


CP_FORCE_INLINE 
::cp::file_descriptor 
open(char const* pathname, int flags, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(pathname != nullptr);

  ::cp::file_descriptor result (::open(pathname, flags));
  if (CP_UNLIKELY(!result))
  {
    ec = ::cp::make_system_error_code();
  }
  return result;
}


CP_FORCE_INLINE 
::cp::file_descriptor 
open(char const* pathname, int flags)
{
  std::error_code ec;
  ::cp::file_descriptor result = ::cp::open(pathname, flags, ec);
  if (CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("error openng file: ", pathname, ", with flags: ", flags));
  }
  return result;
}


CP_FORCE_INLINE 
::cp::file_descriptor 
open(char const * pathname, int flags, ::mode_t mode, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(pathname);

  ::cp::file_descriptor result(
    ::open(pathname, flags, mode)
  );
  if (CP_UNLIKELY(!result) )
  {
    ec = ::cp::make_system_error_code();
  }
  return result;
}


CP_FORCE_INLINE 
::cp::file_descriptor 
open(char const*pathname, int flags, ::mode_t mode) 
{
  std::error_code ec;
  ::cp::file_descriptor result = ::cp::open(pathname, flags, mode, ec);
  if (CP_UNLIKELY( ec ))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec,::cp::concat("error opening file: [", pathname, "], flags: [", flags, "], mode [", mode, "]"));
  }
  return result;
}

CP_FORCE_INLINE 
::cp::file_descriptor 
creat(const char* pathname, ::mode_t mode, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(pathname);

  ::cp::file_descriptor result(
    ::creat(pathname, mode)
  );
  if (CP_UNLIKELY( !result ))
  {
    ec = cp::make_system_error_code();
  }
  return result;
}

CP_FORCE_INLINE
std::error_code access(const char* pathname, int mode) noexcept
{
  CP_ASSERT(pathname);
  int result = ::access(pathname, mode);
  if (!result) result = errno;
  return ::cp::make_system_error_code(result);
}

CP_FORCE_INLINE 
::cp::file_descriptor 
creat(const char* pathname, ::mode_t mode)
{
  std::error_code ec;
  ::cp::file_descriptor result = cp::creat(pathname, mode, ec);
  if ( CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("error opening file: [", pathname, "], mode: [", mode, "]"));
  }
  return result;
}

CP_FORCE_INLINE std::size_t 
read(::cp::file_descriptor const& fd, void* buffer, std::size_t nbytes, std::error_code& ec) noexcept 
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT_MSG(fd, "invalid file descriptor");

  const int result =  ::read(fd, buffer, nbytes);
  if (CP_UNLIKELY(-1 == result ))
  {
    ec = cp::make_system_error_code();
  }
  return result;
}

CP_FORCE_INLINE std::size_t 
read(::cp::file_descriptor const& fd, void* buffer, std::size_t bytes_count)
{
  std::error_code ec;
  const int result =  ::cp::read(fd, buffer, bytes_count, ec);
  if (CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("error reading file, fd: [", fd, "], bytes count: [", bytes_count, "]"));
  }
  return result;
}

CP_FORCE_INLINE std::size_t 
write(::cp::file_descriptor const& fd, void* buffer, ::std::size_t bytes_count, ::std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(fd);

  const int result = ::write(fd, buffer, bytes_count);
  if (CP_UNLIKELY(-1 == result))
  {
    ec = cp::make_system_error_code();
  }
  return result;
}

CP_FORCE_INLINE std::size_t 
write(::cp::file_descriptor const& fd, void* buffer, ::std::size_t bytes_count)
{
  std::error_code ec;
  std::size_t result = cp::write(fd, buffer, bytes_count, ec);
  if (CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("error writing to file, fd: [", fd, "] bytes count: [", bytes_count, "]"));
  }
  return result;
}

CP_FORCE_INLINE
off_t lseek( cp::file_descriptor const& fd, ::off_t offset, int whence, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(fd);

  const off_t result = ::lseek(fd.get(), offset, whence);
  if (CP_UNLIKELY( -1 == result)) ec = cp::make_system_error_code();
  return result;
}

CP_FORCE_INLINE
off_t 
lseek(cp::file_descriptor const& fd, ::off_t offset, int whence)
{
  std::error_code ec;
  const off_t result = cp::lseek(fd, offset, whence);
  if ( CP_UNLIKELY(ec )) {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("error seeking into file, fd: [", fd,"] offset: [", (long long) offset, "] whence: [", whence,"]" ));
  }
  return result;
}

CP_FORCE_INLINE
cp::file_descriptor dup(int fd, ::std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(fd != -1);

  const int result = ::dup(fd);
  if (CP_UNLIKELY( -1 == result)) ec = ::cp::make_system_error_code();
  return cp::file_descriptor(result);
}

CP_FORCE_INLINE
cp::file_descriptor dup(int fd)
{
  std::error_code ec;
  cp::file_descriptor result = cp::dup(fd, ec);
  if (CP_UNLIKELY( ec ))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("error duplicating fd, fd: [", fd,"]"));
  }
  return result;
}

CP_FORCE_INLINE
cp::file_descriptor dup(::cp::file_descriptor fd, ::std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(fd);

  return ::cp::dup(fd.get(), ec);
}

CP_FORCE_INLINE
cp::file_descriptor dup(cp::file_descriptor fd)
{
  CP_ASSERT(fd);
  return ::cp::dup(fd.get());
}

#if (_XOPEN_SOURCE >= 500 ||  _POSIX_C_SOURCE >= 200809L)

CP_FORCE_INLINE
ssize_t pread(cp::file_descriptor const& fd, void* buf, std::size_t nbytes, ::off_t offset, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(buf);
  CP_ASSERT(offset >= 0);

  const ssize_t result = ::pread(fd, buf, nbytes, offset);
  if(CP_UNLIKELY( -1==result)) ec = ::cp::make_system_error_code();
  return result;
}

CP_FORCE_INLINE
ssize_t pread(cp::file_descriptor const& fd, void* buf, size_t nbytes, off_t offset)
{
  std::error_code ec;
  const ssize_t result = ::cp::pread(fd, buf, nbytes,offset, ec);
  if(CP_UNLIKELY( ec ))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("pread fd, fd: [", fd,"], nbytes [", nbytes, "], offset: [", offset, "]"));
  }
  return result;
}

CP_FORCE_INLINE
::ssize_t pwrite(::cp::file_descriptor const& fd, const void* buf, std::size_t nbytes, ::off_t offset, std::error_code& ec ) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(fd);
  CP_ASSERT(offset >= 0);

  const ssize_t result = ::pwrite(fd, buf, nbytes, offset);
  if (CP_UNLIKELY( -1 == result)) ec = cp::make_system_error_code();
  return result;
}

CP_FORCE_INLINE
::ssize_t pwrite(::cp::file_descriptor const& fd, const void* buf, std::size_t nbytes, ::off_t offset)
{
  std::error_code ec; 
  const ssize_t result = ::cp::pwrite(fd, buf, nbytes, offset , ec);
  if (CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("pwrite fd, fd: [", fd,"], nbytes [", nbytes, "], offset: [", offset, "]"));
  }
  return result;
}
#endif

CP_FORCE_INLINE
::ssize_t readv(::cp::file_descriptor const& fd, const iovec* iov, int iovcnt, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(fd);
  const ssize_t result = ::readv(fd.get(), iov, iovcnt);
  if (CP_UNLIKELY( -1 == result)) ec = cp::make_system_error_code();
  return result;
}

CP_FORCE_INLINE
::ssize_t readv(::cp::file_descriptor const& fd, const ::iovec* iov, int iovcnt)
{
  std::error_code ec;
  const ssize_t result = ::cp::readv(fd, iov, iovcnt, ec);
  if (CP_UNLIKELY( ec ))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("readv fd, fd: [", fd,"], iovcnt [", iovcnt, "]"));
  }
  return result;
}

CP_FORCE_INLINE
::ssize_t writev(::cp::file_descriptor const& fd, const ::iovec* iov, int iovcnt, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(fd);

  const ::ssize_t result = ::writev(fd, iov, iovcnt);
  if (CP_UNLIKELY( -1 == result )) ec = ::cp::make_system_error_code();
  return result;
}

CP_FORCE_INLINE
::ssize_t writev(::cp::file_descriptor const& fd, const ::iovec* iov, int iovcnt)
{
  std::error_code ec; 
  const ::ssize_t result = ::cp::writev(fd, iov, iovcnt, ec);
  if (CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("writev fd, fd: [", fd,"], iovcnt [", iovcnt, "]"));
  }
  return result;
}

#if defined _DEFAULT_SOURCE
CP_FORCE_INLINE
::ssize_t preadv(::cp::file_descriptor const& fd, const ::iovec* iov, int iovcnt, ::off_t offset, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(fd);
  CP_ASSERT(offset >= 0);

  const ::ssize_t result = ::preadv(fd, iov, iovcnt, offset);
  if (CP_UNLIKELY( -1 == result)) ec = ::cp::make_system_error_code();
  return result;
}

CP_FORCE_INLINE
::ssize_t preadv( ::cp::file_descriptor const& fd, const ::iovec* iov, int iovcnt, ::off_t offset)
{
  std::error_code ec;
  const ::ssize_t result = ::cp::preadv(fd, iov, iovcnt, offset, ec);
  if (CP_UNLIKELY( ec ))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("preadv fd, fd: [", fd,"], iovcnt [", iovcnt, "], offset: [", offset, "]"));
  }
  return result;
}

CP_FORCE_INLINE
::ssize_t pwritev( ::cp::file_descriptor const& fd, const ::iovec* iov, int iovcnt, ::off_t offset, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(fd);
  CP_ASSERT(offset >= 0);

  const ::ssize_t result = ::pwritev(fd, iov, iovcnt, offset);
  if(CP_UNLIKELY( -1 == result)) ec = ::cp::make_system_error_code();
  return result;
}

CP_FORCE_INLINE
::ssize_t pwritev( ::cp::file_descriptor const& fd, const ::iovec* iov, int iovcnt, ::off_t offset)
{
  std::error_code ec;
  const ::size_t result = cp::pwritev(fd, iov, iovcnt, offset,ec);
  if (CP_UNLIKELY( ec ))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("pwritev fd, fd: [", fd,"], iovcnt [", iovcnt, "], offset: [", offset, "]"));
  }
  return result;
}
#endif

#if ( _XOPEN_SOURCE >= 00 || _POSIX_C_SOURCE >= 200809L || _BSD_SOURCE)

CP_FORCE_INLINE
void truncate(const char* pathname, ::off_t length, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(pathname);
  CP_ASSERT(length >= 0);

  const int status = ::truncate(pathname, length);
  if (CP_UNLIKELY( -1 == status)) ec = ::cp::make_system_error_code();
}

CP_FORCE_INLINE
void truncate(const char* pathname, ::off_t length)
{
  std::error_code ec;
  ::cp::truncate(pathname, length, ec);
  if(CP_UNLIKELY( ec ))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("truncate path: [", pathname,"], length: [", length, "]"));
  }
}
#endif

#if ( _XOPEN_SOURCE >= 500 || _POSIX_C_SOURCE >= 200112L || _BSD_SOURCE)
CP_FORCE_INLINE
void ftruncate(::cp::file_descriptor const& fd, ::off_t length, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(fd);
  CP_ASSERT(length >= 0);

  const int status = ::ftruncate(fd, length);
  if (CP_UNLIKELY( -1 == status)) ec = ::cp::make_system_error_code();
}

CP_FORCE_INLINE
void ftruncate(::cp::file_descriptor const& fd, ::off_t length)
{
  std::error_code ec;
  ::cp::ftruncate(fd, length, ec);
  if (CP_UNLIKELY( ec ))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("ftruncate fd: [", fd,"], length: [", length, "]"));
  }
}
#endif

CP_FORCE_INLINE
::cp::file_descriptor mkstemp(char * in_template_out_filename, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(in_template_out_filename);

  ::cp::file_descriptor result( ::mkstemp(in_template_out_filename));
  if (CP_UNLIKELY(!result)) ec = ::cp::make_system_error_code();
  return result;
}

CP_FORCE_INLINE
::cp::file_descriptor mkstemp(char* in_template_out_filename)
{
  std::error_code ec;
  if(CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("mkstemp template [", in_template_out_filename,"]"));
  }
}

// TODO (nebojsa) what is a best way to provide type safe wrapper for functions with variadic arguments like fcntl


// file info structure is created to enable intelisence 
struct file_info : public ::stat
{
  // accessing fields
  ::dev_t           device()                  const noexcept { return st_dev;    }
  ::ino_t           inode_number()            const noexcept { return st_ino;    }
  ::mode_t          mode()                    const noexcept { return st_mode;   }
  ::nlink_t         number_of_hard_links()    const noexcept { return st_nlink;  }
  ::uid_t           owners_user_id()          const noexcept { return st_uid;    }
  ::gid_t           owners_group_id()         const noexcept { return st_gid;    }
  ::dev_t           rdevice()                 const noexcept { return st_rdev;   }
  ::off_t           size()                    const noexcept { return st_size;   }
  ::blksize_t       filesystem_block_size()   const noexcept { return st_blksize;}
  ::blkcnt_t        number_of_512B_blocks()   const noexcept { return st_blocks; }
  ::timespec        last_access_time()        const noexcept { return st_atim;   }
  ::timespec        last_modification_time()  const noexcept { return st_mtim;   }
  ::timespec        last_status_change()      const noexcept { return st_ctim;   }
  
  bool is_block_device()     const noexcept { return (st_mode & S_IFMT) == S_IFBLK; }
  bool is_character_device() const noexcept { return (st_mode & S_IFMT) == S_IFCHR; }
  bool is_directory()        const noexcept { return (st_mode & S_IFMT) == S_IFDIR; }
  bool is_FIFO()             const noexcept { return (st_mode & S_IFMT) == S_IFIFO; }
  bool is_socket()           const noexcept { return (st_mode & S_IFMT) == S_IFSOCK;}
  bool is_regular_file()     const noexcept { return (st_mode & S_IFMT) == S_IFREG; }
  bool is_symbolic_link()    const noexcept { return (st_mode & S_IFMT) == S_IFLNK; }
};

// TODO (nebojsa) add to_string method for file_info

static_assert(sizeof(file_info) == sizeof(struct ::stat), "do not add members or virtual funcitons to fine_info struct");

CP_FORCE_INLINE
void stat(const char* pathname, file_info& fi, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(pathname);
  
  const int result = ::stat(pathname, &fi);
  if ( CP_UNLIKELY(-1 == result))  ec = ::cp::make_system_error_code();
}

CP_FORCE_INLINE
void stat(const char* pathname, file_info& fi) 
{
  std::error_code ec;
  ::cp::stat(pathname, fi,ec);
  if ( CP_UNLIKELY(ec)) 
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("stat path [",pathname,"]"));
  }
}

#if (_DEFAULT_SOURCE   || _XOPEN_SOURCE >= 500  || _POSIX_C_SOURCE >= 200112L)

CP_FORCE_INLINE
void lstat(const char* pathname, ::cp::file_info& statbuf, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(pathname);
  
  const int result = ::lstat(pathname, &statbuf);
  if ( CP_UNLIKELY(-1 == result))  ec = ::cp::make_system_error_code();
}

CP_FORCE_INLINE
void lstat(const char* pathname, ::cp::file_info& statbuf)
{
  std::error_code ec;
  ::cp::lstat(pathname, statbuf,ec);
  if ( CP_UNLIKELY(ec)) 
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("lstat path [",pathname,"]"));
  }
}
#endif

CP_FORCE_INLINE
bool is_regular_file(const char* pathname )
// this helper funciton follows symbolic links
{
  file_info info;
  ::cp::stat(pathname, info);
  return info.is_regular_file();
}

CP_FORCE_INLINE
bool is_regular_file(const char* pathname, std::error_code& ec) noexcept
// this helper funciton follows symbolic links
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  file_info info;
  ::cp::stat(pathname, info, ec);
  return info.is_regular_file();
}

CP_FORCE_INLINE
bool is_directory(const char* pathname)
// this helper funciton follows symbolic links
{
  file_info info;
  ::cp::stat(pathname, info);
  return info.is_directory();
}

CP_FORCE_INLINE
bool is_directory(const char* pathname, std::error_code& ec) noexcept
// this helper funciton follows symbolic links
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  file_info info;
  ::cp::stat(pathname, info, ec);
  return info.is_directory();
}

CP_FORCE_INLINE
bool is_socket(const char* pathname)
// this helper funciton follows symbolic links
{
  file_info info;
  ::cp::stat(pathname, info);
  return info.is_socket();
}

CP_FORCE_INLINE
bool is_socket(const char* pathname, std::error_code& ec) noexcept
// this helper funciton follows symbolic links
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  file_info info;
  ::cp::stat(pathname, info, ec);
  return info.is_socket();
}

CP_FORCE_INLINE
bool is_FIFO(const char* pathname)
// this helper funciton follows symbolic links
{
  file_info info;
  ::cp::stat(pathname, info);
  return info.is_FIFO();
}

CP_FORCE_INLINE
bool is_FIFO(const char* pathname, std::error_code& ec) noexcept
// this helper funciton follows symbolic links
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  file_info info;
  ::cp::stat(pathname, info, ec);
  return info.is_FIFO();
}

CP_FORCE_INLINE
bool is_pipe(const char* pathname)
// this helper funciton follows symbolic links
{
  return is_FIFO(pathname);
}

CP_FORCE_INLINE
bool is_pipe(const char* pathname, std::error_code& ec) noexcept
// this helper funciton follows symbolic links
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  return is_FIFO(pathname, ec);
}

CP_FORCE_INLINE
bool is_character_device(const char* pathname)
// this helper funciton follows symbolic links
{
  file_info info;
  ::cp::stat(pathname, info);
  return info.is_character_device();
}

CP_FORCE_INLINE
bool is_character_device(const char* pathname, std::error_code& ec) noexcept
// this helper funciton follows symbolic links
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  file_info info;
  ::cp::stat(pathname, info, ec);
  return info.is_character_device();
}

CP_FORCE_INLINE
bool is_block_device(const char* pathname)
// this helper funciton follows symbolic links
{
  file_info info;
  ::cp::stat(pathname, info);
  return info.is_block_device();
}

CP_FORCE_INLINE
bool is_block_device(const char* pathname, std::error_code& ec) noexcept
// this helper funciton follows symbolic links
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  file_info info;
  ::cp::stat(pathname, info, ec);
  return info.is_block_device();
}

CP_FORCE_INLINE
bool is_symbolic_link(const char* pathname)
{
  file_info info;
  ::cp::lstat(pathname, info);
  return info.is_symbolic_link();
}

CP_FORCE_INLINE
bool is_symbolic_link(const char* pathname, std::error_code& ec) noexcept 
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  file_info info;
  ::cp::lstat(pathname, info, ec);
  return info.is_symbolic_link();
}

CP_FORCE_INLINE
void fstat(::cp::file_descriptor const& fd, ::cp::file_info& statbuf, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(fd);
  
  const int result = ::fstat(fd, &statbuf);
  if ( CP_UNLIKELY(-1 == result))  ec = ::cp::make_system_error_code();
}

CP_FORCE_INLINE
void fstat(::cp::file_descriptor const& fd, ::cp::file_info& statbuf)
{
  std::error_code ec;
  ::cp::fstat(fd, statbuf,ec);
  if ( CP_UNLIKELY(ec)) 
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("fstat fd [",fd,"]"));
  }
}

CP_FORCE_INLINE
bool is_regular_file(::cp::file_descriptor const& fd)
{
  file_info info;
  ::cp::fstat(fd, info);
  return info.is_regular_file();
}

CP_FORCE_INLINE
bool is_regular_file(::cp::file_descriptor const& fd, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  file_info info;
  ::cp::fstat(fd, info, ec);
  return info.is_regular_file();
}

CP_FORCE_INLINE
bool is_directory(::cp::file_descriptor const& fd)
{
  file_info info;
  ::cp::fstat(fd, info);
  return info.is_directory();
}

CP_FORCE_INLINE
bool is_directory(::cp::file_descriptor const& fd, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  file_info info;
  ::cp::fstat(fd, info, ec);
  return info.is_directory();
}

CP_FORCE_INLINE
bool is_socket(::cp::file_descriptor const& fd)
{
  file_info info;
  ::cp::fstat(fd, info);
  return info.is_socket();
}

CP_FORCE_INLINE
bool is_socket(::cp::file_descriptor const& fd, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  file_info info;
  ::cp::fstat(fd, info, ec);
  return info.is_socket();
}

CP_FORCE_INLINE
bool is_FIFO(::cp::file_descriptor const& fd)
{
  file_info info;
  ::cp::fstat(fd, info);
  return info.is_FIFO();
}

CP_FORCE_INLINE
bool is_FIFO(::cp::file_descriptor const& fd, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  file_info info;
  ::cp::fstat(fd, info, ec);
  return info.is_FIFO();
}

CP_FORCE_INLINE
bool is_pipe(::cp::file_descriptor const& fd)
{
  return is_FIFO(fd);
}

CP_FORCE_INLINE
bool is_pipe(::cp::file_descriptor const& fd, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  return is_FIFO(fd, ec);
}

CP_FORCE_INLINE
bool is_character_device(::cp::file_descriptor const& fd)
{
  file_info info;
  ::cp::fstat(fd, info);
  return info.is_character_device();
}

CP_FORCE_INLINE
bool is_character_device(::cp::file_descriptor const& fd, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  file_info info;
  ::cp::fstat(fd, info, ec);
  return info.is_character_device();
}

CP_FORCE_INLINE
bool is_block_device(::cp::file_descriptor const& fd)
{
  file_info info;
  ::cp::fstat(fd, info);
  return info.is_block_device();
}


CP_FORCE_INLINE
bool is_block_device(::cp::file_descriptor const& fd, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  file_info info;
  ::cp::fstat(fd, info, ec);
  return info.is_block_device();
}

#if (_DEFAULT_SOURCE || _XOPEN_SOURCE >= 500 || _POSIX_C_SOURCE >= 200112L)

CP_FORCE_INLINE
bool is_symbolic_link(::cp::file_descriptor const& fd)
// this helper function only work if file description is obtained with open call 
// that has O_NOFOLLOW flag or similar
{
  file_info info;
  ::cp::fstat(fd, info);
  return info.is_symbolic_link();
}

CP_FORCE_INLINE
bool is_symbolic_link(::cp::file_descriptor const& fd, std::error_code& ec) noexcept 
// this helper function only work if file description is obtained with open call 
// that has O_NOFOLLOW flag or similar
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  file_info info;
  ::cp::fstat(fd, info, ec);
  return info.is_symbolic_link();
}
#endif

struct utime_times : ::utimbuf
{
  ::time_t access_time( )       const noexcept { return actime;  }
  ::time_t modification_time( ) const noexcept { return modtime; }
  void access_time      (::time_t actime ) noexcept {  this->actime = actime;  }
  void modification_time(::time_t modtime) noexcept { this->modtime = modtime; }
};
static_assert(sizeof(utime_times) == sizeof(::utimbuf), "do not add members or virtual funcitons to utime_times struct");

#if (CP_REMOVE_DEPRECATED > 0)
CP_FORCE_INLINE
void utime(const char* pathname, ::cp::utime_times const& times, std::error_code& ec) noexcept
{
  CP_ASSERT(pathname);
  CP_ASSERT(!ec);
  
  const int result = ::utime(pathname, &times);
  if (CP_UNLIKELY(-1 == result)) ec = ::cp::make_system_error_code();
}

CP_FORCE_INLINE
void utime(const char* pathname, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(pathname);
  CP_ASSERT(!ec);
  
  const int result = ::utime(pathname, nullptr);
  if (CP_UNLIKELY(-1 == result)) ec = ::cp::make_system_error_code();
}

CP_FORCE_INLINE
void utime(const char* pathname, ::cp::utime_times const& times )
{
  std::error_code ec; 
  ::cp::utime(pathname, times);
  if ( ec )
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("utime pathname: [", pathname, "]"));
  }
}

CP_FORCE_INLINE
void utime(const char* pathname)
{
  std::error_code ec; 
  ::cp::utime(pathname, ec);

  if (CP_UNLIKELY(ec)) 
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("utime pathname: [", pathname, "]"));
  }
}
#endif

CP_FORCE_INLINE
void utimes(const char* pathname, struct ::timeval tv[2], std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(pathname);
  CP_ASSERT(!ec);

  const int result = ::utimes(pathname, tv);
  if ( CP_UNLIKELY(-1 == result)) ec= ::cp::make_system_error_code();
}

CP_FORCE_INLINE
void utimes(const char* pathname, struct ::timeval tv[2])
{
  std::error_code ec;
  ::cp::utimes(pathname, tv);
  if (ec)
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("utimes pathname: [", pathname, "]"));
  }
}

#if _DEFAULT_SOURCE
CP_FORCE_INLINE
void futimes(::cp::file_descriptor const& fd, const timeval tv[2], std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(fd);
  CP_ASSERT(!ec);

  const int status = ::futimes(fd, tv);
  if (CP_UNLIKELY(-1 == status)) ec = ::cp::make_system_error_code();
}

CP_FORCE_INLINE
void futimes(::cp::file_descriptor const& fd, const timeval tv[2])
{
  std::error_code ec;
  ::cp::futimes(fd, tv, ec);
  if (CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("futimes fd [", fd, "]"));
  }
}


CP_FORCE_INLINE
void lutimes(const char* pathname, struct ::timeval tv[2], std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(pathname);
  CP_ASSERT(!ec);

  const int result = ::lutimes(pathname, tv);
  if ( CP_UNLIKELY(-1 == result)) ec= ::cp::make_system_error_code();
}

CP_FORCE_INLINE
void lutimes(const char* pathname, struct ::timeval tv[2])
{
  std::error_code ec;
  ::cp::lutimes(pathname, tv);
  if (ec)
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("lutimes pathname: [", pathname, "]"));
  }
}
#endif

#if (_POSIX_C_SOURCE >= 200809L)
CP_FORCE_INLINE
void utimensat(::cp::file_descriptor const& dirfd, const char* pathname, const ::timespec times[2], int flags, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(dirfd);
  CP_ASSERT(pathname);

  const int status = ::utimensat(dirfd, pathname, times, flags);
  if ( CP_UNLIKELY( -1 == status)) ec = ::cp::make_system_error_code();
}

CP_FORCE_INLINE
void utimensat(::cp::file_descriptor const& dirfd, const char* pathname, const ::timespec times[2], int flags)
{
  std::error_code ec;
  ::cp::utimensat(dirfd, pathname, times, flags, ec);
  if ( CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("utimensat dirfd: [", dirfd, "], pathname: [", pathname, "], flags [", flags, "]"));
  }
}

CP_FORCE_INLINE
void futimens(::cp::file_descriptor const& fd, const ::timespec times[2], std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(fd);
    
  const int status = ::futimens(fd.get(), times);
  if (CP_UNLIKELY(-1 == status)) ec = ::cp::make_system_error_code();
}

CP_FORCE_INLINE
void futimens(::cp::file_descriptor const& fd, const ::timespec times[2])
{
  std::error_code ec;
  ::cp::futimens(fd, times, ec);
  if ( CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("futimens fd[", fd, "]"));
  }
}
#endif

CP_FORCE_INLINE
void setvbuf( ::cp::file const& file, char* buf, int mode, std::size_t size, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(file);

  const int nok = ::setvbuf(file, buf, mode, size);
  if ( CP_UNLIKELY(nok)) ec = ::cp::make_system_error_code();
 }

CP_FORCE_INLINE
void setvbuf( ::cp::file const& file, char* buf, int mode, std::size_t size)
{
  std::error_code ec;
  ::cp::setvbuf(file, buf, mode, size, ec);
  if ( CP_UNLIKELY(ec)) 
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("setvbuf, mode: [", mode, "] size: [", size, "]"));
  }
 }

CP_FORCE_INLINE
void fsync(::cp::file_descriptor const& fd, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(fd);

  const int status = ::fsync(fd);
  if ( CP_UNLIKELY(-1 == status)) ec = ::cp::make_system_error_code();
}

CP_FORCE_INLINE
void fsync(::cp::file_descriptor const& fd)
{
  std::error_code ec;
  ::cp::fsync(fd, ec);
  if ( CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("fync fd: [", fd, "]"));
  }
}

#if (_POSIX_C_SOURCE >= 199309L || _XOPEN_SOURCE >= 500)

CP_FORCE_INLINE
void fdatasync(::cp::file_descriptor const& fd, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(fd);

  const int status = ::fdatasync(fd);
  if ( CP_UNLIKELY(-1 == status)) ec = ::cp::make_system_error_code();
}

CP_FORCE_INLINE
void fdatasync(::cp::file_descriptor const& fd)
{
  std::error_code ec;
  ::cp::fdatasync(fd, ec);
  if ( CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("fdatasync fd: [", fd, "]"));
  }
}
#endif

#if (_POSIX_C_SOURCE >= 200112L)
CP_FORCE_INLINE
void posix_fadvise(::cp::file_descriptor const& fd, ::off_t offset, ::off_t len, int advice, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(fd);
  CP_ASSERT(!ec);
  CP_ASSERT(offset >= 0);// "offset from the begining of the file");

  const int error_number = ::posix_fadvise(fd, offset, len, advice);
  if ( CP_UNLIKELY(error_number)) ec = ::cp::make_system_error_code(error_number);
}

CP_FORCE_INLINE
void posix_fadvise(::cp::file_descriptor const& fd, ::off_t offset, ::off_t len, int advice)
{
  std::error_code ec;
  ::cp::posix_fadvise(fd, offset, len, advice, ec);
  if ( CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("posix_fadvise fd: [", fd, "], offset: [", offset, "], len: [", len, "], advice: [", advice,"]"));
  }
}
#endif

::cp::file_descriptor fileno(::cp::file const& stream, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(stream);

  const int result = ::fileno(stream);
  if ( CP_UNLIKELY(-1 == result)) ec = ::cp::make_system_error_code();
  return ::cp::file_descriptor(result);
}

::cp::file_descriptor fileno(::cp::file const& stream) 
{
  std::error_code ec; 
  ::cp::file_descriptor result = ::cp::fileno(stream, ec);
  if ( ec ) 
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("fileno, FILE*: [", stream ,"]"));
  }
  return result;
}

::cp::file fdopen(::cp::file_descriptor const& fd, const char* mode, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(fd);

  FILE * const stream = ::fdopen(fd, mode);
  if ( CP_UNLIKELY(nullptr == stream)) ec  = ::cp::make_system_error_code();
  return ::cp::file(stream);
}

::cp::file fdopen(::cp::file_descriptor const& fd, const char* mode)
{
  std::error_code ec;
  cp::file result = ::cp::fdopen(fd, mode, ec);
  if ( CP_UNLIKELY(ec)) 
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, cp::concat("fdopen fd", fd, ", mode: [", mode, "]"));
  }
  return result;
}

CP_FORCE_INLINE
void link(const char* oldpath, const char* newpath, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(oldpath);
  CP_ASSERT(newpath); 

  const int status = ::link(oldpath, newpath);
  if(CP_UNLIKELY(-1 == status)) ec = ::cp::make_system_error_code();
}

CP_FORCE_INLINE
void link(const char* oldpath, const char* newpath)
{
  std::error_code ec;
  ::cp::link(oldpath, newpath, ec); 
  if( CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("link oldpath: [", oldpath,"], newpath: [", newpath, "]"));
  }
}

CP_FORCE_INLINE
void unlink(const char* pathname, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(pathname);
  
  const int status = ::unlink(pathname);
  if ( CP_UNLIKELY(-1 == status)) ec = ::cp::make_system_error_code();
}

CP_FORCE_INLINE
void unlink(const char* pathname)
{
  std::error_code ec;
  ::cp::unlink(pathname);
  if ( CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("unlink pathname: [", pathname,"]"));
  }
}

CP_FORCE_INLINE
void rename(const char* oldpath, const char* newpath, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(oldpath);
  CP_ASSERT(newpath);

  int status = ::rename(oldpath, newpath);
  if( CP_UNLIKELY(status)) ec = ::cp::make_system_error_code();
}

CP_FORCE_INLINE
void rename(const char* oldpath, const char* newpath)
{
  std::error_code ec;
  ::cp::rename(oldpath, newpath, ec);
  if ( CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("rename oldpath: [", oldpath,"], newpath: [", newpath, "]"));
  }
}

#if (_XOPEN_SOURCE >= 500 || _POSIX_C_SOURCE >= 200112L)
CP_FORCE_INLINE
void symlink(const char* filepath, const char* linkpath, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(filepath);
  CP_ASSERT(linkpath);

  const int status = ::symlink(filepath, linkpath);
  if ( CP_UNLIKELY(-1 == status)) ec = ::cp::make_system_error_code();
}

CP_FORCE_INLINE
void symlink(const char* filepath, const char* linkpath)
{
  std::error_code ec;
  ::cp::symlink(filepath, linkpath, ec);
  if ( CP_UNLIKELY(ec)) 
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("symlink filepath: [", filepath,"], newpath: [", linkpath, "]"));
  }
}
#endif

#if (_XOPEN_SOURCE >= 500 || _POSIX_C_SOURCE >= 200112L || _BSD_SOURCE)
CP_FORCE_INLINE
ssize_t readlink(const char* pathname, char* buffer, std::size_t bufsz, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(pathname);
  CP_ASSERT(buffer);
  CP_ASSERT(bufsz != 0);
  
  const ssize_t nbytes = ::readlink(pathname, buffer, bufsz);
  if ( CP_UNLIKELY(-1 == nbytes)) ec = ::cp::make_system_error_code();
  return nbytes;
}

CP_FORCE_INLINE
ssize_t readlink(const char* pathname, char* buffer, std::size_t bufsz) 
{
  std::error_code ec;
  const ssize_t result = ::cp::readlink(pathname, buffer, bufsz, ec);
  if(CP_UNLIKELY( ec ))
  { 
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("readlink pathname: [", pathname, "], buffer ", std::uintptr_t(buffer), "], bufsz: [",bufsz, "]"));
  }
  return result;
}
#endif

CP_FORCE_INLINE 
void mkdir(const char* pathname, ::mode_t mode, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(pathname);

  const int status = ::mkdir(pathname, mode);
  if ( CP_UNLIKELY( -1 == status)) ec = ::cp::make_system_error_code();
}

CP_FORCE_INLINE
void mkdir(const char* pathname, ::mode_t mode) 
{
  std::error_code ec;
  ::cp::mkdir(pathname, mode, ec);
  if( CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("mkdir pathname: [", pathname, "], mode: [", mode, "]"));
  }
}

#if defined _DEFAULT_SOURCE

CP_FORCE_INLINE
char* mkdtemp(char* temp, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(temp);
  //TODO( nebojsa ) check if template has last 6 characters XXXXXX

  char* new_dir = ::mkdtemp(temp);
  if( CP_UNLIKELY(nullptr == new_dir)) ec = ::cp::make_system_error_code();
  return new_dir;
}

CP_FORCE_INLINE
char* mkdtemp(char* temp) 
{
  std::error_code ec;
  char* new_dir = ::cp::mkdtemp(temp, ec);

  if (CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("mkdtemp template: [", temp, "]"));
  }
  return new_dir;
}
#endif

CP_FORCE_INLINE
void rmdir(const char* pathname, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(pathname);

  const int status = ::rmdir(pathname);
  if ( CP_UNLIKELY(-1 == status )) ec = ::cp::make_system_error_code();
}

CP_FORCE_INLINE
void rmdir(const char* pathname)
{
  std::error_code ec;
  ::cp::rmdir(pathname, ec);
  if ( CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("rmdir pathname: [", pathname, "]"));
  }
}

struct dir_traits 
{
  static constexpr DIR* invalid(void) noexcept { return nullptr; }
  static void close(DIR* dp) noexcept { ::closedir(dp); }
}; 

using dir_stream  = ::cp::unique_handle<DIR*, ::cp::dir_traits>;

CP_FORCE_INLINE
std::string to_string(dir_stream const& d)
{
  return d ? ::cp::to_string(std::uintptr_t(d.get())) : "invalid";
}

CP_DEFINE_SERIALIZATION_SPECIALIZATION(::cp::dir_stream);

CP_FORCE_INLINE
::cp::dir_stream opendir(const char* dirpath, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(dirpath);

  ::cp::dir_stream result( ::opendir(dirpath));
  if ( CP_UNLIKELY(!result)) ec == ::cp::make_system_error_code();
  return result;
}

CP_FORCE_INLINE
::cp::dir_stream opendir(const char* dirpath) 
{
  std::error_code ec; 
  ::cp::dir_stream result = ::cp::opendir(dirpath, ec);
  if ( CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("opendir dirpath: [", dirpath, "]"));
  }
  return result;
}

#if (_POSIX_C_SOURCE >= 200809L)

CP_FORCE_INLINE
::cp::dir_stream fdopendir(::cp::file_descriptor const& dir_fd, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(dir_fd);
  CP_ASSERT(::cp::is_directory(dir_fd));

  ::cp::dir_stream result( ::fdopendir(dir_fd));
  if (CP_UNLIKELY(result)) ec = ::cp::make_system_error_code();
  return result;
}

CP_FORCE_INLINE
::cp::dir_stream fdopendir(::cp::file_descriptor const& dir_fd)
{
   std::error_code ec;

   ::cp::dir_stream result(::cp::fdopendir(dir_fd, ec));
   if ( CP_UNLIKELY(ec))
   {
     CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("opendir dirpath: [", dir_fd, "]"));
   }
}
#endif

CP_FORCE_INLINE
::dirent* readdir(::cp::dir_stream const& dirp, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(dirp);

  errno = 0; 
  dirent * const result = ::readdir(dirp);
  if ( CP_UNLIKELY( nullptr == result && errno != 0 )) {
    ec = ::cp::make_system_error_code(errno);
  }

  return result;
}

CP_FORCE_INLINE
::dirent* readdir(::cp::dir_stream const& dirp) 
{
  std::error_code ec; 
  ::dirent* const result = ::cp::readdir( dirp, ec);
  if (CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("readdir dirpath: [", dirp, "]"));
  }
  return result;
}

CP_FORCE_INLINE
void rewinddir(::cp::dir_stream const& dirp) noexcept
{
  ::rewinddir(dirp);
}

#if (_POSIX_C_SOURCE >= 200809L)
CP_FORCE_INLINE
::cp::file_descriptor dirfd(::cp::dir_stream const& dir, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(dir);
  
  ::cp::file_descriptor fd( ::dirfd(dir));
  if (CP_UNLIKELY(!fd)) 
  {
    ec = ::cp::make_system_error_code();
  }
  return fd;
}

CP_FORCE_INLINE
::cp::file_descriptor dirfd(::cp::dir_stream const& dir)
{
  std::error_code ec;
  ::cp::file_descriptor fd = ::cp::dirfd(dir);
  if ( ec ) 
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("dirfd dir stream: [", dir, "]"));
  }
  return fd;
}
#endif 

CP_FORCE_INLINE
constexpr std::size_t size_of_dirent( ) 
{
  return offsetof(::dirent, d_name) + NAME_MAX + 1;
}

#if (CP_REMOVE_DEPRECATED == 0)

CP_FORCE_INLINE
void readdir_r(::cp::dir_stream const& dir, ::dirent* entry, dirent** result, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(dir);
  CP_ASSERT(entry);
  CP_ASSERT(result);

  const int error_no = ::readdir_r(dir, entry, result);
  if ( CP_UNLIKELY(error_no)) ec = ::cp::make_system_error_code(error_no);
}

CP_FORCE_INLINE
void readdir_r(::cp::dir_stream const& dir, ::dirent* entry, dirent** result)
{
  std::error_code ec;
  ::cp::readdir_r(dir, entry, result, ec);
  if ( CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, 
      ::cp::concat("readdir_r dir stream: [", dir, "], entry: [", std::uintptr_t(entry) ,"], result: [", std::uintptr_t(result), "]"));
  }
}

#endif

#if (_XOPEN_SOURCE >= 500)

CP_FORCE_INLINE
int nftw(
  const char * pathdir, 
  int (*func) (const char* pathname, const struct ::stat *statbuf, int flagtype, FTW* ftwbuf), 
  int nopenfd, 
  int flags,
  std::error_code& ec
  ) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(pathdir);

  const int result = ::nftw(pathdir, func, nopenfd, flags);
  if ( CP_UNLIKELY( -1 == result)) ec = ::cp::make_system_error_code();
  return result;
}

CP_FORCE_INLINE
int nftw(
  const char * pathdir, 
  int (*func) (const char* pathname, const struct ::stat *statbuf, int flagtype, FTW* ftwbuf), 
  int nopenfd, 
  int flags
  )
{
  std::error_code ec;
  const int result = ::cp::nftw(pathdir, func, nopenfd, flags);

  if ( CP_UNLIKELY(-1 == result)) ec = ::cp::make_system_error_code();
  return result;
}
#endif

CP_FORCE_INLINE
char *getcwd(char *buf, size_t size, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(buf); // buff = 0 and size= O argumest to getcwd is GNU extension in case of it use get_current_dir_name() func for same behaviour

  char* result = ::getcwd(buf, size);
  if ( CP_UNLIKELY(nullptr == result)) ec = ::cp::make_system_error_code();
  return result;
}

CP_FORCE_INLINE
char *getcwd(char *buf, size_t size)
{
  std::error_code ec;
  char* result = ::cp::getcwd(buf, size, ec);
  if ( CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("getcwd buf: [", std::uintptr_t(buf), "], size: [", size ,"]"));
  }
  return result;
}

#if defined(_GNU_SOURCE)

CP_FORCE_INLINE
::cp::unique_malloc_ptr<char[]> get_current_dir_name( std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  char* result = ::get_current_dir_name();
  if (CP_UNLIKELY(nullptr == result)) ec == ::cp::make_system_error_code();
  return ::cp::unique_malloc_ptr<char[]>(result);
}

CP_FORCE_INLINE
::cp::unique_malloc_ptr<char[]> get_current_dir_name()
{
  std::error_code ec;
  ::cp::unique_malloc_ptr<char[]> result = ::cp::get_current_dir_name(ec);
  if (CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR(ec);
  }
  return result;
}
#endif

#if (CP_REMOVE_DEPRECATED == 0)
#if (_BSD_SOURCE || (_XOPEN_SOURCE >= 500 || _XOPEN_SOURCE && _XOPEN_SOURCE_EXTENDED) &&  !(_POSIX_C_SOURCE >= 200809L || _XOPEN_SOURCE >= 700))

[[deprecated("Please read man to see reasons")]]
CP_FORCE_INLINE
char *getwd(char *buf, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(buf);

  char* result = ::getwd(buf);
  if (CP_UNLIKELY(nullptr ==result )) ec == ::cp::make_system_error_code();
  return result;
}

[[deprecated("Please read man to see reasons")]]
CP_FORCE_INLINE
char *getwd(char *buf)
{
  std::error_code ec;
  char * const result = ::cp::getwd(buf, ec);
  if (CP_UNLIKELY(ec))  
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("getwd buf: [", std::uintptr_t(result), "]"));
  }
  return result;
}
#endif
#endif

CP_FORCE_INLINE
void chdir(const char* pathname, std::error_code& ec)
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(pathname);

  const int status = ::chdir(pathname);
  if (CP_UNLIKELY(-1 == status)) ec = ::cp::make_system_error_code();
}

CP_FORCE_INLINE
void chdir(const char* pathname)
{
  std::error_code ec;
  ::cp::chdir(pathname, ec);
  if (CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("chdir pathname: [",pathname, "]"));
  }
}

#if (XOPEN_SOURCE >= 500 || _POSIX_C_SOURCE >= 200809L || _BSD_SOURCE)
CP_FORCE_INLINE
void fchdir(::cp::file_descriptor const& fd, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(fd);
  CP_ASSERT(::cp::is_directory(fd));

  const int status = ::fchdir(fd);
  if (CP_UNLIKELY(-1 == status)) ec = ::cp::make_system_error_code();
}

CP_FORCE_INLINE
void fchdir(::cp::file_descriptor const& fd)
{
  std::error_code ec;
  ::cp::fchdir(fd, ec);
  if ( CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("fchdir fd: [", fd, "]"));
  }
}
#endif

#if (_POSIX_C_SOURCE >= 200809L)

CP_FORCE_INLINE 
::cp::file_descriptor 
openat(::cp::file_descriptor const& dirfd, char const* relpath, int flags, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(relpath != nullptr);
  CP_ASSERT(dirfd);
  CP_ASSERT(::cp::is_directory(dirfd));

  ::cp::file_descriptor result (::openat(dirfd, relpath, flags));
  if (CP_UNLIKELY(!result))
  {
    ec = ::cp::make_system_error_code();
  }
  return result;
}

CP_FORCE_INLINE 
::cp::file_descriptor 
openat(::cp::file_descriptor const& dirfd, char const* relpath, int flags)
{
  std::error_code ec;
  ::cp::file_descriptor result = ::cp::openat(dirfd, relpath, flags, ec);
  if (CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("opendat dirfd: [", dirfd, "],  file: [", relpath, "], flags: [", flags,"]"));
  }
  return result;
}

CP_FORCE_INLINE 
::cp::file_descriptor 
openat(::cp::file_descriptor const& dirfd, char const * relpath, int flags, ::mode_t mode, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(relpath);
  CP_ASSERT(dirfd);
  CP_ASSERT(::cp::is_directory(dirfd));

  ::cp::file_descriptor result(
    ::openat(dirfd, relpath, flags, mode)
  );
  if (CP_UNLIKELY(!result) )
  {
    ec = ::cp::make_system_error_code();
  }
  return result;
}

CP_FORCE_INLINE 
::cp::file_descriptor 
openat(::cp::file_descriptor const& dirfd, char const * relpath, int flags, ::mode_t mode) 
{
  std::error_code ec;
  ::cp::file_descriptor result = ::cp::openat(dirfd,relpath, flags, mode, ec);
  if (CP_UNLIKELY( ec ))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("openat dirfd: [", dirfd, ", file: [", relpath, "], flags: [", flags, "], mode [", mode, "]"));
  }
  return result;
}

CP_FORCE_INLINE 
::cp::file_descriptor 
openat(char const* relpath, int flags, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(relpath != nullptr);

  ::cp::file_descriptor result (::openat(AT_FDCWD, relpath, flags));
  if (CP_UNLIKELY(!result))
  {
    ec = ::cp::make_system_error_code();
  }
  return result;
}

CP_FORCE_INLINE 
::cp::file_descriptor 
openat(char const* relpath, int flags)
{
  std::error_code ec;
  ::cp::file_descriptor result = ::cp::openat( relpath, flags, ec);
  if (CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("opendat dirfd: [AT_FDCWD],  file: [", relpath, "], flags: [", flags,"]"));
  }
  return result;
}

CP_FORCE_INLINE 
::cp::file_descriptor 
openat(char const * relpath, int flags, ::mode_t mode, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(relpath);

  ::cp::file_descriptor result(
    ::openat(AT_FDCWD, relpath, flags, mode)
  );
  if (CP_UNLIKELY(!result) )
  {
    ec = ::cp::make_system_error_code();
  }
  return result;
}

CP_FORCE_INLINE 
::cp::file_descriptor 
openat(char const * relpath, int flags, ::mode_t mode) 
{
  std::error_code ec;
  ::cp::file_descriptor result = ::cp::openat(relpath, flags, mode, ec);
  if (CP_UNLIKELY( ec ))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("openat dirfd: [AT_FDCWD], file: [", relpath, "], flags: [", flags, "], mode [", mode, "]"));
  }
  return result;
}

CP_FORCE_INLINE
std::error_code faccessat(::cp::file_descriptor const& dirfd, const char* pathname, int mode, int flag) noexcept
{
  CP_ASSERT(dirfd);
  CP_ASSERT(::cp::is_directory(dirfd));

  int result = ::faccessat(dirfd, pathname, mode, flag);
  if (!result) result = errno;
  return ::cp::make_system_error_code(result);
}

CP_FORCE_INLINE
void fstatat(::cp::file_descriptor const& dirfd, const char *relpath, ::cp::file_info& file_info, int flags, std::error_code& ec) noexcept
{

  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(dirfd);
  CP_ASSERT(::cp::is_directory(dirfd));

  const int status = ::fstatat(dirfd, relpath, &file_info, flags);
  if (CP_UNLIKELY( -1 == status)) ec = ::cp::make_system_error_code();
}

CP_FORCE_INLINE
void fstatat(::cp::file_descriptor const& dirfd, const char *relpath, ::cp::file_info& file_info, int flags)
{
  std::error_code ec;
  ::cp::fstatat(dirfd, relpath, file_info, flags, ec);
  if ( CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("fstatat fd: [", dirfd, "], file: [", relpath, "], flags: [", flags, "]"));
  }
}

CP_FORCE_INLINE
void fstatat(const char *relpath, ::cp::file_info& file_info, int flags, std::error_code& ec) noexcept
{

  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);

  const int status = ::fstatat(AT_FDCWD, relpath, &file_info, flags);
  if (CP_UNLIKELY( -1 == status)) ec = ::cp::make_system_error_code();
}

CP_FORCE_INLINE
void fstatat( const char *relpath, ::cp::file_info& file_info, int flags)
{
  std::error_code ec;
  ::cp::fstatat( relpath, file_info, flags, ec);
  if ( CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("fstatat fd: [AT_FDCWD], file: [", relpath, "], flags: [", flags, "]"));
  }
}

CP_FORCE_INLINE
void linkat(
  ::cp::file_descriptor const& olddir_fd, // valid dir file descriptor or uninitilazed than dir is current working directory
  const char* old_relpath,                // path relative to old_relpath if not fullpath 
  ::cp::file_descriptor const& newdir_fd, // valid dir file descriptor or uninitilazed than dir path shuult be relative to olddif
  const char* new_relpath,                // path relative to new path
  int flags, 
  std::error_code& ec
) 
  noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(ec);

  const int status = ::linkat( 
    (!!olddir_fd) ? olddir_fd : AT_FDCWD,
    old_relpath, 
    (!!newdir_fd) ? newdir_fd : AT_FDCWD, 
    new_relpath,
    flags
    );

    if ( CP_UNLIKELY(-1 == status)) ec = ::cp::make_system_error_code();
}

CP_FORCE_INLINE
void linkat(
  ::cp::file_descriptor const& olddir_fd, // valid dir file descriptor or uninitilazed than dir is current working directory
  const char* old_relpath,                // path relative to old_relpath if not fullpath 
  ::cp::file_descriptor const& newdir_fd, // valid dir file descriptor or uninitilazed than dir path shuult be relative to olddif
  const char* new_relpath,                // path relative to new path
  int flags)
{
  std::error_code ec;
  cp::linkat(olddir_fd, old_relpath, newdir_fd, new_relpath, flags, ec);

  if( CP_UNLIKELY(ec))
  {
   CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat(
       "linkat : olddir_fd: [", olddir_fd, "], old_replpath: [", old_relpath, 
       "], newdir_fd: [", newdir_fd, "], new_replpath: [",new_relpath , "], flags: [", flags, "]"
     )
   );
  }
}

CP_FORCE_INLINE
void unlinkat(::cp::file_descriptor const& dirfd, const char* relpath, int flags, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(relpath);
  CP_ASSERT(dirfd);
  CP_ASSERT(::cp::is_directory(dirfd));
  
  const int status = ::unlinkat(dirfd, relpath, flags);
  if ( CP_UNLIKELY(-1 == status)) ec = ::cp::make_system_error_code();
}

CP_FORCE_INLINE
void unlinkat(::cp::file_descriptor const& dirfd, const char* relpath, int flags)
{
  std::error_code ec;
  ::cp::unlinkat(dirfd, relpath, flags ,ec);
  if ( CP_UNLIKELY( ec ))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat( "unlinkat : dirfd: [", dirfd, "], relpath: [", relpath, ", flags: [", flags, "]"));
  }
}

CP_FORCE_INLINE
void unlinkat(const char* relpath, int flags, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(relpath);
  
  const int status = ::unlinkat(AT_FDCWD, relpath, flags);
  if ( CP_UNLIKELY(-1 == status)) ec = ::cp::make_system_error_code();
}

CP_FORCE_INLINE
void unlinkat(const char* relpath, int flags)
{
  std::error_code ec;
  ::cp::unlinkat(relpath, flags ,ec);
  if ( CP_UNLIKELY( ec ))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat( "unlinkat : dirfd: [AT_FDCWD], relpath: [", relpath, ", flags: [", flags, "]"));
  }
}

CP_FORCE_INLINE
void mkdirat(::cp::file_descriptor const& dirfd, const char* relpath , ::mode_t mode, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(relpath);
  CP_ASSERT(dirfd);
  CP_ASSERT(::cp::is_directory(dirfd));

  const int status = ::mkdirat(dirfd, relpath, mode);
  if ( CP_UNLIKELY( -1 == status)) ec = ::cp::make_system_error_code();
}

CP_FORCE_INLINE
void mkdirat(::cp::file_descriptor const& dirfd, const char* relpath , ::mode_t mode)
{
  std::error_code ec;
  ::cp::mkdirat(dirfd, relpath, mode, ec);
  if ( CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("mkdir: dirfd [", dirfd, "], relpath: [", relpath, "], mode: [", mode, "]"));
  }
}

CP_FORCE_INLINE
void mkdirat(const char* relpath , ::mode_t mode, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(relpath);

  const int status = ::mkdirat(AT_FDCWD, relpath, mode);
  if ( CP_UNLIKELY( -1 == status)) ec = ::cp::make_system_error_code();
}

CP_FORCE_INLINE
void mkdirat(const char* relpath , ::mode_t mode)
{
  std::error_code ec;
  ::cp::mkdirat(relpath, mode, ec);
  if ( CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("mkdir: dirfd [AT_FDCWD], relpath: [", relpath, "], mode: [", mode, "]"));
  }
}

CP_FORCE_INLINE
void symlinkat(const char *target, ::cp::file_descriptor const& newdirfd, const char *linkpath, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(target);
  CP_ASSERT(linkpath);
  CP_ASSERT(newdirfd);
  CP_ASSERT(::cp::is_directory(newdirfd));

  const int status = ::symlinkat(target, newdirfd, linkpath);
  if ( CP_UNLIKELY(-1==status)) ec = ::cp::make_system_error_code();
}

CP_FORCE_INLINE
void symlinkat(const char *target, ::cp::file_descriptor const& newdirfd, const char *linkpath) 
{
  std::error_code ec;
  ::cp::symlinkat(target, newdirfd, linkpath, ec);
  if ( CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("symlinkat, targed: [", target, "], dirfd: [", newdirfd, "] linkpath[", linkpath, "]"));
  }
}

CP_FORCE_INLINE
void symlinkat(const char *target,  const char *linkpath, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(target);
  CP_ASSERT(linkpath);

  const int status = ::symlinkat(target, AT_FDCWD, linkpath);
  if ( CP_UNLIKELY(-1==status)) ec = ::cp::make_system_error_code();
}

CP_FORCE_INLINE
void symlinkat(const char *target,  const char *linkpath) 
{
  std::error_code ec;
  ::cp::symlinkat(target, linkpath, ec);
  if ( CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("symlinkat, targed: [", target, "], dirfd: [AT_FDCWD] linkpath[", linkpath, "]"));
  }
}

CP_FORCE_INLINE
void renameat(
  ::cp::file_descriptor const& olddir_fd, // valid dir file descriptor or uninitilazed than dir is current working directory
  const char* old_relpath,                // path relative to old_relpath if not fullpath 
  ::cp::file_descriptor const& newdir_fd, // valid dir file descriptor or uninitilazed than dir path shuult be relative to olddif
  const char* new_relpath,                // path relative to new path
  std::error_code& ec
) 
  noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(ec);

  const int status = ::renameat( 
    (!!olddir_fd) ? olddir_fd : AT_FDCWD,
    old_relpath, 
    (!!newdir_fd) ? newdir_fd : AT_FDCWD, 
    new_relpath
    );

    if ( CP_UNLIKELY(-1 == status)) ec = ::cp::make_system_error_code();
}

CP_FORCE_INLINE
void renameat(
  ::cp::file_descriptor const& olddir_fd, // valid dir file descriptor or uninitilazed than dir is current working directory
  const char* old_relpath,                // path relative to old_relpath if not fullpath 
  ::cp::file_descriptor const& newdir_fd, // valid dir file descriptor or uninitilazed than dir path shuult be relative to olddif
  const char* new_relpath                 // path relative to new path
  )
{
  std::error_code ec;
  ::cp::renameat(olddir_fd, old_relpath, newdir_fd, new_relpath, ec);

  if( CP_UNLIKELY(ec))
  {
   CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat(
       "renameat : olddir_fd: [", olddir_fd, "], old_replpath: [", old_relpath, 
       "], newdir_fd: [", newdir_fd, "], new_replpath: [",new_relpath , "]"
     )
   );
  }
}

ssize_t readlinkat(::cp::file_descriptor const& dirfd, const char *pathname, char *buf, size_t bufsiz, std::error_code& ec) noexcept
{
// NOT SUPPORTED If you find this limiting use readlink
//
//       Since Linux 2.6.39, pathname can be an empty string, in which case
//       the call operates on the symbolic link referred to by dirfd (which
//       should have been obtained using open(2) with the O_PATH and
//       O_NOFOLLOW flags).

  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(dirfd);
  CP_ASSERT(::cp::is_directory(dirfd));
  CP_ASSERT(pathname);

  const ::ssize_t result =  ::readlinkat(dirfd, pathname, buf, bufsiz);
  if ( CP_UNLIKELY(-1 == result)) ec = ::cp::make_system_error_code();
  return result;
}

ssize_t readlinkat(::cp::file_descriptor const& dirfd, const char *pathname, char *buf, size_t bufsiz)
{
  std::error_code ec;
  const ::size_t result = ::cp::readlinkat(dirfd, pathname, buf, bufsiz,ec);
  if ( CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, 
      ::cp::concat("readlink dirfd: [", dirfd, "] pathname: [", pathname,
                   "], buffer: [", std::uintptr_t(buf), ", bufsiz: [", bufsiz, "]"
      )
    );
  }
  return result;
}

ssize_t readlinkat(const char *pathname, char *buf, size_t bufsiz, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(pathname);

  const ::ssize_t result =  ::readlinkat(AT_FDCWD, pathname, buf, bufsiz);
  if ( CP_UNLIKELY(-1 == result)) ec = ::cp::make_system_error_code();
  return result;
}

ssize_t readlinkat( const char *pathname, char *buf, size_t bufsiz)
{
  std::error_code ec;
  const ::size_t result = ::cp::readlinkat(pathname, buf, bufsiz,ec);
  if ( CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, 
      ::cp::concat("readlink dirfd: [AT_FDCWD] pathname: [", pathname,
                   "], buffer: [", std::uintptr_t(buf), ", bufsiz: [", bufsiz, "]"
      )
    );
  }
  return result;
}

#endif

#if ( _XOPEN_SOURCE && ! (_POSIX_C_SOURCE >= 200112L) || _DEFAULT_SOURCE  ||  _BSD_SOURCE)

CP_FORCE_INLINE
void chroot(const char* pathname, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(pathname);

  const int status = ::chroot(pathname);
  if ( CP_UNLIKELY(-1 == status)) ec = ::cp::make_system_error_code();
}

CP_FORCE_INLINE
void chroot(const char* pathname)
{
  std::error_code ec;

  ::cp::chroot(pathname,ec);
  if( CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("chroot pathame: [", pathname, "]"));
  }
}
#endif

CP_FORCE_INLINE
char* realpath(const char* pathname, char* resolved_path, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(pathname);
  CP_ASSERT(resolved_path); // check relpath(const char* );

  char * const  result = ::realpath(pathname, resolved_path);
  if(CP_UNLIKELY(!result)) ec == ::cp::make_system_error_code();
  return result;
}

CP_FORCE_INLINE
char* realpath(const char* pathname, char* resolved_path)
{
  std::error_code ec;
  char * const result = ::cp::realpath(pathname, resolved_path, ec);
  if ( CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("realpath, pathname: [", pathname, "]"));
  }
  return result;
}


CP_FORCE_INLINE
::cp::unique_malloc_ptr<char[]> realpath(const char* pathname, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(pathname);

  char * const  result = ::realpath(pathname, nullptr);
  if(CP_UNLIKELY(!result)) ec == ::cp::make_system_error_code();
  return ::cp::unique_malloc_ptr<char[]>(result);
}

CP_FORCE_INLINE
::cp::unique_malloc_ptr<char[]> realpath(const char* pathname)
{
  std::error_code ec;

  ::cp::unique_malloc_ptr<char[]> result = ::cp::realpath(pathname, ec);
  if ( CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("realpath, pathname: [", pathname, "]"));
  }
  return result;
}

CP_FORCE_INLINE 
char *dirname(char *pathname, std::error_code& ec) noexcept 
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(pathname);

  char * const result = ::dirname(pathname);
  if (CP_UNLIKELY( nullptr == result)) ec = ::cp::make_system_error_code();
  return result;
}

CP_FORCE_INLINE 
char *dirname(char *pathname)
{
  std::error_code ec;
  char * const result = ::cp::dirname(pathname, ec);
  if ( CP_UNLIKELY(ec)) 
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("dirname pathname: [", pathname, "]"));
  }
  return result;
}

CP_FORCE_INLINE
char *basename(char *pathname, std::error_code& ec) noexcept 
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(pathname);

  char * const result = ::basename(pathname);
  if ( CP_UNLIKELY(nullptr == pathname)) ec = ::cp::make_system_error_code();
  return result;
}

CP_FORCE_INLINE 
char *basename(char *pathname)
{
  std::error_code ec;
  char * const result = ::cp::basename(pathname, ec);
  if ( CP_UNLIKELY(ec)) 
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("basename pathname: [", pathname, "]"));
  }
  return result;
}

CP_FORCE_INLINE
struct ::passwd *getpwnam(const char *name, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(name);

  errno = 0; 
  ::passwd* const result = ::getpwnam(name);
  if (CP_UNLIKELY( nullptr == result && errno)) ec = ::cp::make_system_error_code();

  return result;
}

CP_FORCE_INLINE
struct ::passwd *getpwnam(const char *name)
{
  std::error_code ec;

  ::passwd* const result = ::cp::getpwnam(name, ec);
  if ( CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("getpwnam name: [", name, "]"));
  }
  return result;
}

CP_FORCE_INLINE
::passwd *getpwuid(uid_t uid, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(uid);

  errno = 0; 
  ::passwd* const result = ::getpwuid(uid);
  if (CP_UNLIKELY( nullptr == result && errno)) ec = ::cp::make_system_error_code();

  return result;
}

CP_FORCE_INLINE
::passwd *getpwuid(::uid_t uid)
{
  std::error_code ec;

  ::passwd* const result = ::cp::getpwuid(uid);
  if ( CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("getpwuid uid: [", uid, "]"));
  }
  return result;
}

struct ::group *getgrnam(const char *name, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(name);

  errno = 0; 
  ::group * const result = ::getgrnam(name);
  if (CP_UNLIKELY( nullptr == result && errno)) ec = ::cp::make_system_error_code();
  return result;
}

CP_FORCE_INLINE
struct ::group *getgrnam(const char *name)
{
  std::error_code ec;

  ::group * const result = ::cp::getgrnam(name, ec);
  if ( CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("getgrnam name: [", name, "]"));
  }
  return result;
}

CP_FORCE_INLINE
struct ::group *getgrgid(::gid_t gid, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(gid);

  errno = 0; 
  ::group* const result = ::getgrgid(gid);
  if (CP_UNLIKELY( nullptr == result && errno)) ec = ::cp::make_system_error_code();

  return result;
}

CP_FORCE_INLINE
struct ::group *getgrgid(::gid_t gid)
{
  std::error_code ec;

  ::group* const result = ::cp::getgrgid(gid);
  if ( CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("getgrgid uid: [", gid, "]"));
  }
  return result;
}

#if (_POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _BSD_SOURCE || _SVID_SOURCE || _POSIX_SOURCE )

namespace detail {
  CP_FORCE_INLINE
  bool check_getpwnam_r_buffer_size(std::size_t nbytes)
  {
    errno = 0; 
    const int min_bytes = sysconf(_SC_GETPW_R_SIZE_MAX);
    const int error_number = errno;
    if ( (-1 == min_bytes) && (error_number != 0))
    {
      CP_THROW_SYSTEM_ERROR_MSG(::cp::make_system_error_code(error_number), ::cp::concat("check_getpwnam_r_buffer_size unable to check bounds error"));
    }
    return (-1 == min_bytes) ? true : ((std::size_t)min_bytes <= nbytes);
  }
}

CP_FORCE_INLINE
bool getpwnam_r(const char *name, struct ::passwd *pwd, char *buf, size_t buflen, std::error_code& ec) noexcept
// return if the user is found
{

  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(name);
  CP_ASSERT(pwd);
  CP_ASSERT(buf);
  CP_ASSERT(buflen > 0); // check_getpwnam_r_buffer_size will return true when system can not estimate size of buffer, so i do elementar check here
  CP_ASSERT(::cp::detail::check_getpwnam_r_buffer_size(buflen));
  
  struct ::passwd* result = nullptr;
  
  const int error_number = ::getpwnam_r(name, pwd, buf, buflen, &result);
  if (CP_UNLIKELY( nullptr == result))
  {
    if ( 0 == error_number) return false;
    ec = ::cp::make_system_error_code(error_number);
  }
  return true;
}

CP_FORCE_INLINE
bool getpwnam_r(const char *name, struct ::passwd *pwd, char *buf, size_t buflen)
{
  std::error_code ec;
  const bool found = ::cp::getpwnam_r(name, pwd, buf, buflen,ec);
  if ( CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, 
      ::cp::concat("getpwan_r name: [", name, "] pwd: [", std::uintptr_t(pwd), "], buf: [", std::uintptr_t(buf), "], buflen: [", buflen,"]")
      );
  }
  return found;
}

CP_FORCE_INLINE
bool getpwuid_r(::uid_t uid, struct ::passwd *pwd, char *buf, std::size_t buflen, std::error_code& ec) noexcept
// return if the user is found
{

  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(pwd);
  CP_ASSERT(buf);
  CP_ASSERT(buflen > 0); // check_getpwnam_r_buffer_size will return true when system can not estimate size of buffer, so i do elementar check here
  CP_ASSERT(::cp::detail::check_getpwnam_r_buffer_size(buflen));
  
  struct ::passwd* result = nullptr;
  
  const int error_number = ::getpwuid_r(uid, pwd, buf, buflen, &result);
  if (CP_UNLIKELY( nullptr == result))
  {
    if ( 0 == error_number) return false;
    ec = ::cp::make_system_error_code(error_number);
  }
  return true;
}

CP_FORCE_INLINE
bool getpwuid_r(::uid_t uid, struct ::passwd *pwd, char *buf, std::size_t buflen)
{
  std::error_code ec;

  const bool found = ::cp::getpwuid_r(uid, pwd, buf, buflen, ec);
  if ( CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, 
      ::cp::concat("getpwuid_r uid: [", uid, "] pwd: [", std::uintptr_t(pwd), "], buf: [", std::uintptr_t(buf), "], buflen: [", buflen,"]")
      );
  }
  return found;
}

namespace detail {
  CP_FORCE_INLINE
  bool check_getgrnam_r_buffer_size(std::size_t nbytes)
  {
    errno = 0; 
    const int min_bytes = sysconf(_SC_GETGR_R_SIZE_MAX);
    const int error_number = errno;
    if ( (-1 == min_bytes) && (error_number != 0))
    {
      CP_THROW_SYSTEM_ERROR_MSG(::cp::make_system_error_code(error_number), ::cp::concat("check_getgrnam_r_buffer_size unable to check bounds error"));
    }
    return (-1 == min_bytes) ? true : ((std::size_t)min_bytes <= nbytes);
  }
}

CP_FORCE_INLINE
bool getgrnam_r(const char *name, struct ::group *grp, char *buf, size_t buflen, std::error_code& ec) noexcept
// return if the group is found
{

  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(name);
  CP_ASSERT(grp);
  CP_ASSERT(buf);
  CP_ASSERT(buflen > 0); // check_getgrnam_r_buffer_size will return true when system can not estimate size of buffer, so i do elementar check here
  CP_ASSERT(::cp::detail::check_getgrnam_r_buffer_size(buflen));
  
  struct ::group* result = nullptr;
  
  const int error_number = ::getgrnam_r(name, grp, buf, buflen, &result);
  if (CP_UNLIKELY( nullptr == result))
  {
    if ( 0 == error_number) return false;
    ec = ::cp::make_system_error_code(error_number);
  }
  return true;
}

CP_FORCE_INLINE
bool getgrnam_r(const char *name, struct ::group *grp, char *buf, size_t buflen)
// return if the group is found
{
  std::error_code ec;
  const bool found = ::cp::getgrnam_r(name, grp, buf, buflen,ec);
  if ( CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, 
      ::cp::concat("getgrnam_r name: [", name, "] grp: [", std::uintptr_t(grp), "], buf: [", std::uintptr_t(buf), "], buflen: [", buflen,"]")
      );
  }
  return found;
}

CP_FORCE_INLINE
bool getgrgid_r(::gid_t gid, struct ::group *grp, char *buf, std::size_t buflen, std::error_code& ec) noexcept
// return if the group is found
{

  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);
  CP_ASSERT(grp);
  CP_ASSERT(buf);
  CP_ASSERT(buflen > 0); // check_getgrnam_r_buffer_size will return true when system can not estimate size of buffer, so i do elementar check here
  CP_ASSERT(::cp::detail::check_getgrnam_r_buffer_size(buflen));
  
  struct ::group* result = nullptr;
  
  const int error_number = ::getgrgid_r(gid, grp, buf, buflen, &result);
  if (CP_UNLIKELY( nullptr == result))
  {
    if ( 0 == error_number) return false;
    ec = ::cp::make_system_error_code(error_number);
  }
  return true;
}

CP_FORCE_INLINE
bool getgrgid_r(::gid_t gid, struct ::group *grp, char *buf, std::size_t buflen)
// return if the group is found
{
  std::error_code ec;
  const bool found = ::cp::getgrgid_r(gid, grp, buf, buflen, ec);
  if ( CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, 
      ::cp::concat("getgrgid_r gid: [", gid, "] grp: [", std::uintptr_t(grp), "], buf: [", std::uintptr_t(buf), "], buflen: [", buflen,"]")
      );
  }
  return found;
}

#endif

struct pswd_environment
{
  pswd_environment() noexcept { ::setpwent(); }
  struct ::passwd* getpwent( std::error_code& ec) noexcept 
  {
    static_assert(std::is_lvalue_reference<decltype(ec)>::value);
    CP_ASSERT(!ec);

    struct ::passwd* result = ::getpwent();
    if (CP_UNLIKELY(nullptr == result && errno)) ec = ::cp::make_system_error_code();
    return result;
  }

  struct ::passwd* getpwent( ) {
    std::error_code ec;

    struct ::passwd* result = getpwent(ec);
    if ( CP_UNLIKELY(ec))
    {
      CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("pswd_environment::getpwent"));
    }
    return result;
  }
 ~pswd_environment() { ::endpwent(); }
};

CP_FORCE_INLINE ::uid_t getuid()  noexcept { return ::getuid();  }
CP_FORCE_INLINE ::uid_t geteuid() noexcept { return ::geteuid(); }
CP_FORCE_INLINE ::gid_t getgid()  noexcept { return ::getgid();  }
CP_FORCE_INLINE ::gid_t getegid() noexcept { return ::getegid(); }

CP_FORCE_INLINE
void setuid(::uid_t uid, std::error_code& ec) noexcept 
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);

  const int status = ::setuid(uid);
  if ( CP_UNLIKELY(-1 == status)) ec = ::cp::make_system_error_code();
}

CP_FORCE_INLINE
void setuid(::uid_t uid) 
{
  std::error_code ec;
  ::cp::setuid(uid, ec); 
  if (CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("setuid [", uid,"]"));
  }
}

CP_FORCE_INLINE
void setgid(::gid_t gid, std::error_code& ec) noexcept 
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);

  const int status = ::setgid(gid); 
  if ( CP_UNLIKELY(-1 == status)) ec = ::cp::make_system_error_code();
}

CP_FORCE_INLINE
void setgid(::gid_t gid) 
{
  std::error_code ec;

  ::cp::setgid(gid, ec);
  if ( CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("setgid [", gid, "]"));
  }
}

CP_FORCE_INLINE 
void seteuid(::uid_t uid, std::error_code& ec) noexcept 
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);

  const int status = ::seteuid(uid);
  if ( CP_UNLIKELY( -1 == status)) ec = ::cp::make_system_error_code();
}

CP_FORCE_INLINE
void seteuid(::uid_t uid)
{
  std::error_code ec;
  ::cp::seteuid(uid, ec);
  if ( CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("seteuid [", uid, "]"));
  }
}

CP_FORCE_INLINE 
void setegid(::gid_t gid, std::error_code& ec) noexcept 
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);

  const int status = ::setegid(gid);
  if ( CP_UNLIKELY( -1 == status)) ec = ::cp::make_system_error_code();  
}

CP_FORCE_INLINE
void setegid(::gid_t gid) 
{
  std::error_code ec;
  ::cp::setegid(gid, ec);
  if ( CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("setegid gid[", gid, "]"));
  }
}

#if defined _GNU_SOURCE
CP_FORCE_INLINE
void setresuid( ::uid_t ruid, ::uid_t euid, ::uid_t suid, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);

  const int status = ::setresuid(ruid, euid, suid); 
  if( CP_UNLIKELY( -1 == status)) ec = ::cp::make_system_error_code();
}

CP_FORCE_INLINE
void setresuid( ::uid_t ruid, ::uid_t euid, ::uid_t suid)
{
  std::error_code ec;
  ::cp::setresuid(ruid, euid, suid, ec); 
  if ( CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("setresuid gid: [", ruid, "], euid: [",euid, "], suid: [", suid, "]"));
  }
}

CP_FORCE_INLINE
void setresgid(::gid_t rgid, ::gid_t egid, ::gid_t sgid, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);

  const int status = ::setresgid(rgid, egid, sgid);
  if ( CP_UNLIKELY(-1 == status)) ec = ::cp::make_system_error_code();
}

CP_FORCE_INLINE
void setresgid(::gid_t rgid, ::gid_t egid, ::gid_t sgid)
{
  std::error_code ec;

  ::cp::setresgid(rgid, egid, sgid, ec);
  if ( CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("setresgid rgid: [",rgid,"], egid: [", egid, "], sgid: [", sgid, "]"));
  }
}

#endif

CP_FORCE_INLINE
void gettimeofday( struct ::timeval& tv, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);

  const int status = ::gettimeofday(&tv, nullptr);
  if ( CP_UNLIKELY( -1 == status)) ec = ::cp::make_system_error_code(); 
}

CP_FORCE_INLINE
void gettimeofday( struct ::timeval& tv)
{
  std::error_code ec;
  cp::gettimeofday(tv, ec);
  if ( CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("gettimeofday"));
  }
}

#if defined _DEFAULT_SOURCE

CP_FORCE_INLINE
void settimeofday( struct ::timeval& tv, std::error_code& ec) noexcept
{
  static_assert(std::is_lvalue_reference<decltype(ec)>::value);
  CP_ASSERT(!ec);

  const int status = ::settimeofday(&tv, nullptr);
  if ( CP_UNLIKELY( -1 == status)) ec = ::cp::make_system_error_code(); 
}

CP_FORCE_INLINE
void settimeofday(struct ::timeval& tv)
{
  std::error_code ec;
  cp::settimeofday(tv, ec);
  if ( CP_UNLIKELY(ec))
  {
    CP_THROW_SYSTEM_ERROR_MSG(ec, ::cp::concat("settimeofday"));
  }
}

#endif

struct time_clock
{
  using rep = int;
  using duration = std::chrono::duration<rep>;
  using period = duration::period;
  using time_point = std::chrono::time_point<time_clock>;
  static const bool is_steady = true;

  static time_point now() noexcept 
  {
    return time_point{duration{std::time(nullptr)}};
  }
};

} // namespace cp