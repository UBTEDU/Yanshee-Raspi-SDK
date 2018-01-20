#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <errno.h>
#include <pthread.h>
#include <fcntl.h>
#include <syslog.h>

#include "log_pub.h"

#define MAX_TIME_BUF 20
#define MAX_LOGPARA_BUF 256
#define MAX_FMT_BUF 256
#define MAX_TASKNAME_BUF 64
static ELogPara s_eLogPara = L_PARA_NULL;


#define _DEBUG_            /*must be comment when produced production*/





typedef struct _code {
    unsigned int   uluser_code;
    unsigned int   ulsys_code;
}TCODE;


static char* GetLogTimeStamp() {
    static char scTimeBuf[MAX_TIME_BUF];
    time_t   now;         //实例化time_t结构
    struct   tm     *timenow;         //实例化tm结构指针
    time(&now); 

    timenow   =   localtime(&now);

    snprintf(scTimeBuf, sizeof(scTimeBuf), "%04d/%02d/%02d/%02d/%02d/%02d",
             (timenow->tm_year+1900),
             timenow->tm_mon,
             timenow->tm_mday,
             timenow->tm_hour,
             timenow->tm_min,
             timenow->tm_sec
             );
    return scTimeBuf;

#if 0
    TSalSysTime tTime;
    static char scTimeBuf[MAX_TIME_BUF];
    SAL_TimeGet(&tTime);
    snprintf(scTimeBuf, sizeof(scTimeBuf), "%04d%02d%02d%02d%02d%02d",
             (int)tTime.year,
             (int)tTime.month,
             (int)tTime.date,
             (int)tTime.hour,
             (int)tTime.minute,
             (int)tTime.second
             );
    return scTimeBuf;
#endif
}

TCODE facility[] =
{
    { LOG_CONSOLE, LOG_LOCAL0 },
    { LOG_FILE,    LOG_LOCAL1 },
    { LOG_TV,      LOG_LOCAL2 },
    { LOG_NET,     LOG_LOCAL3 },
    { 0, 0 }
};

void _ERR(EFacility ulFacility, char *pcFormat, ...);

void SAL_ASSERT(int exp) {
#ifdef _DEBUG_
    if (!(exp)) {
        _ERR(LOG_DEFAULT, "Fail to SAL_ASSERT in Exp \r\n FILE:%s:%d",
             __FILE__, __LINE__);
        exit(0);
    }
#else
    if (!(exp)) {
        _ERR(LOG_DEFAULT, "Fail to SAL_ASSERT in Exp \r\n FILE:%s:%d",
             __FILE__, __LINE__);
    }
#endif
}

void SAL_SetLogPara(ELogPara para) {
    s_eLogPara = para;
}

static unsigned char* PrintLogPara(unsigned int ulPri) {
    static char szParaBuf[MAX_LOGPARA_BUF];
    //char szTaskName[MAX_TASKNAME_BUF];
    int slCount = 0;
    unsigned long int ulpthread_id = 0;
    if (s_eLogPara == L_PARA_NULL || s_eLogPara & ~L_PARA_ALL) {
        return NULL;
    }
    if (s_eLogPara & L_PARA_TS) {
        slCount = (int)snprintf(szParaBuf, sizeof(szParaBuf), "[%s]", GetLogTimeStamp());
    }
    if (s_eLogPara & L_PARA_PID) {
        slCount += (int)snprintf(szParaBuf + slCount, (int)sizeof(szParaBuf) - slCount, "[%d]", getpid());
    }
    if (s_eLogPara & L_PARA_TSKNM) {
        //Shawn set it task self id
        ulpthread_id = (unsigned long int)(pthread_self());
        slCount += (int)snprintf(szParaBuf + slCount,
                                 (int)sizeof(szParaBuf) - slCount, "[%lu]", ulpthread_id);

    }
    if (s_eLogPara & L_PARA_PRI) {
        slCount += (int)snprintf(szParaBuf + slCount, sizeof(szParaBuf) - slCount, "[%d]", ulPri);
    }

    return (unsigned char *)szParaBuf;
}

