#ifndef MSGUTIL_H
#define MSGUTIL_H

#include <QObject>
#include <QDebug>

#include "ctrlmsg.h"
#include "uniformlabel.h"

/**
 * @brief The MsgUtil class
 * @author 余宗宪
 * 生成控制信息的工具类
 */
class MsgUtil : public QObject
{
    Q_OBJECT
public:
    MsgUtil();

    /**
     * @brief createCtrlMsg
     */
    CtrlMsg createRenameMsg();
    CtrlMsg createLoginSuccessMsg();
    CtrlMsg createLoginFailureMsg();
    CtrlMsg createLogoutSuccessMsg();
    CtrlMsg createLogoutFailureMsg();
    CtrlMsg createReturnAllPartners(QVector<ClientNode> & clients);
    CtrlMsg createObtainSuccessMsg();
    CtrlMsg createObtainFailureMsg();
    CtrlMsg createP2PHolePackage(ClientNode & client);
};

#endif // MSGUTIL_H
