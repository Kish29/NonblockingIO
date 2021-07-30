//
// Created by 蒋澳然 on 2021/7/28.
// email: 875691208@qq.com
// $desc
//

#include "httpcode_map.h"

std::mutex httpcode_map::mutex_{};
httpcode_map *httpcode_map::instance_{};

httpcode_map::httpcode_map() {
    // see https://developer.mozilla.org/en-US/docs/Web/HTTP/Status#client_error_responses
    code_map_.insert(std::make_pair(100, "Continue"));
    code_map_.insert(std::make_pair(101, "Switching Protocols"));
    code_map_.insert(std::make_pair(102, "Processing"));
    code_map_.insert(std::make_pair(103, "Early Hints"));
    code_map_.insert(std::make_pair(200, "OK"));
    code_map_.insert(std::make_pair(201, "Created"));
    code_map_.insert(std::make_pair(202, "Accepted"));
    code_map_.insert(std::make_pair(203, "Non-Authoritative Information"));
    code_map_.insert(std::make_pair(204, "No Content"));
    code_map_.insert(std::make_pair(205, "Reset Content"));
    code_map_.insert(std::make_pair(206, "Partial Content"));
    code_map_.insert(std::make_pair(300, "Multiple Choices"));
    code_map_.insert(std::make_pair(301, "Moved Permanently"));
    code_map_.insert(std::make_pair(302, "Found"));
    code_map_.insert(std::make_pair(303, "See Other"));
    code_map_.insert(std::make_pair(304, "Not Modified"));
    code_map_.insert(std::make_pair(305, "Use Proxy"));
    code_map_.insert(std::make_pair(307, "Temporary Redirect"));
    code_map_.insert(std::make_pair(308, "Permanent Redirect"));
    code_map_.insert(std::make_pair(400, "Bad Request"));
    code_map_.insert(std::make_pair(401, "Unauthorized"));
    code_map_.insert(std::make_pair(402, "Payment Required"));
    code_map_.insert(std::make_pair(403, "Forbidden"));
    code_map_.insert(std::make_pair(404, "Not Found"));
    code_map_.insert(std::make_pair(405, "Method Not Allowed"));
    code_map_.insert(std::make_pair(406, "Not Acceptable"));
    code_map_.insert(std::make_pair(407, "Proxy Authentication Required"));
    code_map_.insert(std::make_pair(408, "Request Timeout"));
    code_map_.insert(std::make_pair(409, "Conflict"));
    code_map_.insert(std::make_pair(410, "Gone"));
    code_map_.insert(std::make_pair(411, "Length Required"));
    // ...

    code_map_.insert(std::make_pair(500, "Internal Server Error"));
    code_map_.insert(std::make_pair(501, "Not Implemented"));
    code_map_.insert(std::make_pair(502, "Bad Gateway"));
    code_map_.insert(std::make_pair(503, "Service Unavailable"));
    code_map_.insert(std::make_pair(504, "Gateway Timeout"));
    code_map_.insert(std::make_pair(505, "HTTP Version Not Supported"));
    code_map_.insert(std::make_pair(506, "Variant Also Negotiates"));
    code_map_.insert(std::make_pair(507, "Insufficient Storage"));
    code_map_.insert(std::make_pair(508, "Loop Detected"));
    code_map_.insert(std::make_pair(510, "Not Extended"));
    code_map_.insert(std::make_pair(511, "Network Authentication Required"));
}
