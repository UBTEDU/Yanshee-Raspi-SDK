#ifndef _SAL_UTIL_H_
#define _SAL_UTIL_H_

#ifdef  __cplusplus
extern "C"
{
#endif

#define MAX_LOG_LEN 256 /* log maximum length */

typedef enum EFacility
{
    LOG_DEFAULT = 0x00,
    LOG_CONSOLE = 0x01,
    LOG_FILE    = 0x02,
    LOG_TV      = 0x04,
    LOG_NET     = 0x08
}EFacility;

typedef enum ELogPara
{
    L_PARA_NULL         = 0x00,
    L_PARA_TS           = 0x01,
    L_PARA_PID          = 0x02,
    L_PARA_TSKNM        = 0x04,
    L_PARA_PRI          = 0x08,
    L_PARA_ALL          = 0x0f
}ELogPara;




/* ---------- typedefs -------------------- */

typedef enum
{
    L_FATAL = 1,
    L_CRITIC = 2,
    L_ALERT = 4,
    L_WARN = 8,
    L_ERR = 16,
    L_INFO = 32,
    L_DBG = 64,
    L_LOG = 128,
    L_ALL = 256
}ELogType;

typedef enum
{
    L_OPTION_PERROR = 0x01, /*write log to stderr*/
    L_OPTION_CONSOLE = 0x02, /*write log to system console*/
}ELogOpt;


/* ----------- Functions declarations ----------------- */

/*pident:give each mesage a prefix*/

/*EFacility dft:establishes a default to be used 
if none is specified in subsequent calls to syslog()*/
extern void SAL_OpenLog(char *pident, ELogOpt option, EFacility dft);
extern void SAL_CloseLog(void);


extern unsigned int SAL_SetLogMask(ELogType usNewLogMask);
extern void SAL_SetLogPara(ELogPara para);

extern void SAL_DBG(EFacility ulFacility, char *pcFormat, ...);
extern void SAL_INF(EFacility ulFacility, char *pcFormat, ...);
extern void SAL_TRC(EFacility ulFacility, char *pcFormat, ...);
extern void SAL_WRN(EFacility ulFacility, char *pcFormat, ...);
extern void SAL_ERR(EFacility ulFacility, char *pcFormat, ...);
extern void SAL_ALERT(EFacility ulFacility, char *pcFormat, ...);
extern void SAL_ASSERT(int exp);


#ifdef  __cplusplus
}
#endif

#endif

