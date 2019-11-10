#ifndef MSGUTIL_H
#define MSGUTIL_H

#include <QDebug>

#include "ctrlmsg.h"
#include "uniformlabel.h"

class MsgUtil
{
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
    CtrlMsg createP2PHolePackage(ClientNode & client);
};

#endif // MSGUTIL_H
