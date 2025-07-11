

#include "SocketServer.hpp"

#include <errno.h>



const char * ErrnoText(int eno)
{
    switch (eno)
    {
        case EPERM              : return "EPERM"            ;
        case ENOENT             : return "ENOENT"           ;
        case ESRCH              : return "ESRCH"            ;
        case EINTR              : return "EINTR"            ;
        case EIO                : return "EIO"              ;
        case ENXIO              : return "ENXIO"            ;
        case E2BIG              : return "E2BIG"            ;
        case ENOEXEC            : return "ENOEXEC"          ;
        case EBADF              : return "EBADF"            ;
        case ECHILD             : return "ECHILD"           ;
        case EAGAIN             : return "EAGAIN"           ;
        case ENOMEM             : return "ENOMEM"           ;
        case EACCES             : return "EACCES"           ;
        case EFAULT             : return "EFAULT"           ;
        case ENOTBLK            : return "ENOTBLK"          ;
        case EBUSY              : return "EBUSY"            ;
        case EEXIST             : return "EEXIST"           ;
        case EXDEV              : return "EXDEV"            ;
        case ENODEV             : return "ENODEV"           ;
        case ENOTDIR            : return "ENOTDIR"          ;
        case EISDIR             : return "EISDIR"           ;
        case EINVAL             : return "EINVAL"           ;
        case ENFILE             : return "ENFILE"           ;
        case EMFILE             : return "EMFILE"           ;
        case ENOTTY             : return "ENOTTY"           ;
        case ETXTBSY            : return "ETXTBSY"          ;
        case EFBIG              : return "EFBIG"            ;
        case ENOSPC             : return "ENOSPC"           ;
        case ESPIPE             : return "ESPIPE"           ;
        case EROFS              : return "EROFS"            ;
        case EMLINK             : return "EMLINK"           ;
        case EPIPE              : return "EPIPE"            ;
        case EDOM               : return "EDOM"             ;
        case ERANGE             : return "ERANGE"           ;
        case EDEADLK            : return "EDEADLK"          ;
        case ENAMETOOLONG       : return "ENAMETOOLONG"     ;
        case ENOLCK             : return "ENOLCK"           ;
        case ENOSYS             : return "ENOSYS"           ;
        case ENOTEMPTY          : return "ENOTEMPTY"        ;
        case ELOOP              : return "ELOOP"            ;
        //case EWOULDBLOCK        : return "EWOULDBLOCK"      ;
        case ENOMSG             : return "ENOMSG"           ;
        case EIDRM              : return "EIDRM"            ;
        case ECHRNG             : return "ECHRNG"           ;
        case EL2NSYNC           : return "EL2NSYNC"         ;
        case EL3HLT             : return "EL3HLT"           ;
        case EL3RST             : return "EL3RST"           ;
        case ELNRNG             : return "ELNRNG"           ;
        case EUNATCH            : return "EUNATCH"          ;
        case ENOCSI             : return "ENOCSI"           ;
        case EL2HLT             : return "EL2HLT"           ;
        case EBADE              : return "EBADE"            ;
        case EBADR              : return "EBADR"            ;
        case EXFULL             : return "EXFULL"           ;
        case ENOANO             : return "ENOANO"           ;
        case EBADRQC            : return "EBADRQC"          ;
        case EBADSLT            : return "EBADSLT"          ;
        case EBFONT             : return "EBFONT"           ;
        case ENOSTR             : return "ENOSTR"           ;
        case ENODATA            : return "ENODATA"          ;
        case ETIME              : return "ETIME"            ;
        case ENOSR              : return "ENOSR"            ;
        case ENONET             : return "ENONET"           ;
        case ENOPKG             : return "ENOPKG"           ;
        case EREMOTE            : return "EREMOTE"          ;
        case ENOLINK            : return "ENOLINK"          ;
        case EADV               : return "EADV"             ;
        case ESRMNT             : return "ESRMNT"           ;
        case ECOMM              : return "ECOMM"            ;
        case EPROTO             : return "EPROTO"           ;
        case EMULTIHOP          : return "EMULTIHOP"        ;
        case EDOTDOT            : return "EDOTDOT"          ;
        case EBADMSG            : return "EBADMSG"          ;
        case EOVERFLOW          : return "EOVERFLOW"        ;
        case ENOTUNIQ           : return "ENOTUNIQ"         ;
        case EBADFD             : return "EBADFD"           ;
        case EREMCHG            : return "EREMCHG"          ;
        case ELIBACC            : return "ELIBACC"          ;
        case ELIBBAD            : return "ELIBBAD"          ;
        case ELIBSCN            : return "ELIBSCN"          ;
        case ELIBMAX            : return "ELIBMAX"          ;
        case ELIBEXEC           : return "ELIBEXEC"         ;
        case EILSEQ             : return "EILSEQ"           ;
        case ERESTART           : return "ERESTART"         ;
        case ESTRPIPE           : return "ESTRPIPE"         ;
        case EUSERS             : return "EUSERS"           ;
        case ENOTSOCK           : return "ENOTSOCK"         ;
        case EDESTADDRREQ       : return "EDESTADDRREQ"     ;
        case EMSGSIZE           : return "EMSGSIZE"         ;
        case EPROTOTYPE         : return "EPROTOTYPE"       ;
        case ENOPROTOOPT        : return "ENOPROTOOPT"      ;
        case EPROTONOSUPPORT    : return "EPROTONOSUPPORT"  ;
        case ESOCKTNOSUPPORT    : return "ESOCKTNOSUPPORT"  ;
        case EOPNOTSUPP         : return "EOPNOTSUPP"       ;
        case EPFNOSUPPORT       : return "EPFNOSUPPORT"     ;
        case EAFNOSUPPORT       : return "EAFNOSUPPORT"     ;
        case EADDRINUSE         : return "EADDRINUSE"       ;
        case EADDRNOTAVAIL      : return "EADDRNOTAVAIL"    ;
        case ENETDOWN           : return "ENETDOWN"         ;
        case ENETUNREACH        : return "ENETUNREACH"      ;
        case ENETRESET          : return "ENETRESET"        ;
        case ECONNABORTED       : return "ECONNABORTED"     ;
        case ECONNRESET         : return "ECONNRESET"       ;
        case ENOBUFS            : return "ENOBUFS"          ;
        case EISCONN            : return "EISCONN"          ;
        case ENOTCONN           : return "ENOTCONN"         ;
        case ESHUTDOWN          : return "ESHUTDOWN"        ;
        case ETOOMANYREFS       : return "ETOOMANYREFS"     ;
        case ETIMEDOUT          : return "ETIMEDOUT"        ;
        case ECONNREFUSED       : return "ECONNREFUSED"     ;
        case EHOSTDOWN          : return "EHOSTDOWN"        ;
        case EHOSTUNREACH       : return "EHOSTUNREACH"     ;
        case EALREADY           : return "EALREADY"         ;
        case EINPROGRESS        : return "EINPROGRESS"      ;
        case ESTALE             : return "ESTALE"           ;
        case EUCLEAN            : return "EUCLEAN"          ;
        case ENOTNAM            : return "ENOTNAM"          ;
        case ENAVAIL            : return "ENAVAIL"          ;
        case EISNAM             : return "EISNAM"           ;
        case EREMOTEIO          : return "EREMOTEIO"        ;
        case EDQUOT             : return "EDQUOT"           ;
        case ENOMEDIUM          : return "ENOMEDIUM"        ;
        case EMEDIUMTYPE        : return "EMEDIUMTYPE"      ;
        case ECANCELED          : return "ECANCELED"        ;
        case ENOKEY             : return "ENOKEY"           ;
        case EKEYEXPIRED        : return "EKEYEXPIRED"      ;
        case EKEYREVOKED        : return "EKEYREVOKED"      ;
        case EKEYREJECTED       : return "EKEYREJECTED"     ;
        case EOWNERDEAD         : return "EOWNERDEAD"       ;
        case ENOTRECOVERABLE    : return "ENOTRECOVERABLE"  ;
        case ERFKILL            : return "ERFKILL"          ;
        case EHWPOISON          : return "EHWPOISON"        ;
    }

    return "NA";
}


