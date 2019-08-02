#include "3ds/services/cecdu.h"
#include <3ds/svc.h>
#include <3ds/srv.h>
#include <3ds/synchronization.h>
#include <3ds/ipc.h>

static Handle cecduHandle;
static int cecduRefCount;

Result cecduInit(void)
{
    Result ret;

    if (AtomicPostIncrement(&cecduRefCount)) return 0;

    ret = srvGetServiceHandle(&cecduHandle, "cecd:u");
    if (R_FAILED(ret)) AtomicDecrement(&cecduRefCount);

    return ret;
}

void cecduExit(void)
{
	if (AtomicDecrement(&cecduRefCount)) return;
	svcCloseHandle(cecduHandle);
}

Result CECDU_Open(u32 programID, CecDataPathType path, u32 flag, u32* size)
{
    Result ret;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = IPC_MakeHeader(0x1, 3, 2);
    cmdbuf[1] = programID;
    cmdbuf[2] = path;
    cmdbuf[3] = flag;
    cmdbuf[4] = IPC_Desc_CurProcessId(); // do placeholder
    cmdbuf[5] = 0; // placeholder

    if (R_FAILED(ret = svcSendSyncRequest(cecduHandle))) return ret;
    ret = (Result)cmdbuf[1];

    if(R_SUCCEEDED(ret))
    {
        if(size)*size = cmdbuf[2];
    }
    return ret;
}

Result CECDU_Read(u32 bufferSize, void* buffer, u32* readSize)
{
    Result ret;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = IPC_MakeHeader(0x2, 1, 2);
    cmdbuf[1] = bufferSize;
    cmdbuf[2] = IPC_Desc_Buffer(bufferSize, IPC_BUFFER_W);
    cmdbuf[3] = (u32)buffer;

    if (R_FAILED(ret = svcSendSyncRequest(cecduHandle))) return ret;
    ret = (Result)cmdbuf[1];

    if(R_SUCCEEDED(ret))
    {
        if(readSize)*readSize = cmdbuf[2];
    }
    return ret;
}

Result CECDU_ReadMessage(u32 programID, bool outBox, u32 idSize, u32 bufferSize, const void* messageID, void* buffer, u32* readSize)
{
    Result ret;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = IPC_MakeHeader(0x3, 4, 4);
    cmdbuf[1] = programID;
    cmdbuf[2] = outBox;
    cmdbuf[3] = idSize;
    cmdbuf[4] = bufferSize;
    cmdbuf[5] = IPC_Desc_Buffer(idSize, IPC_BUFFER_R);
    cmdbuf[6] = (u32)messageID;
    cmdbuf[7] = IPC_Desc_Buffer(bufferSize, IPC_BUFFER_W);
    cmdbuf[8] = (u32)buffer;

    if (R_FAILED(ret = svcSendSyncRequest(cecduHandle))) return ret;
    ret = (Result)cmdbuf[1];

    if(R_SUCCEEDED(ret))
    {
        if(readSize)*readSize = cmdbuf[2];
    }
    return ret;
}

Result CECDU_ReadMessageWithHMAC(u32 programID, bool outBox, u32 idSize, u32 bufferSize, const void* messageID, const void* hmacKey, void* buffer, u32* readSize)
{
    Result ret;
    u32* cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = IPC_MakeHeader(0x4, 4, 6);
    cmdbuf[1] = programID;
    cmdbuf[2] = outBox;
    cmdbuf[3] = idSize;
    cmdbuf[4] = bufferSize;
    cmdbuf[5] = IPC_Desc_Buffer(idSize, IPC_BUFFER_R);
    cmdbuf[6] = (u32)messageID;
    cmdbuf[7] = IPC_Desc_Buffer(0x20, IPC_BUFFER_R); // 32 byte key size
    cmdbuf[8] = (u32)hmacKey;
    cmdbuf[9] = IPC_Desc_Buffer(bufferSize, IPC_BUFFER_W);
    cmdbuf[10] = (u32)buffer;

    if (R_FAILED(ret = svcSendSyncRequest(cecduHandle))) return ret;
    ret = (Result)cmdbuf[1];

    if(R_SUCCEEDED(ret))
    {
        if(readSize)*readSize = cmdbuf[2];
    }
    return ret;
}