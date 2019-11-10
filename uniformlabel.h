#ifndef UNIFORMLABEL_H
#define UNIFORMLABEL_H

#include <QString>

/**
 * @brief The UDPCtrlMsgType enum
 * LOGIN                客户端信息注册到服务器
 * LOGOUT               服务器上客户端信息清空
 * RENAME               客户端设置的主机名不允许重复
 * LOGINSUCCESS         客户端登陆成功
 * LOGINFAILURE         客户端登陆失败
 * LOGOUTSUCCESS        客户端登出成功
 * LOGOUTFAILURE        客户端登出失败
 * OBTAINALLPARTNERS    客户端申请获取伙伴客户端信息
 * RETURNALLPARTNERS    服务器返回伙伴客户端信息
 * P2PTRANS             请求服务器“打洞”
 * P2PHOLEPACKAGE       服务器向客户端发送，要求此客户端发送UDP打洞包
 */
enum class UDPCtrlMsgType : qint8 { LOGIN, LOGOUT, RENAME, LOGINSUCCESS, LOGINFAILURE, LOGOUTSUCCESS, LOGOUTFAILURE, OBTAINALLPARTNERS, RETURNALLPARTNERS, P2PTRANS, P2PNEEDHOLE };

/**
 * @brief The clientNode struct
 * 服务器记录客户端信息的结构体
 */
struct clientNode {
    QString name;
    QString pwd;
    QString ip;
    quint16 port;
    quint16 udpPort;
};

typedef struct clientNode ClientNode;

/**
 * @brief CommMsg CtrlMsg FileMsg
 * 各类消息json格式传送时的key值
 */
const QString MSGTYPE = "msgType";
const QString HOSTNAME = "hostName";
const QString PARTNERNAME = "partnerName";
const QString PARTNERVECTOR = "partnerVector";
const QString PWD = "pwd";
const QString IP = "ip";
const QString PORT = "port";
const QString UDPPORT = "udpPort";
const QString FRIEND = "friend";

#endif // UNIFORMLABEL_H