void to_string(ESRV_RETCODE val, std::ostringstream &ss)
{
    switch (val)
    {
        case ESRV_RETCODE::SUCCESS          : ss << "SUCCESS";          break;
        case ESRV_RETCODE::ERROR_SOCKET     : ss << "ERROR_SOCKET";     break;
        case ESRV_RETCODE::ERROR_BIND       : ss << "ERROR_BIND";       break;
        case ESRV_RETCODE::ERROR_LISTEN     : ss << "ERROR_LISTEN";     break;
        case ESRV_RETCODE::ERROR_ACCEPT     : ss << "ERROR_ACCEPT";     break;
        case ESRV_RETCODE::ERROR_CONNECT    : ss << "ERROR_CONNECT";    break;
        case ESRV_RETCODE::ERROR_SEND       : ss << "ERROR_SEND";       break;
        case ESRV_RETCODE::ERROR_RECV       : ss << "ERROR_RECV";       break;
        case ESRV_RETCODE::ERROR_PACKET     : ss << "ERROR_PACKET";     break;
        case ESRV_RETCODE::ERROR_PARAMETER  : ss << "ERROR_PARAMETER";  break;
        case ESRV_RETCODE::ERROR_PTHREAD    : ss << "ERROR_PTHREAD";    break;
        case ESRV_RETCODE::ERROR_EPOLL      : ss << "ERROR_EPOLL";      break;
        case ESRV_RETCODE::ERROR_EPOLLWAIT  : ss << "ERROR_EPOLLWAIT";  break;

        default                         : ss << "NA";
    }
}