unsigned int  SAL_SetLogMask(ELogType uLogMask) {
    int priorty;

    switch ( uLogMask ) {
    case L_ERR:
        priorty = LOG_ERR;
        break;

    case L_INFO:
        //priorty = LOG_MASK(LOG_INFO);
        priorty = LOG_UPTO(LOG_INFO);
        break;

    case L_DBG:
        priorty = LOG_MASK(LOG_DEBUG);
        break;

    case L_FATAL:
        priorty = LOG_MASK(LOG_EMERG);
        break;

    case L_CRITIC:
        priorty = LOG_MASK(LOG_CRIT);
        break;

    case L_ALERT:
        priorty = LOG_MASK(LOG_ALERT);
        break;

    case L_WARN:
        priorty = LOG_MASK(LOG_WARNING);
        break;

    case L_ALL:
        priorty = LOG_UPTO(LOG_DEBUG);
        break;

    default:
        priorty = LOG_UPTO(LOG_DEBUG);
    }

    return (unsigned int)setlogmask((int)priorty);
}



void SAL_OpenLog(char *pident, ELogOpt opt, EFacility faci) {
    unsigned int sysOpt = 0;
    TCODE *ptfac;

    if (opt & L_OPTION_PERROR) {
        sysOpt |= LOG_PERROR;
    }

    if (opt & L_OPTION_CONSOLE) {
        sysOpt |= LOG_CONS;
    }


    for (ptfac = facility; ptfac->uluser_code != 0; ptfac++) {
        if (faci & ptfac->uluser_code) {
            openlog(pident, (int)sysOpt, (int)ptfac->ulsys_code);
            break;
        }
    }
}

void SAL_CloseLog(void) {
    closelog();
}


void SAL_DBG(EFacility ulFacility, char *pcFormat, ...) {
#ifdef _DEBUG_
    TCODE *ptfac;
    unsigned char *ucLogPara;
    char ucFmt[MAX_FMT_BUF];

    va_list pVar;
    va_start(pVar, pcFormat);

    vsnprintf(ucFmt, sizeof(ucFmt), pcFormat, pVar);
    ucLogPara = PrintLogPara(LOG_DEBUG);

    if (ulFacility == LOG_DEFAULT) {
        if (ucLogPara != NULL) {
            syslog(LOG_DEBUG, "%s:%s", ucLogPara, ucFmt);
        } else {
            syslog(LOG_DEBUG, "%s", ucFmt);
        }
    } else {
        for (ptfac = facility; ptfac->uluser_code != 0; ptfac++) {
            if (ulFacility & ptfac->uluser_code) {
                if (ucLogPara != NULL) {
                    syslog((int)LOG_DEBUG | ptfac->ulsys_code, "%s:%s", ucLogPara, ucFmt);
                } else {
                    syslog((int)LOG_DEBUG | ptfac->ulsys_code, "%s", ucFmt);
                }
                break;
            }
        }
    }

    va_end(pVar);
#else
#endif
}

void SAL_INF(EFacility ulFacility, char *pcFormat, ...) {
#ifdef _DEBUG_
    TCODE *ptfac;
    unsigned char *ucLogPara;
    char ucFmt[MAX_FMT_BUF];

    va_list pVar;
    va_start(pVar, pcFormat);

    vsnprintf(ucFmt, sizeof(ucFmt), pcFormat, pVar);
    ucLogPara = PrintLogPara(LOG_INFO);

    if (ulFacility == LOG_DEFAULT) {
        if (ucLogPara != NULL) {
            syslog(LOG_INFO, "%s:%s", ucLogPara, ucFmt);
        } else {
            syslog(LOG_INFO, "%s", ucFmt);
        }
    } else {
        for (ptfac = facility; ptfac->uluser_code != 0; ptfac++) {
            if (ulFacility & ptfac->uluser_code) {
                if (ucLogPara != NULL) {
                    syslog(LOG_INFO | ptfac->ulsys_code, "%s:%s", ucLogPara, ucFmt);
                } else {
                    syslog(LOG_INFO | ptfac->ulsys_code, "%s", ucFmt);
                }
                break;
            }
        }
    }

    va_end(pVar);
#else
#endif
}

