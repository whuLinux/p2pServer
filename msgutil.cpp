#include "msgutil.h"

MsgUtil::MsgUtil()
{

}

CtrlMsg MsgUtil::createRenameMsg()
{
    CtrlMsg renameMsg(UDPCtrlMsgType::RENAME);

    return renameMsg;
}

CtrlMsg MsgUtil::createLoginSuccessMsg()
{
    CtrlMsg successMsg(UDPCtrlMsgType::LOGINSUCCESS);

    return successMsg;
}

CtrlMsg MsgUtil::createLoginFailureMsg()
{
    CtrlMsg failureMsg(UDPCtrlMsgType::LOGINFAILURE);

    return failureMsg;
}

CtrlMsg MsgUtil::createLogoutSuccessMsg()
{
    CtrlMsg successMsg(UDPCtrlMsgType::LOGOUTSUCCESS);

    return successMsg;
}

CtrlMsg MsgUtil::createLogoutFailureMsg()
{
    CtrlMsg failureMsg(UDPCtrlMsgType::LOGOUTFAILURE);

    return failureMsg;
}

CtrlMsg MsgUtil::createReturnAllPartners(QVector<ClientNode> & clients)
{
    CtrlMsg returnMsg(UDPCtrlMsgType::RETURNALLPARTNERS);

    if (clients.empty()) {
        qDebug() << "伙伴客户端信息列表有误" << endl;
        return returnMsg;
    }

    returnMsg.setClients(clients);

    return returnMsg;
}

CtrlMsg MsgUtil::createObtainSuccessMsg()
{
    CtrlMsg successMsg(UDPCtrlMsgType::OBTAINSUCCESS);

    return successMsg;
}

CtrlMsg MsgUtil::createObtainFailureMsg()
{
    CtrlMsg failureMsg(UDPCtrlMsgType::OBTAINFAILURE);

    return failureMsg;
}

CtrlMsg MsgUtil::createP2PHolePackage(ClientNode & client)
{
    CtrlMsg holeMsg(UDPCtrlMsgType::P2PNEEDHOLE);
    holeMsg.addClient(client);

    return holeMsg;
}