std::string to_string(const epoll_event &val)
{
    std::ostringstream ss;

    ss  << "data.fd : " << val.data.fd << std::endl
        << "events : " << val.events << " - ";

    if (val.events & EPOLLIN)           ss << "EPOLLIN ";
    if (val.events & EPOLLPRI)          ss << "EPOLLPRI ";
    if (val.events & EPOLLOUT)          ss << "EPOLLOUT ";
    if (val.events & EPOLLRDNORM)       ss << "EPOLLRDNORM ";
    if (val.events & EPOLLRDBAND)       ss << "EPOLLRDBAND ";
    if (val.events & EPOLLWRNORM)       ss << "EPOLLWRNORM ";
    if (val.events & EPOLLWRBAND)       ss << "EPOLLWRBAND ";
    if (val.events & EPOLLMSG)          ss << "EPOLLMSG ";
    if (val.events & EPOLLERR)          ss << "EPOLLERR ";
    if (val.events & EPOLLHUP)          ss << "EPOLLHUP ";
    if (val.events & EPOLLRDHUP)        ss << "EPOLLRDHUP ";
    if (val.events & EPOLLEXCLUSIVE)    ss << "EPOLLEXCLUSIVE ";
    if (val.events & EPOLLWAKEUP)       ss << "EPOLLWAKEUP ";
    if (val.events & EPOLLONESHOT)      ss << "EPOLLONESHOT ";
    if (val.events & EPOLLET)           ss << "EPOLLET ";

    return ss.str();
}





std::string to_string(ESRV_RETCODE val)
{
    std::ostringstream ss;
    to_string(val, ss);
    return ss.str();
}



void to_string(const ::Payload &pck, std::ostringstream &ss)
{
    ss  << "len : " << pck._len  << ", " << "payload : " << pck._packet;
}



std::string to_string(const ::Payload &pck)
{
    std::ostringstream ss;
    to_string(pck, ss);
    return ss.str();
}