void SAL_TRC(EFacility ulFacility, char *pcFormat, ...) {
    TCODE *ptfac;
    unsigned char *ucLogPara = NULL;
    char ucFmt[MAX_FMT_BUF];

    va_list pVar;
    va_start(pVar, pcFormat);

    vsnprintf(ucFmt, sizeof(ucFmt), pcFormat, pVar);
    ucLogPara = PrintLogPara(LOG_NOTICE);

    if (ulFacility == LOG_DEFAULT) {
        if (ucLogPara != NULL) {
            syslog(LOG_NOTICE, "%s:%s", ucLogPara, ucFmt);
        } else {
            syslog(LOG_NOTICE, "%s", ucFmt);
        }
    } else {
        for (ptfac = facility; ptfac->uluser_code != 0; ptfac++) {
            if (ulFacility & ptfac->uluser_code) {
                if (ucLogPara != NULL) {
                    syslog(LOG_NOTICE | ptfac->ulsys_code, "%s:%s", ucLogPara, ucFmt);
                } else {
                    syslog(LOG_NOTICE | ptfac->ulsys_code, "%s", ucFmt);
                }
                break;
            }
        }
    }

    va_end(pVar);

}


void SAL_WRN(EFacility ulFacility, char *pcFormat, ...) {
    TCODE *ptfac;
    unsigned char *ucLogPara;
    char ucFmt[MAX_FMT_BUF];

    va_list pVar;
    va_start(pVar, pcFormat);

    vsnprintf(ucFmt, sizeof(ucFmt), pcFormat, pVar);
    ucLogPara = PrintLogPara(LOG_WARNING);

    if (ulFacility == LOG_DEFAULT) {
        if (ucLogPara != NULL) {
            syslog(LOG_WARNING, "%s:%s", ucLogPara, ucFmt);
        } else {
            syslog(LOG_WARNING, "%s", ucFmt);
        }
    } else {
        for (ptfac = facility; ptfac->uluser_code != 0; ptfac++) {
            if (ulFacility & ptfac->uluser_code) {
                if (ucLogPara != NULL) {
                    syslog(LOG_WARNING | ptfac->ulsys_code, "%s:%s", ucLogPara, ucFmt);
                } else {
                    syslog(LOG_WARNING | ptfac->ulsys_code, "%s", ucFmt);
                }
                break;
            }
        }
    }

    va_end(pVar);
}


void SAL_ERR(EFacility ulFacility, char *pcFormat, ...) {
    TCODE *ptfac;
    unsigned char *ucLogPara;
    char ucFmt[MAX_FMT_BUF];

    va_list pVar;
    va_start(pVar, pcFormat);

    vsnprintf(ucFmt, sizeof(ucFmt), pcFormat, pVar);
    ucLogPara = PrintLogPara(LOG_ERR);

    if (ulFacility == LOG_DEFAULT) {
        if (ucLogPara != NULL) {
            syslog(LOG_ERR, "%s:%s", ucLogPara, ucFmt);
        } else {
            syslog(LOG_ERR, "%s", ucFmt);
        }
    } else {
        for (ptfac = facility; ptfac->uluser_code != 0; ptfac++) {
            if (ulFacility & ptfac->uluser_code) {
                if (ucLogPara != NULL) {
                    syslog(LOG_ERR | ptfac->ulsys_code, "%s:%s", ucLogPara, ucFmt);
                } else {
                    syslog(LOG_ERR | ptfac->ulsys_code, "%s", ucFmt);
                }
                break;
            }
        }
    }

    va_end(pVar);
}


