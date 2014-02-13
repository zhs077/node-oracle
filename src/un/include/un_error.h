#ifndef _ERROR_H_
#define  _ERROR_H_
void un_get_error(const int error_no, const char* err_msg);

#define  UN_ENOMEM -10000
#define  UN_EAGAIN -10001
#define  UV_EBUSY  -10002
#define  UN_EINVAL -10003
#define  UN_ETIMEDOUT -10004
#endif
