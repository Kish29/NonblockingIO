//
// Created by 蒋澳然 on 2021/7/28.
// email: 875691208@qq.com
// $desc
//

#ifndef EPOLLLEARN_HTTP_PARSER_H
#define EPOLLLEARN_HTTP_PARSER_H

#include "no_copyable.h"
#include "httpcode_map.h"
#include "string"

// http的每一行末尾都是由 回车符CR和换行符LF组成的
// 回车
#define CR      '\r'
// 换行
#define LF      '\n'
#define CRLF    "\r\n"
#define SPACE   ' '
// 冒号
#define COLON   ':'

const char *const HTTP_VERSION = "HTTP/1.1";

// 除了数据可以为空之外，响应(请求)行和首部不能为空
// http请求报文结构
/*          +--------+-------+-----+-------+---------+------+
 * 请求行-> ｜ method | space | URL | space | version | CRLF |
 *         +--------+-------+------+----+------------------+
 *首部行1-> | 字段名 | colon | space | 值 | CRLF |
 *首部行2-> | 字段名 | colon | space | 值 | CRLF |
 *首部行3-> | 字段名 | colon | space | 值 | CRLF |
 *        +-----------------------------------+
 * CRLF   +---------------------------------+
 * 数据-> |**********************************|
 * */
struct request_line {
    std::string method{};
    std::string url{};
    std::string version{};
};

// http响应报文结构
/*          +---------+-------+-------------+-------+-----------+------+
 * 响应行-> ｜ version | space | status_code | space | short_msg | CRLF |
 *         +----------------------------------------------------------+
 *首部行1-> | 字段名 | colon | space | 值 | CRLF |
 *首部行2-> | 字段名 | colon | space | 值 | CRLF |
 *首部行3-> | 字段名 | colon | space | 值 | CRLF |
 *        +-----------------------------------+
 * CRLF   +---------------------------------+
 * 数据-> |**********************************|
 * */

struct response_line {
    std::string version{};
    int status_code{};
    std::string short_msg{};
};

class http_parser : no_copyable {
public:

private:
};


#endif //EPOLLLEARN_HTTP_PARSER_H