void SAL_ALERT(EFacility ulFacility, char *pcFormat, ...) {
    TCODE *ptfac;
    unsigned char *ucLogPara;
    char ucFmt[MAX_FMT_BUF];

    va_list pVar;
    va_start(pVar, pcFormat);

    vsnprintf(ucFmt, sizeof(ucFmt), pcFormat, pVar);
    ucLogPara = PrintLogPara(LOG_ALERT);

    if (ulFacility == LOG_DEFAULT) {
        if (ucLogPara != NULL) {
            syslog(LOG_ALERT, "%s:%s", ucLogPara, ucFmt);
        } else {
            syslog(LOG_ALERT, "%s", ucFmt);
        }
    } else {
        for (ptfac = facility; ptfac->uluser_code != 0; ptfac++) {
            if (ulFacility & ptfac->uluser_code) {
                if (ucLogPara != NULL) {
                    syslog(LOG_ALERT | ptfac->ulsys_code, "%s:%s", ucLogPara, ucFmt);
                } else {
                    syslog(LOG_ALERT | ptfac->ulsys_code, "%s", ucFmt);
                }
                break;
            }
        }
    }

    va_end(pVar);
}


void _DBG(EFacility ulFacility, char *pcFormat, ...) {
#ifdef _DEBUG_
    TCODE *ptfac;
    unsigned char *ucLogPara;
    char ucFmt[MAX_FMT_BUF];

    va_list pVar;
    va_start(pVar, pcFormat);

    vsnprintf(ucFmt, sizeof(ucFmt), pcFormat, pVar);
    ucLogPara = PrintLogPara(LOG_DEBUG);

    if (ulFacility == LOG_DEFAULT) {
        if (ucLogPara != NULL) {
            syslog(LOG_DEBUG, "%s:%s", ucLogPara, ucFmt);
        } else {
            syslog(LOG_DEBUG, "%s", ucFmt);
        }
    } else {
        for (ptfac = facility; ptfac->uluser_code != 0; ptfac++) {
            if (ulFacility & ptfac->uluser_code) {
                if (ucLogPara != NULL) {
                    syslog((int)LOG_DEBUG | ptfac->ulsys_code, "%s:%s", ucLogPara, ucFmt);
                } else {
                    syslog((int)LOG_DEBUG | ptfac->ulsys_code, "%s", ucFmt);
                }
                break;
            }
        }
    }

    va_end(pVar);
#else
#endif
}

void _INF(EFacility ulFacility, char *pcFormat, ...) {
#ifdef _DEBUG_
    TCODE *ptfac;
    unsigned char *ucLogPara;
    char ucFmt[MAX_FMT_BUF];

    va_list pVar;
    va_start(pVar, pcFormat);

    vsnprintf(ucFmt, sizeof(ucFmt), pcFormat, pVar);
    ucLogPara = PrintLogPara(LOG_INFO);

    if (ulFacility == LOG_DEFAULT) {
        if (ucLogPara != NULL) {
            syslog(LOG_INFO, "%s:%s", ucLogPara, ucFmt);
        } else {
            syslog(LOG_INFO, "%s", ucFmt);
        }
    } else {
        for (ptfac = facility; ptfac->uluser_code != 0; ptfac++) {
            if (ulFacility & ptfac->uluser_code) {
                if (ucLogPara != NULL) {
                    syslog(LOG_INFO | ptfac->ulsys_code, "%s:%s", ucLogPara, ucFmt);
                } else {
                    syslog(LOG_INFO | ptfac->ulsys_code, "%s", ucFmt);
                }
                break;
            }
        }
    }

    va_end(pVar);
#else
#endif
}

void _TRC(EFacility ulFacility, char *pcFormat, ...) {
    TCODE *ptfac;
    unsigned char *ucLogPara = NULL;
    char ucFmt[MAX_FMT_BUF];

    va_list pVar;
    va_start(pVar, pcFormat);

    vsnprintf(ucFmt, sizeof(ucFmt), pcFormat, pVar);
    ucLogPara = PrintLogPara(LOG_NOTICE);

    if (ulFacility == LOG_DEFAULT) {
        if (ucLogPara != NULL) {
            syslog(LOG_NOTICE, "%s:%s", ucLogPara, ucFmt);
        } else {
            syslog(LOG_NOTICE, "%s", ucFmt);
        }
    } else {
        for (ptfac = facility; ptfac->uluser_code != 0; ptfac++) {
            if (ulFacility & ptfac->uluser_code) {
                if (ucLogPara != NULL) {
                    syslog(LOG_NOTICE | ptfac->ulsys_code, "%s:%s", ucLogPara, ucFmt);
                } else {
                    syslog(LOG_NOTICE | ptfac->ulsys_code, "%s", ucFmt);
                }
                break;
            }
        }
    }

    va_end(pVar);

}


void _WRN(EFacility ulFacility, char *pcFormat, ...) {
    TCODE *ptfac;
    unsigned char *ucLogPara;
    char ucFmt[MAX_FMT_BUF];

    va_list pVar;
    va_start(pVar, pcFormat);

    vsnprintf(ucFmt, sizeof(ucFmt), pcFormat, pVar);
    ucLogPara = PrintLogPara(LOG_WARNING);

    if (ulFacility == LOG_DEFAULT) {
        if (ucLogPara != NULL) {
            syslog(LOG_WARNING, "%s:%s", ucLogPara, ucFmt);
        } else {
            syslog(LOG_WARNING, "%s", ucFmt);
        }
    } else {
        for (ptfac = facility; ptfac->uluser_code != 0; ptfac++) {
            if (ulFacility & ptfac->uluser_code) {
                if (ucLogPara != NULL) {
                    syslog(LOG_WARNING | ptfac->ulsys_code, "%s:%s", ucLogPara, ucFmt);
                } else {
                    syslog(LOG_WARNING | ptfac->ulsys_code, "%s", ucFmt);
                }
                break;
            }
        }
    }

    va_end(pVar);
}


void _ERR(EFacility ulFacility, char *pcFormat, ...) {
    TCODE *ptfac;
    unsigned char *ucLogPara;
    char ucFmt[MAX_FMT_BUF];

    va_list pVar;
    va_start(pVar, pcFormat);

    vsnprintf(ucFmt, sizeof(ucFmt), pcFormat, pVar);
    ucLogPara = PrintLogPara(LOG_ERR);

    if (ulFacility == LOG_DEFAULT) {
        if (ucLogPara != NULL) {
            syslog(LOG_ERR, "%s:%s", ucLogPara, ucFmt);
        } else {
            syslog(LOG_ERR, "%s", ucFmt);
        }
    } else {
        for (ptfac = facility; ptfac->uluser_code != 0; ptfac++) {
            if (ulFacility & ptfac->uluser_code) {
                if (ucLogPara != NULL) {
                    syslog(LOG_ERR | ptfac->ulsys_code, "%s:%s", ucLogPara, ucFmt);
                } else {
                    syslog(LOG_ERR | ptfac->ulsys_code, "%s", ucFmt);
                }
                break;
            }
        }
    }

    va_end(pVar);
}


void _ALERT(EFacility ulFacility, char *pcFormat, ...) {
    TCODE *ptfac;
    unsigned char *ucLogPara;
    char ucFmt[MAX_FMT_BUF];

    va_list pVar;
    va_start(pVar, pcFormat);

    vsnprintf(ucFmt, sizeof(ucFmt), pcFormat, pVar);
    ucLogPara = PrintLogPara(LOG_ALERT);

    if (ulFacility == LOG_DEFAULT) {
        if (ucLogPara != NULL) {
            syslog(LOG_ALERT, "%s:%s", ucLogPara, ucFmt);
        } else {
            syslog(LOG_ALERT, "%s", ucFmt);
        }
    } else {
        for (ptfac = facility; ptfac->uluser_code != 0; ptfac++) {
            if (ulFacility & ptfac->uluser_code) {
                if (ucLogPara != NULL) {
                    syslog(LOG_ALERT | ptfac->ulsys_code, "%s:%s", ucLogPara, ucFmt);
                } else {
                    syslog(LOG_ALERT | ptfac->ulsys_code, "%s", ucFmt);
                }
                break;
            }
        }
    }

    va_end(pVar);
}


