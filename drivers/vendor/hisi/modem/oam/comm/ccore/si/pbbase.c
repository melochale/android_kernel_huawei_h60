/************************************************************************
  Copyright    : 2005-2007, Huawei Tech. Co., Ltd.
  File name    : UsimmApi.c
  Author       : zhuli
  Version      : V100R002
  Date         : 2008-5-15
  Description  : ��C�ļ�������---��ɳ�ʼ������Ϣ����ģ��ʵ��
  Function List:
  History      :
 ************************************************************************/

/*lint  --e{537,767,718,746,740,958,701,553,516,451}*/

#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif
#endif

#include "si_pb.h"
#include "UsimmApi.h"
#include "TafOamInterface.h"
#include "omnvinterface.h"

/*****************************************************************************
    Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
*****************************************************************************/

#define    THIS_FILE_ID PS_FILE_ID_PB_BASE_C


VOS_UINT32  g_ulPbrRecordNum = 0;


VOS_BOOL SI_PB_CheckSupportAP(VOS_VOID)
{
    NAS_NVIM_SYSTEM_APP_CONFIG_STRU     stAPPConfig;

    if (NV_OK != NV_Read(en_NV_Item_System_APP_Config, &stAPPConfig, sizeof(VOS_UINT16)))
    {
        PB_NORMAL_LOG("SI_PB_CheckSupportAP: Read en_NV_Item_System_APP_Config Failed");

        return VOS_FALSE;
    }

    if (SYSTEM_APP_ANDROID == stAPPConfig.usSysAppConfigType)
    {
        PB_NORMAL_LOG("SI_PB_CheckSupportAP: System App is Android");

        return VOS_TRUE;
    }

    PB_NORMAL_LOG("SI_PB_CheckSupportAP: System App is other");

    return VOS_FALSE;
}



VOS_VOID SI_PB_ClearPBContent(VOS_UINT32 ulClearType)
{
    VOS_UINT32 i;

    /*�п���ADN��MSISDNָ��ͬһ���ڴ棬�����ظ��ͷ�*/
    if(gastEXTContent[PB_ADN_CONTENT].pExtContent ==
                gastEXTContent[PB_MSISDN_CONTENT].pExtContent)
    {
        gastEXTContent[PB_MSISDN_CONTENT].pExtContent = VOS_NULL_PTR;
    }

    for (i=0; i<SI_PB_MAX_NUMBER; i++)
    {
        if((SI_CLEARXDN == ulClearType)&&(PB_ECC == gastPBContent[i].enPBType))/*ֻ������ֵ绰������ECC*/
        {
           continue;
        }

        if(VOS_NULL_PTR != gastPBContent[i].pContent)
        {
            PB_FREE(gastPBContent[i].pContent);

            gastPBContent[i].pContent = VOS_NULL_PTR;
        }

        if (VOS_NULL_PTR != gastPBContent[i].pIndex)
        {
            PB_FREE(gastPBContent[i].pIndex);
        }

        if(VOS_NULL_PTR != gastEXTContent[i].pExtContent)
        {
            PB_FREE(gastEXTContent[i].pExtContent);
        }

        gastPBContent[i].enInitialState = PB_NOT_INITIALISED;
    }

    PB_WARNING_LOG("SI_PB_ClearPBContent: Clear The PB Content");

    return;
}


VOS_VOID SI_PB_ClearSPBContent(VOS_UINT32 ulClearType)
{
    VOS_UINT32 i;

    for (i=0; i<SI_PB_ANRMAX; i++)
    {
        if(VOS_NULL_PTR != gastANRContent[i].pContent)
        {
            PB_FREE(gastANRContent[i].pContent);

            gastANRContent[i].pContent = VOS_NULL_PTR;
        }
    }

    if(VOS_NULL_PTR != gstEMLContent.pContent)
    {
        PB_FREE(gstEMLContent.pContent);

        gstEMLContent.pContent = VOS_NULL_PTR;
    }

    if(VOS_NULL_PTR != gstIAPContent.pIAPContent)
    {
        PB_FREE(gstIAPContent.pIAPContent);
    }

    PB_WARNING_LOG("SI_PB_ClearSPBContent: Clear The SPB Content");

    return;
}


VOS_VOID SI_PB_ExtExceptionProc(VOS_UINT16 usExtFileId)
{
    VOS_UINT8   i;
    VOS_UINT32  ulType;

    PB_WARNING_LOG("SI_PB_ExtExceptionProc: Into the PB Ext File Exc Handle");

    for( i = 0 ; i < PB_MSISDN_CONTENT; i++)
    {
        if(usExtFileId == gastEXTContent[i+1].usExtFileId)
        {
            break;
        }
    }

    if(i >= PB_MSISDN_CONTENT)
    {
        return;
    }

    ulType = i + 1;

    if(VOS_NULL_PTR != gastEXTContent[ulType].pExtContent)
    {
        PB_FREE(gastEXTContent[ulType].pExtContent);
        gastEXTContent[ulType].pExtContent = VOS_NULL_PTR;
    }

    gastEXTContent[ulType].usExtFlag        = SI_PB_FILE_NOT_EXIST;
    gastEXTContent[ulType].usExtFileId      = 0xFFFF;
    gastEXTContent[ulType].usExtTotalNum    = 0;
    gastEXTContent[ulType].usExtUsedNum     = 0;

    return;
}


VOS_UINT32 SI_PB_CheckXDNFileID(VOS_UINT16 usFileID)
{
    switch(usFileID)
    {
        case EFFDN:
        case EFBDN:
        case EFMSISDN:
        case EFPSC:
        case EFCC:
        case EFPUID:
            return VOS_OK;
        default:
            return VOS_ERR;
    }
}


VOS_UINT32 SI_PB_CheckSYNCHFileID(VOS_UINT16 usFileID)
{
    switch(usFileID)
    {
        case EFPSC:
        case EFCC:
        case EFPUID:
            return VOS_OK;
        default:
            if ( usFileID == gstPBCtrlInfo.astUIDInfo[0].usFileID )
            {
                return VOS_OK;
            }
            else if ( usFileID == gstPBCtrlInfo.astPBCInfo[0].usFileID )
            {
                return VOS_OK;
            }
            else
            {
                return VOS_ERR;
            }
    }
}


VOS_UINT32 SI_PB_CheckExtFileID(VOS_UINT16 usFileID)
{
    switch(usFileID)
    {
        case EFEXT1:
        case EFEXT2:
        case EFEXT3:
        /*case EFEXT5: */
        case EFEXT6:
        case EFEXT7:
        case EFEXT4_USIM:
        case EFEXT4_SIM:
            return VOS_OK;
        default:
            break;
    }

    /* ��USIM��ADN�ļ���EXT1�ļ�ID��ͬ */
    if(usFileID == gastEXTContent[PB_ADN_CONTENT].usExtFileId)
    {
        return VOS_OK;
    }

    return VOS_ERR;
}


VOS_VOID SI_PB_JumpToNextPB(VOS_UINT32 ulPBType)
{
    if ( PB_ADN_CONTENT == ulPBType )
    {
        gulPBFileCnt = gstPBCtrlInfo.ulADNFileNum;/*����ADN�绰��*/
    }
    else
    {
        gulPBFileCnt++;
    }

    return;
}
VOS_VOID SI_PB_InitSYNCHGetProc(PBMsgBlock *pstMsg)
{
    PS_USIM_GET_FILE_CNF_STRU    *pstPBMsg;

    pstPBMsg = (PS_USIM_GET_FILE_CNF_STRU *)pstMsg;

    if ( VOS_OK != pstPBMsg->ulResult )/*���USIMMģ��ظ���������ͬ��ʧ��*/
    {
        PB_ERROR_LOG("SI_PB_InitSYNCHProc:USIMM Confirm ERROR");

        gstPBInitState.enPBInitState = SI_PB_INIT_USIMPBCNF_ERR;

        if( (EFPUID == pstPBMsg->usEfId)
            || (gstPBCtrlInfo.astPBCInfo[0].usFileID == pstPBMsg->usEfId) )
        {
            gulPBRecordCnt = 1;/*���濪ʼ��ʼ��EFPBC��EFUID,������ȡ��¼���ӵ�һ����ʼ*/
        }

        return;
    }

    gstPBInitState.enPBInitState = SI_PB_INIT_STATUS_OK;


    if( EFCC == pstPBMsg->usEfId )/*����EFCC�ļ��Ķ�����ظ�*/
    {
        gstPBCtrlInfo.usCCValue += pstPBMsg->aucEf[0]<<8;

        gstPBCtrlInfo.usCCValue += pstPBMsg->aucEf[1];

        return;
    }

    if( EFPSC == pstPBMsg->usEfId )/*����EFPSC�ļ��Ķ�����ظ�*/
    {
        gstPBCtrlInfo.ulPSCValue += pstPBMsg->aucEf[0]<<24;

        gstPBCtrlInfo.ulPSCValue += pstPBMsg->aucEf[1]<<16;

        gstPBCtrlInfo.ulPSCValue += pstPBMsg->aucEf[2]<<8;

        gstPBCtrlInfo.ulPSCValue += pstPBMsg->aucEf[3];

        return;
    }


    if( EFPUID == pstPBMsg->usEfId )/*����EFPUIS�ļ��Ķ�����ظ�*/
    {
        gstPBCtrlInfo.usPUIDValue += pstPBMsg->aucEf[0]<<8;

        gstPBCtrlInfo.usPUIDValue += pstPBMsg->aucEf[1];

        gulPBRecordCnt = 1;/*���濪ʼ��ʼ��EFPBC��EFUID,������ȡ��¼���ӵ�һ����ʼ*/

        return;
    }

    if( gstPBCtrlInfo.astPBCInfo[0].usFileID == pstPBMsg->usEfId )/*����EFPBC�ļ��Ķ�����ظ�*/
    {
        SI_PB_DecodeEFPBCRecord(pstPBMsg->aucEf, pstPBMsg->ucRecordNum);

        if ( ++gulPBRecordCnt > gstPBCtrlInfo.astADNInfo[0].ucRecordNum)/*PBC�ļ�¼����ADN��һ����*/
        {
            gstPBInitState.enPBInitState = SI_PB_INIT_RECORD_EXCEED;/*���м�¼����ȡ���*/
        }
        else
        {
            gstPBInitState.enPBInitState = SI_PB_INIT_RECORD_REMAIN;
        }

        return;
    }

    if( gstPBCtrlInfo.astUIDInfo[0].usFileID == pstPBMsg->usEfId )/*����EFUID�ļ��Ķ�����ظ�*/
    {
        SI_PB_DecodeEFUIDRecord(pstPBMsg->aucEf);

        if ( ++gulPBRecordCnt > gstPBCtrlInfo.astADNInfo[0].ucRecordNum)/*PBC�ļ�¼����ADN��һ����*/
        {
            gstPBInitState.enPBInitState = SI_PB_INIT_RECORD_EXCEED;/*���м�¼����ȡ���*/
        }
        else
        {
            gstPBInitState.enPBInitState = SI_PB_INIT_RECORD_REMAIN;
        }
    }

    return;
}


VOS_VOID SI_PB_InitSYNCHSetProc(PBMsgBlock *pstMsg)
{
    PS_USIM_SET_FILE_CNF_STRU    *pstPBMsg;

    pstPBMsg = (PS_USIM_SET_FILE_CNF_STRU *)pstMsg;

    /*AT2D15855*/
    /*�����PSC�ļ����»ظ�����������б仯��Ŀ�ļ�¼�Ѿ���ɣ�
    �����ȫ�ֱ������㣬׼�������ʼ����һ��*/
    if ( EFCC == pstPBMsg->usEfId )
    {
        gstPBInitState.stPBCUpdate.ucEntryChangeNum = 0;

        PB_INFO_LOG("SI_PB_InitSYNCHSetProc:PB Init Finished!");
    }

    if ( VOS_OK != pstPBMsg->ulResult )
    {
        PB_ERROR_LOG("SI_PB_InitSYNCHProc:USIMM Confirm ERROR");

        return;
    }

    /*����USIMM����PBC������ظ�*/
    if( gstPBCtrlInfo.astPBCInfo[0].usFileID == pstPBMsg->usEfId )
    {

        ++gulPBRecordCnt;

        gstPBInitState.enPBInitState = SI_PB_INIT_RECORD_REMAIN;

        return;
    }

    return;
}
VOS_VOID SI_PB_InitPBStateProc(SI_PB_INIT_STATUS_ENUM_UNIT32 enPBInitState)
{
    VOS_UINT32  ulPBType = 0xFF;

    switch( enPBInitState )
    {
        /* ��������USIMMģ��ط����� */
        case SI_PB_INIT_SEARCH_ERR:
            if (PB_INIT_IAP_SEARCH == gstPBInitState.enPBInitStep)
            {
                gstPBInitState.enPBInitStep = PB_INIT_IAP_CONTENT;
            }
            else if (PB_INIT_EML_SEARCH == gstPBInitState.enPBInitStep)
            {
                gstPBInitState.enPBInitStep = PB_INIT_EML_CONTENT;
            }
            else if (PB_INIT_ANR_SEARCH == gstPBInitState.enPBInitStep)
            {
                gstPBInitState.enPBInitStep = PB_INIT_ANR_CONTENT;
            }
            else if (PB_INIT_XDN_SEARCH == gstPBInitState.enPBInitStep)
            {
                gstPBInitState.enPBInitStep = PB_INIT_XDN_CONTENT;
            }
            else if (PB_INIT_PBC_SEARCH == gstPBInitState.enPBInitStep)
            {
                gstPBInitState.enPBInitStep = PB_INIT_PBC_CONTENT;
            }
            else
            {
                /*do nothing
 */
            }

            gulPBRecordCnt  = 1;    /* �ӵ�һ����¼��ʼ��ʼ���绰������ */

            break;

        case SI_PB_INIT_STATUS_OK:/*��ǰ�ļ���ȷ������׼��������һ���ļ���������*/
        case SI_PB_INIT_EXT_ERR:/*��ʼ��EXT�ļ��ռ�ʱ������������һ��EXT�ļ�*/
            gulPBFileCnt++;

            break;

        case SI_PB_INIT_RECORD_REMAIN:/*��ǰ�ļ�����δ����¼����������ʣ���¼*/
        case SI_PB_INIT_EXT_RECORD:     /* ��Ҫȥ��ʼ��EXT�ļ������� */
        case SI_PB_INIT_SEARCH_OK:      /* SEARCH�����ɹ� */
            break;

        case SI_PB_INIT_PBTYPE_UNKOWN:/*��ǰ��ʼ���ĵ绰�����ͷǷ���������һ���ļ�*/
            gulPBFileCnt++;
            gulPBRecordCnt = 1;
            break;

        case SI_PB_INIT_RECORD_EXCEED:/*��ǰ�ļ����м�¼������ϣ�������һ���ļ�����*/

            /* �����Զ�ȡ��ʽ��ʼ�����ļ���ɳ�ʼ��֮����
            һ���ļ�����Ҫ��SEARCH��ʽ����ʼ�� */
            if (PB_INIT_IAP_CONTENT2 == gstPBInitState.enPBInitStep)
            {
                gstPBInitState.enPBInitStep = PB_INIT_IAP_SEARCH;
            }
            else if (PB_INIT_EML_CONTENT2 == gstPBInitState.enPBInitStep)
            {
                gstPBInitState.enPBInitStep = PB_INIT_EML_SEARCH;
            }
            else if (PB_INIT_ANR_CONTENT2 == gstPBInitState.enPBInitStep)
            {
                gstPBInitState.enPBInitStep = PB_INIT_ANR_SEARCH;
            }
            else if (PB_INIT_XDN_CONTENT2 == gstPBInitState.enPBInitStep)
            {
                gstPBInitState.enPBInitStep = PB_INIT_XDN_SEARCH;
            }
            else
            {

            }

            gulPBFileCnt++;
            gulPBRecordCnt = 1;

            break;

        case SI_PB_INIT_USIMPBCNF_ERR:/*USIMM�ظ�����*/
        case SI_PB_INIT_FILE_DECATIVE:/*���ڳ�ʼ�����ļ�δ����*/
        case SI_PB_INIT_PBMALLOC_FAIL:/*�����ڴ�ʧ��*/

            /*��õ�ǰ��ʼ���ļ���Ӧ�ĵ绰��*/
            SI_PB_GetXDNPBType(&ulPBType, gstPBInitState.ausFileId[gulPBFileCnt]);

            if(0xFF == ulPBType)
            {
                /*No PB Found*/
            }
            else
            {
                if (VOS_NULL_PTR != gastPBContent[ulPBType].pContent)
                {
                    PB_FREE( gastPBContent[ulPBType].pContent);

                    gastPBContent[ulPBType].pContent = VOS_NULL_PTR;
                }

                if (VOS_NULL_PTR != gastPBContent[ulPBType].pIndex)
                {
                    PB_FREE( gastPBContent[ulPBType].pIndex);
                }
            }

            /*������һ���绰��*/
            SI_PB_JumpToNextPB(ulPBType);

            break;

        case SI_PB_INIT_USIMSPBCNF_ERR:/*���ϵ绰����ʼ��USIMM�ظ�����*/
        case SI_PB_INIT_SPBMALLOC_FAIL:/*���ϵ绰�������ڴ�ʧ��*/

            /*�������չ���봦���������������һ���ļ�����*/
            if((PB_INIT_ANR_CONTENT == gstPBInitState.enPBInitStep)
               ||(PB_INIT_EML_CONTENT == gstPBInitState.enPBInitStep))
            {
                gulPBFileCnt++;

                gulPBRecordCnt = 1;

                break;
            }

            gulPBFileCnt = 0;

            gstPBInitState.enPBInitStep++;/*������һ��*/

            if(PB_INIT_SYNCH == gstPBInitState.enPBInitStep)
            {
                /*��д3G��ͬ����Ҫ���ļ���Ϣ*/
                gstPBInitState.ausFileId[0] = EFPSC;
                gstPBInitState.ausFileId[1] = EFCC;
                gstPBInitState.ausFileId[2] = EFPUID;
                /*gstPBInitState.ausFileId[3] = gstPBCtrlInfo.astPBCInfo[0].usFileID;
 */
                gstPBInitState.ausFileId[3] = gstPBCtrlInfo.astUIDInfo[0].usFileID;

                PB_WARNING_LOG("SI_PB_InitPBStateProc: Go to PB_INIT_XDN_CONTENT Step");

                gulPBRecordCnt = 0;
            }
            else
            {
                gulPBRecordCnt = 1;
            }

            break;

        case SI_PB_INIT_MSGTYPE_ERR:

            gstPBInitState.enReqStatus = PB_REQ_SEND;

            break;

        default:

            break;
    }

    return;
}


VOS_UINT32 SI_PB_InitPBStatusJudge(VOS_VOID)
{
    VOS_UINT8 ucCardStatus;
    VOS_UINT8  ucCardType;

    USIMM_GetCardType(&ucCardStatus, &ucCardType);


    /*���ΪSIM������д��Ҫ��ʼ�����ļ��ͳ�ʼ����Ϣ*/
    if ( USIMM_CARD_SIM == ucCardType )
    {
        PB_INFO_LOG("SI_PB_InitPBStatusJudge:Go to PB_INIT_XDN_SPACE Step");

        /* �����AP�Խ�ʱ����Ҫ��ʼ��ADN�ļ� */
        if (VOS_TRUE == SI_PB_CheckSupportAP())
        {
            gstPBCtrlInfo.ulADNFileNum           = 0;

            gstPBInitState.ausFileId[0]          = EFFDN;
            gstPBInitState.ausFileId[1]          = EFBDN;
            gstPBInitState.ausFileId[2]          = EFMSISDN;

            gulPBInitFileNum                     = 3;
        }
        else
        {
            gstPBCtrlInfo.ulADNFileNum           = 1;

            gstPBCtrlInfo.astADNInfo[0].usFileID = SIM_EFADN;

            gstPBInitState.ausFileId[0]          = SIM_EFADN;
            gstPBInitState.ausFileId[1]          = EFFDN;
            gstPBInitState.ausFileId[2]          = EFBDN;
            gstPBInitState.ausFileId[3]          = EFMSISDN;

            gulPBInitFileNum                     = 4;
        }

        gastEXTContent[PB_ADN_CONTENT].usExtFileId      = EFEXT1;
        gastEXTContent[PB_FDN_CONTENT].usExtFileId      = EFEXT2;
        gastEXTContent[PB_BDN_CONTENT].usExtFileId      = EFEXT4_SIM;
        gastEXTContent[PB_MSISDN_CONTENT].usExtFileId   = EFEXT1;

        /*��Ҫ��ʼ����XDN�ļ�����Ϊ4��EXT�ļ��ĸ�����3��
        ԭ����SIM����ADN��MSISDN����һ��EXT�ļ�*/
        gastPBContent[PB_ADN_CONTENT].ulExtInfoNum      = PB_ADN_CONTENT;
        gastPBContent[PB_FDN_CONTENT].ulExtInfoNum      = PB_FDN_CONTENT;
        gastPBContent[PB_BDN_CONTENT].ulExtInfoNum      = PB_BDN_CONTENT;
        gastPBContent[PB_MSISDN_CONTENT].ulExtInfoNum   = PB_ADN_CONTENT;

        /* ����SIM��������SEARCH��ʽ����������Ҫ����ƫ����Ϊ0���Ҳ�֧��SEARCH��ʽ */
        gstPBSearchCtrlInfo.usReadOffset                = 0;
        gstPBInitState.enPBSearchState                  = PB_SEARCH_DISABLE;

        gulPBInitExtFileNum                             = 3;

        /*������һ����ʼ������Ϊ��ʼ��XDN�绰���ռ���Ϣ*/
        gstPBInitState.enPBInitStep          = PB_INIT_XDN_SPACE;
    }
    /*USIM��������ʼ��EFPBR*/
    else
    {
        PB_INFO_LOG("SI_PB_InitPBStatusJudge:Go to PB_INIT_EFPBR Step");

        gstPBInitState.enPBInitStep = PB_INIT_EFPBR;
    }

    gulPBFileCnt = 0;/*�ѳ�ʼ���ļ�������*/

    return VOS_OK;
}

VOS_UINT32 SI_PB_InitEFpbrReq(VOS_VOID)
{
    VOS_UINT32                  ulResult;
    VOS_UINT32                  ulFileCnt;
    USIMM_GET_FILE_INFO_STRU    stGetFileInfo;
    VOS_UINT32                  ulSimSvr;
    VOS_UINT32                  ulUsimSvr;

    /*gulPBFileCnt��Ϊ0,���ѳ�ʼ����EFPBR��������ʼ��XDN�绰���ռ�Ĳ���*/
    if (0 < gulPBFileCnt)
    {
        /* �����AP�Խ�ʱ����Ҫ��ʼ��ADN�ļ� */
        if (VOS_TRUE == SI_PB_CheckSupportAP())
        {
            gstPBCtrlInfo.ulADNFileNum = 0;
        }

        /*��¼��ʼ����������Ҫ��ʼ���ĵ绰���ļ�*/
        for(ulFileCnt=0; ulFileCnt<gstPBCtrlInfo.ulADNFileNum; ulFileCnt++)
        {
            gstPBInitState.ausFileId[ulFileCnt] = gstPBCtrlInfo.astADNInfo[ulFileCnt].usFileID;
        }

        gstPBInitState.ausFileId[ulFileCnt++]   = EFFDN;
        gstPBInitState.ausFileId[ulFileCnt++]   = EFBDN;

        ulSimSvr = USIMM_IsServiceAvailable(SIM_SVR_MSISDN);
        ulUsimSvr = USIMM_IsServiceAvailable(USIM_SVR_MSISDN);

        if ((PS_USIM_SERVICE_AVAILIABLE == ulSimSvr)
            || (PS_USIM_SERVICE_AVAILIABLE == ulUsimSvr))
        {
            gstPBInitState.ausFileId[ulFileCnt++]   = EFMSISDN;
        }

        /* ��¼XDN�ļ���Ӧ��EXT������Ϣ��ƫ���� */
        gastPBContent[PB_ADN_CONTENT].ulExtInfoNum      = PB_ADN_CONTENT;
        gastPBContent[PB_FDN_CONTENT].ulExtInfoNum      = PB_FDN_CONTENT;
        gastPBContent[PB_BDN_CONTENT].ulExtInfoNum      = PB_BDN_CONTENT;
        gastPBContent[PB_MSISDN_CONTENT].ulExtInfoNum   = PB_MSISDN_CONTENT;

        gastEXTContent[PB_ADN_CONTENT].usExtFileId      = gstPBCtrlInfo.usEXT1FileID;
        gastEXTContent[PB_FDN_CONTENT].usExtFileId      = EFEXT2;
        gastEXTContent[PB_BDN_CONTENT].usExtFileId      = EFEXT4_USIM;

        if (PS_USIM_SERVICE_AVAILIABLE != USIMM_IsServiceAvailable(USIM_SVR_EXT5))
        {
            gastEXTContent[PB_MSISDN_CONTENT].usExtFlag = SI_PB_FILE_NOT_EXIST;
            gastEXTContent[PB_MSISDN_CONTENT].usExtFileId = 0xFFFF;

            gulPBInitExtFileNum = 3;
        }
        else
        {
            gastEXTContent[PB_MSISDN_CONTENT].usExtFileId = EFEXT5;

            gulPBInitExtFileNum = 4;
        }

        gstPBInitState.enPBInitStep = PB_INIT_XDN_SPACE;

        /*�����֧�ָ��ϵ绰������������Ϣ��0*/
        if(0 == gstPBConfigInfo.ucSPBFlag)
        {
            gstPBCtrlInfo.ulANRFileNum = 0;
            gstPBCtrlInfo.ulEMLFileNum = 0;
            gstPBCtrlInfo.ulIAPFileNum = 0;
        }

        /*������Type2���͵绰��,ͬһ��ֻ֧��һ�����͵绰��������Anr��Email�±��ȡ0*/
        if((PB_FILE_TYPE2 != gstPBCtrlInfo.astANRInfo[0][0].enANRType)
            &&(PB_FILE_TYPE2 != gstPBCtrlInfo.astEMLInfo[0].enEMLType))
        {
            gstPBCtrlInfo.ulIAPFileNum = 0;
        }

        /*�ܹ����ʼ�����ļ���*/
        gulPBInitFileNum    = ulFileCnt;

        gulPBFileCnt = 0;

        PB_INFO_LOG("SI_PB_InitEFpbrReq: Go to PB_INIT_ADN_SPACE Step");

        return VOS_OK;
    }

    stGetFileInfo.enAppType     = USIMM_PB_APP;
    stGetFileInfo.ucRecordNum   = 0;
    stGetFileInfo.usEfId        = EFPBR;

    ulResult = USIMM_GetFileReq(MAPS_PB_PID, 0, &stGetFileInfo);    /*��USIMMģ�鷢�ͻ�ȡEFPBR�ļ����ݵ�����*/

    /*��������ʧ�ܣ����˳���ʼ��*/
    if ( VOS_OK != ulResult )
    {
        PB_ERROR_LOG("SI_PB_InitEFpbrReq:Get EFPBR Content Failed");

        gstPBInitState.enPBInitStep = PB_INIT_INFO_APP;

        return VOS_ERR;
    }

    /*���ó�ʼ������״̬Ϊ�����ѷ���*/
    gstPBInitState.enReqStatus = PB_REQ_SEND;

    return VOS_OK;
}


VOS_UINT32 SI_PB_InitXDNSpaceReq(VOS_VOID)
{
    VOS_UINT32 ulResult = VOS_OK;

    if(gulPBFileCnt < gulPBInitFileNum)/*������пռ�δ��ʼ���ĵ绰������USIMM���ͻ�ȡ�õ绰����¼��������*/
    {
        ulResult = USIMM_MaxEFRecordNumReq(MAPS_PB_PID, USIMM_PB_APP,
                      gstPBInitState.ausFileId[gulPBFileCnt]);

        /*��������ʧ�ܣ����˳���ʼ������*/
        if ( VOS_OK != ulResult )
        {
            PB_ERROR_LOG("SI_PB_InitXDNSpaceReq:USIMM_MaxEFRecordNumReq Failed");

            gstPBInitState.enPBInitStep = PB_INIT_INFO_APP;

            /*��1��ʼ��ECC����ռ䲻�ͷ�*/
            SI_PB_ClearPBContent(SI_CLEARXDN);

            USIMM_PBInitStatusInd(USIMM_PB_IDLE);

            return VOS_ERR;
        }

        gstPBInitState.enReqStatus = PB_REQ_SEND;

        return VOS_OK;
    }

    /*���е绰���ռ��ʼ����ϣ������ʼ���绰�����ݵĲ���*/
    gulPBFileCnt = 0;/*�ļ���������*/

    gulPBRecordCnt = 1;/*�ӵ�һ����¼��ʼ��ʼ���绰������*/

    gstPBInitState.enPBInitStep = PB_INIT_ANR_SPACE;

    return VOS_OK;
}


VOS_UINT32 SI_PB_InitANRSpaceReq(VOS_VOID)
{
    VOS_UINT16  usANRFileId;

    /*�������ANR�ռ�δ��ʼ���ĵ绰������USIMM���ͻ�ȡ��EXT�ļ���¼��������*/
    if(gulPBFileCnt < gstPBCtrlInfo.ulANRFileNum)
    {
        if (VOS_OK != SI_PB_GetANRFid((gulPBFileCnt+1),&usANRFileId))
        {
            PB_ERROR_LOG("SI_PB_InitANRSpaceReq:SI_PB_GetANRFid Failed");

            /* ��Ӧ��ϵ���ң���û�м�����ʼ���ı�Ҫ */
            SI_PB_ClearSPBContent(SI_CLEARSPB);

            gstPBInitState.enPBInitStep = PB_INIT_INFO_APP;

            return VOS_ERR;
        }

        if(VOS_OK != USIMM_MaxEFRecordNumReq(MAPS_PB_PID, USIMM_PB_APP, usANRFileId) )
        {
            PB_ERROR_LOG("SI_PB_InitANRSpaceReq:USIMM_MaxEFRecordNumReq Failed");

            /* ��������ʧ�ܣ���û�м�����ʼ���ı�Ҫ */
            SI_PB_ClearSPBContent(SI_CLEARSPB);

            gstPBInitState.enPBInitStep = PB_INIT_INFO_APP;

            return VOS_ERR;
        }

        gstPBInitState.enReqStatus = PB_REQ_SEND;

        return VOS_OK;
    }

    PB_INFO_LOG("SI_PB_InitIAPSpaceReq: Go to PB_INIT_EML_SPACE Step");

    gulPBFileCnt = 0;/*�ļ���������*/

    /*����ANR�ռ��ʼ����ϣ�����Email��ʼ���Ĳ���*/
    gstPBInitState.enPBInitStep = PB_INIT_EML_SPACE;

    return VOS_OK;
}
VOS_UINT32 SI_PB_InitEMLSpaceReq(VOS_VOID)
{
    VOS_UINT16  usEMLFileId;

    /*�������Email�ռ�δ��ʼ���ĵ绰������USIMM���ͻ�ȡ��EML�ļ���¼��������*/
    if(gulPBFileCnt < gstPBCtrlInfo.ulEMLFileNum)
    {
        usEMLFileId = gstPBCtrlInfo.astEMLInfo[gulPBFileCnt].usEMLFileID;

        if(VOS_OK != USIMM_MaxEFRecordNumReq(MAPS_PB_PID, USIMM_PB_APP, usEMLFileId) )
        {
            PB_ERROR_LOG("SI_PB_InitEMLSpaceReq:USIMM_MaxEFRecordNumReq Failed");

            /* ��������ʧ�ܣ���û�м�����ʼ���ı�Ҫ */
            SI_PB_ClearSPBContent(SI_CLEARSPB);

            gstPBInitState.enPBInitStep = PB_INIT_INFO_APP;

            return VOS_ERR;
        }

        gstPBInitState.enReqStatus = PB_REQ_SEND;

        return VOS_OK;
    }

    /*����Email�ռ��ʼ����ϣ�����IAP��ʼ���Ĳ���*/

    PB_INFO_LOG("SI_PB_InitEMLSpaceReq: Go to PB_INIT_IAP_SPACE Step");

    gulPBFileCnt = 0;/*�ļ���������*/

    gstPBInitState.enPBInitStep = PB_INIT_IAP_SPACE;

    return VOS_OK;
}


VOS_UINT32 SI_PB_InitIAPSpaceReq(VOS_VOID)
{
    VOS_UINT16  usIAPFileId;

    /*�������IAP�ռ�δ��ʼ���ĵ绰������USIMM���ͻ�ȡ��IAP�ļ���¼��������*/
    if(gulPBFileCnt < gstPBCtrlInfo.ulIAPFileNum)
    {
        usIAPFileId = gstPBCtrlInfo.astIAPInfo[gulPBFileCnt].usIAPFileID;

        if(VOS_OK != USIMM_MaxEFRecordNumReq(MAPS_PB_PID, USIMM_PB_APP, usIAPFileId) )
        {
            PB_ERROR_LOG("SI_PB_InitEMLSpaceReq:USIMM_MaxEFRecordNumReq Failed");

            /* ��������ʧ�ܣ���û�м�����ʼ���ı�Ҫ */
            SI_PB_ClearSPBContent(SI_CLEARSPB);

            gstPBInitState.enPBInitStep = PB_INIT_INFO_APP;

            return VOS_ERR;
        }

        gstPBInitState.enReqStatus = PB_REQ_SEND;

        return VOS_OK;
    }

    /*����IAP�ռ��ʼ����ϣ�����EXT��ʼ���Ĳ���*/

    PB_INFO_LOG("SI_PB_InitIAPSpaceReq: Go to PB_INIT_EXT_SPACE Step");

    gulPBFileCnt = 0;/*�ļ���������*/

    gstPBInitState.enPBInitStep = PB_INIT_EXT_SPACE;

    return VOS_OK;
}


VOS_UINT32 SI_PB_InitEXTSpaceReq(VOS_VOID)
{
    VOS_UINT16  usExtFileId;
    VOS_UINT8   ucCardStatus;
    VOS_UINT8   ucCardType;

    /*�������EXT�ռ�δ��ʼ���ĵ绰������USIMM���ͻ�ȡ��EXT�ļ���¼��������*/
    if(gulPBFileCnt < gulPBInitExtFileNum)
    {
        /*EXT�ļ����±��1��ʼ��0Ԥ����ʹ��*/
        usExtFileId = gastEXTContent[gulPBFileCnt+1].usExtFileId;

        if(0xFFFF == usExtFileId)
        {
            gulPBFileCnt++;

            return VOS_OK;
        }

        if(VOS_OK != USIMM_MaxEFRecordNumReq(MAPS_PB_PID, USIMM_PB_APP, usExtFileId) )
        {
            PB_ERROR_LOG("SI_PB_InitEXTSpaceReq:USIMM_MaxEFRecordNumReq Failed");

            /* �����ʼEXT�ļ��ռ�ʧ�ܣ�����Ӧ�ļ�����ΪFF�������ʼ����һ�� */
            gulPBFileCnt++;
            SI_PB_ExtExceptionProc(usExtFileId);

            return VOS_OK;
        }

        gstPBInitState.enReqStatus = PB_REQ_SEND;

        return VOS_OK;
    }

    /*����EXT�ռ��ʼ����ϣ��������е绰���ռ��ʼ���Ĳ���*/


    gulPBFileCnt = 0;/*�ļ���������*/
    gulPBRecordCnt = 1;/*�ӵ�һ����¼��ʼ��ʼ���绰������*/

    /* �����SIM��ֱ��ȥ�޻�XDN�ļ� */
    USIMM_GetCardType(&ucCardStatus, &ucCardType);

    if (USIMM_CARD_SIM == ucCardType)/*SIM��*/
    {
        gstPBInitState.enPBInitStep = PB_INIT_XDN_CONTENT;

        return VOS_OK;
    }

    /* ���Ƚ���SEARCH��ʽ�ĳ�ʼ�� */
    if((0 != gstPBCtrlInfo.ulIAPFileNum)/*Type2 ���͵绰������*/
       &&(1 == gstPBConfigInfo.ucSPBFlag))/*֧�ָ��ϵ绰��*/
    {
        gstPBInitState.enPBInitStep = PB_INIT_IAP_SEARCH;

        PB_INFO_LOG("SI_PB_InitEXTSpaceReq: Go to PB_INIT_IAP_SEARCH Step");
    }
    else
    {
        gstPBInitState.enPBInitStep = PB_INIT_EML_SEARCH;

        PB_INFO_LOG("SI_PB_InitEXTSpaceReq: Go to PB_INIT_EML_SEARCH Step");
    }

    return VOS_OK;
}
VOS_UINT32 SI_PB_InitIAPContentReq(VOS_VOID)
{
    VOS_UINT32                  ulResult;
    USIMM_GET_FILE_INFO_STRU    stGetFileInfo;

    if( gulPBFileCnt < gstPBCtrlInfo.ulIAPFileNum)/*ĿǰIAP�绰������δ��������*/
    {
        stGetFileInfo.enAppType     = USIMM_PB_APP;
        stGetFileInfo.ucRecordNum   = (VOS_UINT8)gulPBRecordCnt;
        stGetFileInfo.usEfId        = gstPBCtrlInfo.astIAPInfo[gulPBFileCnt].usIAPFileID;

        /*����һ����¼������*/
        ulResult = USIMM_GetFileReq(MAPS_PB_PID, 0, &stGetFileInfo);

        /*���Ͷ�����ʧ�ܣ����˳���ʼ��*/
        if( VOS_OK != ulResult )
        {
            PB_ERROR_LOG("SI_PB_InitIAPContentReq: USIMM_GetFileReq Failed");

            gstPBInitState.enPBInitStep = PB_INIT_INFO_APP;

            if ( VOS_NULL_PTR != gstIAPContent.pIAPContent)
            {
                PB_FREE( gstIAPContent.pIAPContent);
            }

            return VOS_ERR;
        }

        gstPBInitState.enReqStatus = PB_REQ_SEND;

        return VOS_OK;
    }

    gstPBInitState.enPBInitStep = PB_INIT_EML_SEARCH;
    gulPBFileCnt = 0;
    gulPBRecordCnt = 1;/*Ϊ��ʱ��ȡ���м�¼*/

    return VOS_OK;
}


VOS_UINT32 SI_PB_InitEXTContentReq(VOS_VOID)
{
    VOS_UINT32                  ulResult;
    VOS_UINT16                  usFileId;
    VOS_UINT32                  ulPBType;
    VOS_UINT32                  i;
    USIMM_GET_FILE_INFO_STRU    stGetFileInfo;

    usFileId = SI_PB_TransferFileCnt2ExtFileID(gulPBFileCnt, &ulPBType);

    stGetFileInfo.enAppType     = USIMM_PB_APP;
    stGetFileInfo.ucRecordNum   = (VOS_UINT8)gulExtRecord;
    stGetFileInfo.usEfId        = usFileId;

    /*����һ����¼������*/
    ulResult = USIMM_GetFileReq(MAPS_PB_PID, 0, &stGetFileInfo);

    /*���Ͷ�����ʧ�ܣ����˳���ʼ��*/
    if( VOS_OK != ulResult )
    {
        PB_ERROR_LOG("SI_PB_InitXDNContentReq: USIMM_GetFileReq Failed");

        SI_PB_ExtExceptionProc(usFileId);

        gastPBContent[ulPBType].ucNumberLen -= SI_PB_NUM_LEN;

        /*����ANR�ļ���֧����չ����*/
        if(PB_ADN_CONTENT  == ulPBType )
        {
            for(i = 0; i < SI_PB_ANRMAX; i++)
            {
                gastANRContent[i].ucNumberLen = SI_PB_NUM_LEN;
            }
        }

        gstPBInitState.enPBInitStep = PB_INIT_XDN_CONTENT;

        return VOS_OK;
    }

    gstPBInitState.enReqStatus = PB_REQ_SEND;

    return VOS_OK;

}
VOS_UINT32 SI_PB_InitIAPSearchReq(VOS_VOID)
{
    VOS_UINT32              ulResult;
    VOS_UINT8               aucContent[255];
    USIMM_U8_LVDATA_STRU    stData;

    VOS_MemSet(aucContent, (VOS_CHAR)0xFF, sizeof(aucContent));

    /* ĿǰIAP�ļ�����δ��ʼ�������� */
    if (gulPBFileCnt < gstPBCtrlInfo.ulIAPFileNum)
    {
        stData.pucData      = aucContent;
        stData.ulDataLen    = gstPBCtrlInfo.astIAPInfo[gulPBFileCnt].ucRecordLen;

        /* ����IAP���ҵ����� */
        ulResult = USIMM_SearchReq(MAPS_PB_PID,
                                    USIMM_PB_APP,
                                    &stData,
                                    gstPBCtrlInfo.astIAPInfo[gulPBFileCnt].usIAPFileID);

        /* ���Ͷ�����ʧ�ܣ����˳���ʼ�� */
        if( VOS_OK != ulResult )
        {
            PB_ERROR_LOG("SI_PB_InitIAPSearchReq: USIMM_SearchReq Failed");

            gstPBInitState.enPBInitStep = PB_INIT_INFO_APP;

            /* ��������ʧ�ܣ���û�м�����ʼ���ı�Ҫ */
            SI_PB_ClearSPBContent(SI_CLEARSPB);

            return VOS_ERR;
        }

        gstPBInitState.enReqStatus = PB_REQ_SEND;

        return VOS_OK;
    }

    gulPBFileCnt                        = 0;                        /* �ļ����������� */
    gulPBRecordCnt                      = 1;                        /* Ϊ��ʱ��ȡ���м�¼ */
    gstPBInitState.enPBInitStep         = PB_INIT_EML_SEARCH;       /* ��һ���Բ��ҷ�ʽ����ʼ��EMAIL�ļ� */
    gstPBSearchCtrlInfo.usReadOffset    = 0;                        /* ��ȡ����ƥ���б���ƫ������0��ʼ */

    return VOS_OK;
}


VOS_UINT32 SI_PB_InitEmailSearchReq(VOS_VOID)
{
    VOS_UINT32              ulResult;
    VOS_UINT16              usEMLFileId;
    VOS_UINT8               aucContent[255];
    USIMM_U8_LVDATA_STRU    stData;

    VOS_MemSet(aucContent, (VOS_CHAR)0xFF, sizeof(aucContent));

    /* ĿǰEMAIL����δ���ҵ��ļ� */
    if (gulPBFileCnt < gstPBCtrlInfo.ulEMLFileNum)
    {
        usEMLFileId = gstPBCtrlInfo.astEMLInfo[gulPBFileCnt].usEMLFileID;

        stData.pucData      = aucContent;
        stData.ulDataLen    = gstPBCtrlInfo.astEMLInfo[gulPBFileCnt].ucRecordLen;

        /* ���Ͳ������� */
        ulResult = USIMM_SearchReq(MAPS_PB_PID,
                                    USIMM_PB_APP,
                                    &stData,
                                    usEMLFileId);

        /*���Ͷ�����ʧ�ܣ����˳���ʼ��*/
        if( VOS_OK != ulResult )
        {
            PB_ERROR_LOG("SI_PB_InitEmailSearchReq: USIMM_SearchReq Failed");

            /* ��Ӧ��ϵ���ң���û�м�����ʼ���ı�Ҫ,���ӿ��뻺�� */
            SI_PB_ClearSPBContent(SI_CLEARSPB);

            gstPBInitState.enPBInitStep = PB_INIT_INFO_APP;

            return VOS_ERR;
        }

        gstPBInitState.enReqStatus = PB_REQ_SEND;

        return VOS_OK;
    }

    gulPBFileCnt                        = 0;                        /* �ļ����������� */
    gulPBRecordCnt                      = 1;                        /* Ϊ��ʱ��ȡ���м�¼ */
    gstPBInitState.enPBInitStep         = PB_INIT_ANR_SEARCH;       /* ��һ���Բ��ҷ�ʽ����ʼ��ANR�ļ� */
    gstPBSearchCtrlInfo.usReadOffset    = 0;                        /* ��ȡ����ƥ���б���ƫ������0��ʼ */

    return VOS_OK;
}


VOS_UINT32 SI_PB_InitANRSearchReq(VOS_VOID)
{
    VOS_UINT32              ulResult;
    VOS_UINT16              usANRFileId;
    VOS_UINT8               aucContent[255];
    VOS_UINT8               ucXSuffix;
    VOS_UINT8               ucYSuffix;
    USIMM_U8_LVDATA_STRU    stData;

    VOS_MemSet(aucContent, (VOS_CHAR)0xFF, sizeof(aucContent));

    /* ĿǰANR�绰������δ���ҵ����� */
    if (gulPBFileCnt < gstPBCtrlInfo.ulANRFileNum)
    {
        /* �����ǰ��ʼ�����ļ��ѱ�ȥ�����������һ���绰��������ʼ�� */
        if (VOS_OK != SI_PB_GetANRFid((gulPBFileCnt+1),&usANRFileId))
        {
            /* ��Ӧ��ϵ���ң���û�м�����ʼ���ı�Ҫ */
            SI_PB_ClearSPBContent(SI_CLEARSPB);

            gstPBInitState.enPBInitStep = PB_INIT_INFO_APP;

            PB_ERROR_LOG("SI_PB_InitANRSearchReq: Get ANR Fid Failed");

            return VOS_ERR;
        }

        /* ��ȡ��ǰ��ʼ�����ļ���ID */
        if (VOS_OK != SI_PB_GetANRSuffix(&ucXSuffix, &ucYSuffix, usANRFileId))
        {
            gstPBInitState.enPBInitState = SI_PB_INIT_PBTYPE_UNKOWN;

            PB_ERROR_LOG("SI_PB_InitANRSearchReq:PB Tpye Error");

            return VOS_ERR;
        }

        stData.pucData      = aucContent;
        stData.ulDataLen    = gastANRContent[ucYSuffix].ucRecordLen;

        /* ���Ͳ�ѯ���� */
        ulResult = USIMM_SearchReq(MAPS_PB_PID, USIMM_PB_APP, &stData, usANRFileId);

        /* ���Ͷ�����ʧ�ܣ����˳���ʼ�� */
        if( VOS_OK != ulResult )
        {
            gstPBInitState.enPBInitStep = PB_INIT_INFO_APP;

            /* ���ӿ�������ϵ绰������ */
            SI_PB_ClearSPBContent(SI_CLEARSPB);

            PB_ERROR_LOG("SI_PB_InitANRSearchReq: USIMM_SearchReq Failed");

            return VOS_ERR;
        }

        gstPBInitState.enReqStatus = PB_REQ_SEND;

        return VOS_OK;
    }

    gulPBFileCnt                        = 0;                        /* �ļ����������� */
    gulPBRecordCnt                      = 1;                        /* Ϊ��ʱ��ȡ���м�¼ */
    gstPBInitState.enPBInitStep         = PB_INIT_XDN_SEARCH;       /* ��һ���Բ��ҷ�ʽ����ʼ��XDN�ļ� */
    gstPBSearchCtrlInfo.usReadOffset    = 0;                        /* ��ȡ����ƥ���б���ƫ������0��ʼ */

    return VOS_OK;
}
VOS_UINT32 SI_PB_InitXDNSearchReq(VOS_VOID)
{
    VOS_UINT32              ulResult;
    VOS_UINT32              ulPBType;
    VOS_UINT8               aucContent[255];
    USIMM_U8_LVDATA_STRU    stData;

    VOS_MemSet(aucContent, (VOS_CHAR)0xFF, sizeof(aucContent));

    /* ĿǰXDN�绰������δ�������� */
    if (gulPBFileCnt < gulPBInitFileNum)
    {
        SI_PB_GetXDNPBType(&ulPBType, gstPBInitState.ausFileId[gulPBFileCnt]);

        /* �����ǰ��ʼ�����ļ��ѱ�ȥ�����������һ���绰��������ʼ�� */
        /* coverity[uninit_use] */
        if (SI_PB_DEACTIVE == gastPBContent[ulPBType].enActiveStatus)
        {
            PB_NORMAL_LOG("SI_PB_InitXDNSearchReq:The File is Decative");

            SI_PB_JumpToNextPB(ulPBType);

            return VOS_OK;
        }

        stData.pucData      = aucContent;
        stData.ulDataLen    = gastPBContent[ulPBType].ucRecordLen;

        /* ���Ͳ������� */
        ulResult = USIMM_SearchReq(MAPS_PB_PID,
                                    USIMM_PB_APP,
                                    &stData,
                                    gstPBInitState.ausFileId[gulPBFileCnt]);

        /* ���Ͷ�����ʧ�ܣ����˳���ʼ�� */
        if (VOS_OK != ulResult)
        {
            PB_ERROR_LOG("SI_PB_InitXDNSearchReq:USIMM_SearchReq Failed");

            gstPBInitState.enPBInitStep = PB_INIT_INFO_APP;

            /* ���ӿ�����绰������ */
            SI_PB_ClearPBContent(SI_CLEARXDN);
            SI_PB_ClearSPBContent(SI_CLEARSPB);

            return VOS_ERR;
        }

        gstPBInitState.enReqStatus = PB_REQ_SEND;

        return VOS_OK;
    }

    gulPBFileCnt    = 0;
    gulPBRecordCnt  = 1;

    if(VOS_FALSE == gucPBCStatus)
    {
        gstPBInitState.enPBInitStep = PB_INIT_INFO_APP;
    }
    else
    {
        gstPBInitState.enPBInitStep = PB_INIT_PBC_SEARCH;
    }

    return VOS_OK;
}


VOS_UINT32 SI_PB_InitPBCSearchReq(VOS_VOID)
{
    VOS_UINT32              ulResult;
    VOS_UINT16              usPBCFileId;
    VOS_UINT8               aucContent[2];
    USIMM_U8_LVDATA_STRU    stData;

    usPBCFileId     = gstPBCtrlInfo.astPBCInfo[0].usFileID;
    aucContent[0]   = 0;
    aucContent[1]   = 0;

    /* ĿǰPBC�绰������δ���ҵ����� */
    if (gulPBFileCnt < 1)
    {
        stData.pucData      = aucContent;
        stData.ulDataLen    = sizeof(aucContent);

        /* ���Ͳ�ѯ���� */
        ulResult = USIMM_SearchReq(MAPS_PB_PID, USIMM_PB_APP, &stData, usPBCFileId);

        /* ���Ͷ�����ʧ�ܣ����˳���ʼ�� */
        if( VOS_OK != ulResult )
        {
            gstPBInitState.enPBInitStep = PB_INIT_INFO_APP;

            PB_ERROR_LOG("SI_PB_InitPBCSearchReq: USIMM_SearchReq Failed");

            return VOS_ERR;
        }

        gstPBInitState.enReqStatus = PB_REQ_SEND;

        return VOS_OK;
    }

    /* PBC���ҽ�����������ͬ������ */
    gstPBInitState.enPBInitStep = PB_INIT_SYNCH;

    return VOS_OK;
}


VOS_UINT32 SI_PB_InitIAPContentReq2(VOS_VOID)
{
    VOS_UINT32                  ulResult;
    VOS_UINT16                  usOffset;
    USIMM_GET_FILE_INFO_STRU    stGetFileInfo;

    /* ��ǰIAP�ļ�����δ��ʼ���ļ�¼ */
    if (gstPBSearchCtrlInfo.usReadOffset < gstPBSearchCtrlInfo.usReadNum)
    {
        usOffset = gstPBSearchCtrlInfo.usReadOffset;

        stGetFileInfo.enAppType     = USIMM_PB_APP;
        stGetFileInfo.ucRecordNum   = gstPBSearchCtrlInfo.aucReadString[usOffset];
        stGetFileInfo.usEfId        = gstPBSearchCtrlInfo.usEfId;

        /* ����һ����¼������ */
        ulResult = USIMM_GetFileReq(MAPS_PB_PID, 0, &stGetFileInfo);

        /* ���Ͷ�����ʧ�ܣ����˳���ʼ�� */
        if ( VOS_OK != ulResult )
        {
            PB_ERROR_LOG("SI_PB_InitIAPContentReq2: USIMM_GetFileReq Failed");

            gstPBInitState.enPBInitStep = PB_INIT_INFO_APP;

            SI_PB_ClearSPBContent(SI_CLEARSPB);

            return VOS_ERR;
        }

        gstPBInitState.enReqStatus = PB_REQ_SEND;

        return VOS_OK;
    }

    if (gulPBFileCnt < gstPBCtrlInfo.ulIAPFileNum)
    {
        /* ����IAP�ļ�Ҫ���в��Ҵ��� */
        gstPBInitState.enPBInitStep     = PB_INIT_IAP_SEARCH;
    }
    else
    {
        /* ��һ���Բ��ҷ�ʽ����ʼ��EMAIL�ļ� */
        gstPBInitState.enPBInitStep     = PB_INIT_EML_SEARCH;
        gulPBFileCnt                    = 0;    /* �ļ����������� */
    }

    gulPBRecordCnt                      = 1;    /* Ϊ��ʱ��ȡ���м�¼ */
    gstPBSearchCtrlInfo.usReadOffset    = 0;    /* ��ȡ����ƥ���б���ƫ������0��ʼ */

    return VOS_OK;
}


VOS_UINT32 SI_PB_InitEmailContentReq(VOS_VOID)
{
    VOS_UINT32                  ulResult;
    VOS_UINT16                  usOffset;
    USIMM_GET_FILE_INFO_STRU    stGetFileInfo;

    /* ��ǰEMAIL�ļ�����δ��ʼ���ļ�¼ */
    if (gstPBSearchCtrlInfo.usReadOffset < gstPBSearchCtrlInfo.usReadNum)
    {
        usOffset = gstPBSearchCtrlInfo.usReadOffset;

        stGetFileInfo.enAppType     = USIMM_PB_APP;
        stGetFileInfo.ucRecordNum   = gstPBSearchCtrlInfo.aucReadString[usOffset];
        stGetFileInfo.usEfId        = gstPBSearchCtrlInfo.usEfId;

        /*����һ����¼������*/
        ulResult = USIMM_GetFileReq(MAPS_PB_PID, 0, &stGetFileInfo);

        /*���Ͷ�����ʧ�ܣ����˳���ʼ��*/
        if (VOS_OK != ulResult)
        {
            PB_ERROR_LOG("SI_PB_InitEmailContentReq: USIMM_GetFileReq Failed");

            gstPBInitState.enPBInitStep = PB_INIT_INFO_APP;

            SI_PB_ClearSPBContent(SI_CLEARSPB);

            return VOS_ERR;
        }

        gstPBInitState.enReqStatus = PB_REQ_SEND;

        return VOS_OK;
    }

    if (gulPBFileCnt < gstPBCtrlInfo.ulEMLFileNum)
    {
        /* ����EMAIL�ļ�Ҫ���в��Ҵ��� */
        gstPBInitState.enPBInitStep     = PB_INIT_EML_SEARCH;
    }
    else
    {
        /* ��һ���Բ��ҷ�ʽ����ʼ��ANR�ļ� */
        gstPBInitState.enPBInitStep     = PB_INIT_ANR_SEARCH;
        gulPBFileCnt                    = 0;    /* �ļ����������� */
    }

    gulPBRecordCnt                      = 1;    /* Ϊ��ʱ��ȡ���м�¼ */
    gstPBSearchCtrlInfo.usReadOffset    = 0;    /* ��ȡ����ƥ���б���ƫ������0��ʼ */

    return VOS_OK;
}


VOS_UINT32 SI_PB_InitANRContentReq2(VOS_VOID)
{
    VOS_UINT32                  ulResult;
    VOS_UINT16                  usOffset;
    USIMM_GET_FILE_INFO_STRU    stGetFileInfo;

    /* ��ǰANR�ļ�����δ��ʼ���ļ�¼ */
    if (gstPBSearchCtrlInfo.usReadOffset < gstPBSearchCtrlInfo.usReadNum)
    {
        usOffset = gstPBSearchCtrlInfo.usReadOffset;

        stGetFileInfo.enAppType     = USIMM_PB_APP;
        stGetFileInfo.ucRecordNum   = gstPBSearchCtrlInfo.aucReadString[usOffset];
        stGetFileInfo.usEfId        = gstPBSearchCtrlInfo.usEfId;

        /* ����һ����¼������ */
        ulResult = USIMM_GetFileReq(MAPS_PB_PID, 0, &stGetFileInfo);

        /*���Ͷ�����ʧ�ܣ����˳���ʼ��*/
        if (VOS_OK != ulResult)
        {
            PB_ERROR_LOG("SI_PB_InitANRContentReq2: USIMM_GetFileReq Failed");

            gstPBInitState.enPBInitStep = PB_INIT_INFO_APP;

            /*���ӿ�������ϵ绰������*/
            SI_PB_ClearSPBContent(SI_CLEARSPB);

            return VOS_ERR;
        }

        gstPBInitState.enReqStatus = PB_REQ_SEND;

        return VOS_OK;
    }

    if (gulPBFileCnt < gstPBCtrlInfo.ulANRFileNum)
    {
        /* ����ANR�ļ�Ҫ���в��Ҵ��� */
        gstPBInitState.enPBInitStep = PB_INIT_ANR_SEARCH;
    }
    else
    {
        /* ��һ���Բ��ҷ�ʽ����ʼ��XDN�ļ� */
        gstPBInitState.enPBInitStep = PB_INIT_XDN_SEARCH;
        gulPBFileCnt                = 0;        /* �ļ����������� */
    }

    gulPBRecordCnt                      = 1;    /* Ϊ��ʱ��ȡ���м�¼ */
    gstPBSearchCtrlInfo.usReadOffset    = 0;    /* ��ȡ����ƥ���б���ƫ������0��ʼ */

    return VOS_OK;
}
VOS_UINT32 SI_PB_InitXDNContentReq2(VOS_VOID)
{
    VOS_UINT32                  ulResult;
    VOS_UINT16                  usOffset;
    VOS_UINT8                   ucCardStatus;
    VOS_UINT8                   ucCardType;
    USIMM_GET_FILE_INFO_STRU    stGetFileInfo;

    /* ��ǰXDN�ļ�����δ��ʼ���ļ�¼ */
    if (gstPBSearchCtrlInfo.usReadOffset < gstPBSearchCtrlInfo.usReadNum)
    {
        usOffset = gstPBSearchCtrlInfo.usReadOffset;

        stGetFileInfo.enAppType     = USIMM_PB_APP;
        stGetFileInfo.ucRecordNum   = gstPBSearchCtrlInfo.aucReadString[usOffset];
        stGetFileInfo.usEfId        = gstPBSearchCtrlInfo.usEfId;

        /* ����һ����¼������ */
        ulResult = USIMM_GetFileReq(MAPS_PB_PID, 0, &stGetFileInfo);

        /* ���Ͷ�����ʧ�ܣ����˳���ʼ�� */
        if (VOS_OK != ulResult)
        {
            PB_ERROR_LOG("SI_PB_InitXDNContentReq2: USIMM_GetFileReq Failed");

            gstPBInitState.enPBInitStep = PB_INIT_INFO_APP;

            /* ���ӿ�������ϵ绰������ */
            SI_PB_ClearSPBContent(SI_CLEARSPB);
            SI_PB_ClearPBContent(SI_CLEARXDN);

            return VOS_ERR;
        }

        gstPBInitState.enReqStatus = PB_REQ_SEND;

        return VOS_OK;
    }

    if (gulPBFileCnt < gulPBInitFileNum)
    {
        /* ����XDN�ļ�Ҫ���в��Ҵ��� */
        gstPBInitState.enPBInitStep         = PB_INIT_XDN_SEARCH;
        gstPBSearchCtrlInfo.usReadOffset    = 0;    /* ��ȡ����ƥ���б���ƫ������0��ʼ */
    }
    else
    {
        gulPBFileCnt    = 0;
        gulPBRecordCnt  = 1;

        USIMM_GetCardType(&ucCardStatus, &ucCardType);

        if (USIMM_CARD_SIM == ucCardType)/*SIM��*/
        {
            /* SIM������ͬ�����̣�֪ͨAPP��ʼ������� */
            PB_INFO_LOG("SI_PB_InitXDNContentReq2: Go to PB_INIT_INFO_APP Step");

            gstPBInitState.enPBInitStep = PB_INIT_INFO_APP;
        }
        else
        {
            PB_INFO_LOG("SI_PB_InitXDNContentReq2: Go to PB_INIT_PBC_SEARCH Step");

            if(VOS_FALSE == gucPBCStatus)
            {
                gstPBInitState.enPBInitStep = PB_INIT_INFO_APP;
            }
            else
            {
                gstPBInitState.enPBInitStep = PB_INIT_PBC_SEARCH;
            }
        }
    }

    return VOS_OK;
}


VOS_UINT32 SI_PB_InitPBCContentReq2(VOS_VOID)
{
    VOS_UINT32                  ulResult;
    VOS_UINT16                  usOffset;
    USIMM_GET_FILE_INFO_STRU    stGetFileInfo;

    /* ��ǰPBC�ļ�����δ��ʼ���ļ�¼ */
    if (gstPBSearchCtrlInfo.usReadOffset < gstPBSearchCtrlInfo.usReadNum)
    {
        usOffset = gstPBSearchCtrlInfo.usReadOffset;

        stGetFileInfo.enAppType     = USIMM_PB_APP;
        stGetFileInfo.ucRecordNum   = gstPBSearchCtrlInfo.aucReadString[usOffset];
        stGetFileInfo.usEfId        = gstPBSearchCtrlInfo.usEfId;

        /* ����һ����¼������ */
        ulResult = USIMM_GetFileReq(MAPS_PB_PID, 0, &stGetFileInfo);

        /* ���Ͷ�����ʧ�ܣ����˳���ʼ�� */
        if (VOS_OK != ulResult)
        {
            PB_ERROR_LOG("SI_PB_InitPBCContentReq2: USIMM_GetFileReq Failed");

            gstPBInitState.enPBInitStep = PB_INIT_INFO_APP;

            return VOS_ERR;
        }

        gstPBInitState.enReqStatus = PB_REQ_SEND;

        return VOS_OK;
    }
    else
    {
        gulPBFileCnt    = 0;
        gulPBRecordCnt  = 0;

        /* ��д3G��ͬ����Ҫ���ļ���Ϣ */
        gstPBInitState.ausFileId[0] = EFPSC;
        gstPBInitState.ausFileId[1] = EFCC;
        gstPBInitState.ausFileId[2] = EFPUID;
        /*gstPBInitState.ausFileId[3] = gstPBCtrlInfo.astPBCInfo[0].usFileID; */
        gstPBInitState.ausFileId[3] = gstPBCtrlInfo.astUIDInfo[0].usFileID;
        gstPBInitState.enPBInitStep = PB_INIT_SYNCH;

        return VOS_OK;
    }
}
VOS_UINT32 SI_PB_InitPBCContentReq(VOS_VOID)
{
    VOS_UINT32                  ulResult;
    VOS_UINT16                  usPBCFileId;
    USIMM_GET_FILE_INFO_STRU    stGetFileInfo;

    usPBCFileId     = gstPBCtrlInfo.astPBCInfo[0].usFileID;

    /* ĿǰPBC�绰������δ��ȡ�ļ�¼ */
    if (gulPBFileCnt < 1)
    {
        stGetFileInfo.enAppType     = USIMM_PB_APP;
        stGetFileInfo.ucRecordNum   = (VOS_UINT8)gulPBRecordCnt;
        stGetFileInfo.usEfId        = usPBCFileId;

        ulResult = USIMM_GetFileReq(MAPS_PB_PID, 0, &stGetFileInfo);

        /*���Ͷ�����ʧ�ܣ����˳���ʼ��*/
        if( VOS_OK != ulResult )
        {
            gstPBInitState.enPBInitStep = PB_INIT_INFO_APP;

            PB_ERROR_LOG("SI_PB_InitPBCContentReq: USIMM_GetFileReq Failed");

            return VOS_ERR;
        }

        gstPBInitState.enReqStatus = PB_REQ_SEND;

        return VOS_OK;
    }
    else
    {
        gulPBFileCnt    = 0;
        gulPBRecordCnt  = 0;

        /* ��д3G��ͬ����Ҫ���ļ���Ϣ */
        gstPBInitState.ausFileId[0] = EFPSC;
        gstPBInitState.ausFileId[1] = EFCC;
        gstPBInitState.ausFileId[2] = EFPUID;
        /*gstPBInitState.ausFileId[3] = gstPBCtrlInfo.astPBCInfo[0].usFileID; */
        gstPBInitState.ausFileId[3] = gstPBCtrlInfo.astUIDInfo[0].usFileID;
        gstPBInitState.enPBInitStep = PB_INIT_SYNCH;

        return VOS_OK;
    }
}


VOS_UINT32 SI_PB_InitXDNContentReq(VOS_VOID)
{
    VOS_UINT32                  ulResult;
    VOS_UINT32                  ulPBType;
    VOS_UINT8                   ucCardStatus;
    VOS_UINT8                   ucCardType;
    USIMM_GET_FILE_INFO_STRU    stGetFileInfo;

    if( gulPBFileCnt < gulPBInitFileNum )/*ĿǰXDN�绰������δ��������*/
    {
        SI_PB_GetXDNPBType(&ulPBType, gstPBInitState.ausFileId[gulPBFileCnt]);

        /*�����ǰ��ʼ�����ļ��ѱ�ȥ�����������һ���绰��������ʼ��*/
        /* coverity[uninit_use] */
        if( SI_PB_DEACTIVE == gastPBContent[ulPBType].enActiveStatus )
        {
            PB_NORMAL_LOG("SI_PB_InitXDNContentReq:The File is Decative");

            SI_PB_JumpToNextPB(ulPBType);

            return VOS_OK;
        }

        stGetFileInfo.enAppType     = USIMM_PB_APP;
        stGetFileInfo.ucRecordNum   = (VOS_UINT8)gulPBRecordCnt;
        stGetFileInfo.usEfId        = gstPBInitState.ausFileId[gulPBFileCnt];

        /*����һ����¼������*/
        ulResult = USIMM_GetFileReq(MAPS_PB_PID, 0, &stGetFileInfo);

        /*���Ͷ�����ʧ�ܣ����˳���ʼ��*/
        if( VOS_OK != ulResult )
        {
            PB_ERROR_LOG("SI_PB_InitXDNContentReq: USIMM_GetFileReq Failed");

            gstPBInitState.enPBInitStep = PB_INIT_INFO_APP;

            if ( VOS_NULL_PTR != gastPBContent[ulPBType].pContent )
            {
                PB_FREE( gastPBContent[ulPBType].pContent);

                gastPBContent[ulPBType].pContent = VOS_NULL_PTR;
            }

            return VOS_ERR;
        }

        gstPBInitState.enReqStatus = PB_REQ_SEND;

        return VOS_OK;
    }

    USIMM_GetCardType(&ucCardStatus, &ucCardType);

    if (USIMM_CARD_SIM == ucCardType)/*SIM��*/
    {
        PB_INFO_LOG("SI_PB_InitXDNContentReq: Go to PB_INIT_INFO_APP Step");

        gstPBInitState.enPBInitStep = PB_INIT_INFO_APP;
    }
    else
    {
        PB_INFO_LOG("SI_PB_InitXDNContentReq: Go to PB_INIT_PBC_SEARCH Step");

        if(VOS_FALSE == gucPBCStatus)
        {
            gstPBInitState.enPBInitStep = PB_INIT_INFO_APP;
        }
        else
        {
            gstPBInitState.enPBInitStep = PB_INIT_PBC_SEARCH;
        }
    }

    return VOS_OK;
}


VOS_UINT32 SI_PB_InitANRContentReq(VOS_VOID)
{
    VOS_UINT32                  ulResult;
    VOS_UINT16                  usANRFileId;
    USIMM_GET_FILE_INFO_STRU    stGetFileInfo;

    if( gulPBFileCnt < gstPBCtrlInfo.ulANRFileNum)/*ĿǰANR�绰������δ��������*/
    {
        /*�����ǰ��ʼ�����ļ��ѱ�ȥ�����������һ���绰��������ʼ��*/
        if (VOS_OK != SI_PB_GetANRFid((gulPBFileCnt+1),&usANRFileId))
        {
            /* ��Ӧ��ϵ���ң���û�м�����ʼ���ı�Ҫ */
            SI_PB_ClearSPBContent(SI_CLEARSPB);

            gstPBInitState.enPBInitStep = PB_INIT_INFO_APP;

            PB_ERROR_LOG("SI_PB_InitANRContentReq: Get ANR Fid Failed");

            return VOS_ERR;
        }

        stGetFileInfo.enAppType     = USIMM_PB_APP;
        stGetFileInfo.ucRecordNum   = (VOS_UINT8)gulPBRecordCnt;
        stGetFileInfo.usEfId        = usANRFileId;

        /*����һ����¼������*/
        ulResult = USIMM_GetFileReq(MAPS_PB_PID, 0, &stGetFileInfo);

        /*���Ͷ�����ʧ�ܣ����˳���ʼ��*/
        if( VOS_OK != ulResult )
        {
            gstPBInitState.enPBInitStep = PB_INIT_INFO_APP;

            /*���ӿ�������ϵ绰������*/
            SI_PB_ClearSPBContent(SI_CLEARSPB);

            PB_ERROR_LOG("SI_PB_InitANRContentReq: USIMM_GetFileReq Failed");

            return VOS_ERR;
        }

        gstPBInitState.enReqStatus = PB_REQ_SEND;

        return VOS_OK;
    }

    gulPBFileCnt = 0;
    gulPBRecordCnt = 1;
    gstPBInitState.enPBInitStep = PB_INIT_XDN_SEARCH;

    return VOS_OK;
}


VOS_UINT32 SI_PB_InitEXTRContentReq(VOS_VOID)
{
    VOS_UINT32                  ulResult;
    VOS_UINT16                  usEXTFileId;
    VOS_UINT32                  i;
    USIMM_GET_FILE_INFO_STRU    stGetFileInfo;

    usEXTFileId = gastEXTContent[PB_ADN_CONTENT].usExtFileId;

    stGetFileInfo.enAppType     = USIMM_PB_APP;
    stGetFileInfo.ucRecordNum   = (VOS_UINT8)gulExtRecord;
    stGetFileInfo.usEfId        = usEXTFileId;

    /*����һ����¼������*/
    ulResult = USIMM_GetFileReq(MAPS_PB_PID, 0, &stGetFileInfo);

    /*���Ͷ�����ʧ�ܣ������ö�ӦEXTΪ��Ч��������ʼ������ANR����*/
    if( VOS_OK != ulResult )
    {
        SI_PB_ExtExceptionProc(usEXTFileId);

        gastPBContent[PB_ADN_CONTENT].ucNumberLen = SI_PB_NUM_LEN;

        /*����ANR�ļ���֧����չ����*/
        for(i = 0; i < SI_PB_ANRMAX; i++)
        {
            gastANRContent[i].ucNumberLen = SI_PB_NUM_LEN;
        }

        /*AT2D15843*/
        gstPBInitState.enPBInitStep = PB_INIT_ANR_CONTENT;

        PB_ERROR_LOG("SI_PB_InitEXTRContentReq: USIMM_GetFileReq Failed");

        return VOS_OK;
    }

    gstPBInitState.enReqStatus = PB_REQ_SEND;

    return VOS_OK;

}


VOS_UINT32 SI_PB_InitEMLContentReq(VOS_VOID)
{
    VOS_UINT32                  ulResult;
    VOS_UINT16                  usEMLFileId;
    USIMM_GET_FILE_INFO_STRU    stGetFileInfo;

    if( gulPBFileCnt < gstPBCtrlInfo.ulEMLFileNum)/*ĿǰANR�绰������δ��������*/
    {
        usEMLFileId = gstPBCtrlInfo.astEMLInfo[gulPBFileCnt].usEMLFileID;

        stGetFileInfo.enAppType     = USIMM_PB_APP;
        stGetFileInfo.ucRecordNum   = (VOS_UINT8)gulPBRecordCnt;
        stGetFileInfo.usEfId        = usEMLFileId;

        /*����һ����¼������*/
        ulResult = USIMM_GetFileReq(MAPS_PB_PID, 0, &stGetFileInfo);

        /*���Ͷ�����ʧ�ܣ����˳���ʼ��*/
        if( VOS_OK != ulResult )
        {
            PB_ERROR_LOG("SI_PB_InitXDNContentReq: USIMM_GetFileReq Failed");

            /* ��Ӧ��ϵ���ң���û�м�����ʼ���ı�Ҫ */
            SI_PB_ClearSPBContent(SI_CLEARSPB);

            PB_ERROR_LOG("SI_PB_InitANRContentReq: Get ANR Fid Failed");

            gstPBInitState.enPBInitStep = PB_INIT_INFO_APP;

            /*���ӿ��뻺��*/

            return VOS_ERR;
        }

        gstPBInitState.enReqStatus = PB_REQ_SEND;

        return VOS_OK;
    }

    gulPBFileCnt = 0;
    gulPBRecordCnt = 1;
    gstPBInitState.enPBInitStep = PB_INIT_ANR_SEARCH;

    return VOS_OK;
}
VOS_UINT32 SI_PB_InitSYNCHReq(VOS_VOID)
{
    VOS_UINT32                  ulResult;
    VOS_UINT16                  usZero = 0;
    VOS_UINT8                   ucRecordNum;
    USIMM_SET_FILE_INFO_STRU    stUpdateReq;
    USIMM_GET_FILE_INFO_STRU    stGetFileInfo;

    /* �ܹ���4��3G��ͬ���ļ�,PBC�ļ������������н��г�ʼ�� */
    if( 4 > gulPBFileCnt )
    {
        stGetFileInfo.enAppType     = USIMM_PB_APP;
        stGetFileInfo.ucRecordNum   = (VOS_UINT8)gulPBRecordCnt;
        stGetFileInfo.usEfId        = gstPBInitState.ausFileId[gulPBFileCnt];

        /*�ļ�������������һ�λ�ȡ���м�¼����*/
        ulResult = USIMM_GetFileReq(MAPS_PB_PID, 0, &stGetFileInfo);

        /* ���Ͷ�ȡ�ļ�����ʧ�ܣ����ó�ʼ��״̬Ϊ��ʼ������� */
        if ( VOS_OK != ulResult )
        {
            PB_ERROR_LOG("SI_PB_InitSYNCHReq: USIMM_GetFileReq Failed");

            gstPBInitState.enPBInitStep = PB_INIT_INFO_APP;

            return VOS_OK;
        }

        gstPBInitState.enReqStatus = PB_REQ_SEND;
    }
    /*�ж�ADN�绰���������Ƿ����仯*/
    else if ( 0 != gstPBInitState.stPBCUpdate.ucEntryChangeNum )
    {
        /*���·����仯��PBC�ļ���¼*/
        if ( gulPBRecordCnt <= gstPBInitState.stPBCUpdate.ucEntryChangeNum )
        {
            /*��ȡ�����仯��PBC�ļ��ļ�¼��*/
            ucRecordNum = gstPBInitState.stPBCUpdate.aucRecord[gulPBRecordCnt];

            stUpdateReq.enAppType       = USIMM_PB_APP;
            stUpdateReq.pucEfContent    = (VOS_UINT8 *)&usZero;
            stUpdateReq.ucRecordNum     = ucRecordNum;
            stUpdateReq.ulEfLen         = sizeof(usZero);
            stUpdateReq.usEfId          = gstPBCtrlInfo.astPBCInfo[0].usFileID;

            /*�Ѷ�Ӧ��PBC��¼��Ϊ0*/
            ulResult = USIMM_SetFileReq(MAPS_PB_PID, 0, &stUpdateReq);

            /*����д�ļ�����ʧ�ܣ����˳���ʼ��*/
            if ( VOS_OK != ulResult )
            {
                PB_ERROR_LOG("SI_PB_InitSYNCHReq: USIMM_GetFileReq Failed");

                gstPBInitState.enPBInitStep = PB_INIT_INFO_APP;

                return VOS_ERR;
            }

            gstPBInitState.enReqStatus = PB_REQ_SEND;
        }
        /*�ж��Ƿ���Ҫ����CC��UID��PSC����ͬ���ļ�*/
        else if ( 1 != gstPBInitState.stPBCUpdate.ucCCUpdateFlag )
        {
            SI_PB_IncreaceCCValue(gstPBInitState.stPBCUpdate.ucEntryChangeNum,1);

            gstPBInitState.stPBCUpdate.ucCCUpdateFlag = 1;

            gstPBInitState.enReqStatus = PB_REQ_SEND;
        }
        /*�ȴ���ͬ���������*/
        else
        {
            /*�ȴ�3G��ͬ������*/
            gstPBInitState.enReqStatus = PB_REQ_SEND;
        }
    }
    /*��ͬ��������ɣ�����ص��ϱ�����*/
    else
    {
        PB_INFO_LOG("SI_PB_InitSYNCHReq:Go to PB_INIT_INFO_APP Step");

        gstPBInitState.enPBInitStep = PB_INIT_INFO_APP;
    }

    return VOS_OK;
}


VOS_UINT32 SI_PB_InitInfoApp(VOS_VOID)
{
    VOS_UINT8               ucCardType;
    VOS_UINT32              ulBdnState;
    VOS_UINT32              ulFdnState;
    SI_PB_EVENT_INFO_STRU   stPBEvent;

    USIMM_GetCardType(VOS_NULL_PTR, &ucCardType);

    stPBEvent.ClientId    = MN_CLIENT_ALL;
    stPBEvent.ClientId    = MN_GetRealClientId(stPBEvent.ClientId, MAPS_PB_PID);
    stPBEvent.OpId        = 0;
    stPBEvent.PBError     = VOS_OK;
    stPBEvent.PBEventType = SI_PB_EVENT_INFO_IND;

    /*��ȡFDN��BDN״̬*/
    USIMM_BdnQuery(&ulBdnState);
    USIMM_FdnQuery(&ulFdnState);

    /*��д��Ҫ�ص��ϱ��ĵ绰����Ϣ*/
    stPBEvent.PBEvent.PBInfoInd.ADNRecordNum    = gastPBContent[PB_ADN_CONTENT].usTotalNum;
    stPBEvent.PBEvent.PBInfoInd.ADNTextLen      = (VOS_UINT16)gastPBContent[PB_ADN_CONTENT].ucNameLen;
    stPBEvent.PBEvent.PBInfoInd.ADNNumberLen    = gastPBContent[PB_ADN_CONTENT].ucNumberLen;
    stPBEvent.PBEvent.PBInfoInd.BDNRecordNum    = gastPBContent[PB_BDN_CONTENT].usTotalNum;
    stPBEvent.PBEvent.PBInfoInd.BdnState        = (VOS_UINT16)ulBdnState;
    stPBEvent.PBEvent.PBInfoInd.BDNTextLen      = (VOS_UINT16)gastPBContent[PB_BDN_CONTENT].ucNameLen;
    stPBEvent.PBEvent.PBInfoInd.BDNNumberLen    = gastPBContent[PB_BDN_CONTENT].ucNumberLen;
    stPBEvent.PBEvent.PBInfoInd.FDNRecordNum    = gastPBContent[PB_FDN_CONTENT].usTotalNum;
    stPBEvent.PBEvent.PBInfoInd.FdnState        = (VOS_UINT16)ulFdnState;
    stPBEvent.PBEvent.PBInfoInd.FDNTextLen      = (VOS_UINT16)gastPBContent[PB_FDN_CONTENT].ucNameLen;
    stPBEvent.PBEvent.PBInfoInd.FDNNumberLen    = gastPBContent[PB_FDN_CONTENT].ucNumberLen;
    stPBEvent.PBEvent.PBInfoInd.MSISDNTextLen   = (VOS_UINT16)gastPBContent[PB_MSISDN_CONTENT].ucNameLen;
    stPBEvent.PBEvent.PBInfoInd.MSISDNRecordNum = gastPBContent[PB_MSISDN_CONTENT].usTotalNum;
    stPBEvent.PBEvent.PBInfoInd.MSISDNNumberLen = gastPBContent[PB_MSISDN_CONTENT].ucNumberLen;
    stPBEvent.PBEvent.PBInfoInd.ANRNumberLen    = gastANRContent[0].ucNumberLen;
    stPBEvent.PBEvent.PBInfoInd.EMAILTextLen    = gstEMLContent.ucDataLen;
    stPBEvent.PBEvent.PBInfoInd.CardType        = (VOS_UINT16)ucCardType;

    SI_PBCallback(&stPBEvent);

    /*���ó�ʼ��״̬Ϊ�ѳ�ʼ��*/
    gstPBInitState.enPBInitStep = PB_INIT_FINISHED;

    /*֪ͨUSIMģ����Խ���ʱ��ֹͣģʽ*/
    USIMM_PBInitStatusInd(USIMM_PB_IDLE);

    SI_PBSendGlobalToAcpu();

    gstPBInitState.enReqStatus = PB_REQ_SEND;

    return VOS_OK;
}


VOS_UINT32 SI_PB_InitEccProc(PBMsgBlock *pstMsg)
{
    PS_USIM_ECC_IND_STRU *pstPBMsg;
    VOS_UINT32           ulOffset;
    VOS_UINT8            *pucRecord;

    pstPBMsg = (PS_USIM_ECC_IND_STRU *)(pstMsg);

    /* ������飬�ϱ���ECC����Ϊ�գ����ʼ��������������ش��� */
    if (VOS_NULL == pstPBMsg->usFileSize)
    {
        gastPBContent[PB_ECC_CONTENT].enInitialState = PB_FILE_NOT_EXIST;
        return VOS_ERR;
    }

    gastPBContent[PB_ECC_CONTENT].pContent\
        = (VOS_UINT8 *)PB_MALLOC(pstPBMsg->usFileSize);

    if ( VOS_NULL_PTR == gastPBContent[PB_ECC_CONTENT].pContent )
    {
        PB_ERROR_LOG("SI_PB_InitEccProc:MemAlloc Failed");

        return VOS_ERR;
    }

    VOS_MemCpy(gastPBContent[PB_ECC_CONTENT].pContent, pstPBMsg->ucContent, pstPBMsg->usFileSize);

    /*USIM��ECC����Ϊ��¼�ļ���SIM��ECC�ļ�Ϊ͸���ļ�*/
    if ( USIMM_CARD_USIM == pstPBMsg->ucEccType )
    {
        gastPBContent[PB_ECC_CONTENT].usTotalNum  = pstPBMsg->ucRecordNum;
        gastPBContent[PB_ECC_CONTENT].ucRecordLen = (VOS_UINT8)(pstPBMsg->usFileSize/pstPBMsg->ucRecordNum);
        gastPBContent[PB_ECC_CONTENT].ucNameLen   = gastPBContent[PB_ECC_CONTENT].ucRecordLen - SI_PB_ECC_EXCEPT_NAME;
        gastPBContent[PB_ECC_CONTENT].ucNumberLen = SI_PB_ECC_LEN;
    }
    else
    {
        gastPBContent[PB_ECC_CONTENT].usTotalNum  = (VOS_UINT16)(pstPBMsg->usFileSize/SI_PB_ECC_LEN);
        gastPBContent[PB_ECC_CONTENT].ucRecordLen = SI_PB_ECC_LEN;
        gastPBContent[PB_ECC_CONTENT].ucNameLen   = 0;
        gastPBContent[PB_ECC_CONTENT].ucNumberLen = SI_PB_ECC_LEN;
    }

    /*�ж�ÿ����¼����Ч�ԣ��ó����ü�¼��*/
    for (ulOffset = 0; ulOffset < gastPBContent[PB_ECC_CONTENT].usTotalNum; ulOffset++)
    {
       pucRecord = pstPBMsg->ucContent +(ulOffset*gastPBContent[PB_ECC_CONTENT].ucRecordLen);

       if ( VOS_OK == SI_PB_CheckEccValidity(pucRecord))
       {
            gastPBContent[PB_ECC_CONTENT].usUsedNum++;
       }
    }

    /*��ECC�绰����ʼ��״̬Ϊ�ѳ�ʼ��*/
    gastPBContent[PB_ECC_CONTENT].enInitialState = PB_INITIALISED;

    return VOS_OK;
}


VOS_VOID SI_PB_InitPBStatusProc(PBMsgBlock *pstMsg)
{

    PS_USIM_STATUS_IND_STRU *pstPBMsg;

    pstPBMsg = (PS_USIM_STATUS_IND_STRU *)pstMsg;

    if (PS_USIM_GET_STATUS_IND != pstPBMsg->ulMsgName)
    {
        gstPBInitState.enPBInitState = SI_PB_INIT_MSGTYPE_ERR;

        LogPrint("\r\nSI_PB_InitPBStatusProc:Wrong Msg!\r\n");

        return;
    }

    SI_PB_ClearPBContent(SI_CLEARXDN);

    SI_PB_ClearSPBContent(SI_CLEARALL);

    /*�����񲻿���*/
    if ( USIMM_CARD_SERVIC_AVAILABLE != pstPBMsg->enCardStatus )
    {
        PB_ERROR_LOG("SI_PB_InitPBStatusProc:Card Servic Unavaliable");

        USIMM_PBInitStatusInd(USIMM_PB_IDLE);

        gstPBInitState.enPBInitState = SI_PB_INIT_MSGTYPE_ERR;
    }
    else
    {
        PB_INFO_LOG("SI_PB_InitPBStatusProc:PB Init Begin");

        USIMM_PBInitStatusInd(USIMM_PB_INITIALING);

        SI_PB_InitGlobeVariable();

        gstPBInitState.enReqStatus = PB_REQ_NOT_SEND;

        gstPBInitState.enPBInitState = SI_PB_INIT_STATUS_OK;
    }

    return;
}
VOS_VOID SI_PB_InitEFpbrProc(PBMsgBlock *pstMsg)
{
    PS_USIM_GET_FILE_CNF_STRU *pstPBMsg;
    VOS_UINT32                 ulResult;
    VOS_UINT32                 ulPbrTotalNum;

    pstPBMsg = (PS_USIM_GET_FILE_CNF_STRU *)(pstMsg);

    /*�ظ������ݷǵ�ǰ�����������ʼ���������*/
    if ((PS_USIM_GET_FILE_RSP != pstPBMsg->ulMsgName)
        ||(EFPBR != pstPBMsg->usEfId)||(VOS_OK != pstPBMsg->ulResult))
    {
        gstPBInitState.enPBInitState = SI_PB_INIT_STATUS_OK;

        gstPBCtrlInfo.ulADNFileNum = 0;
        gstPBCtrlInfo.usEXT1FileID = 0xFFFF;
        gucPBCStatus = VOS_FALSE;

        LogPrint1("\r\nSI_PB_InitEFpbrProc:Response Error 0x%x\r\n", (VOS_INT)pstPBMsg->usEfId);

        return;
    }

    ulPbrTotalNum =  pstPBMsg->ucTotalNum;

    if(0 != g_ulPbrRecordNum)
    {
       ulPbrTotalNum = g_ulPbrRecordNum;
       g_ulPbrRecordNum = 0;
    }

    /*����EFPBR����*/
    ulResult = SI_PB_DecodeEFPBR((VOS_UINT8)ulPbrTotalNum,
                                 (VOS_UINT8)pstPBMsg->usEfLen, pstPBMsg->aucEf);

    /*��ȡEFPBR�ļ�����ʧ�ܣ�������ADN��ʼ��������FDN�ռ�ĳ�ʼ��*/
    if ( VOS_OK != ulResult )
    {
        PB_ERROR_LOG("SI_PB_InitEFpbrProc:Decode EFPBR Content Failed");

        return;
    }

    /*��ǰ��������USIMM�ظ�*/
    gstPBInitState.enPBInitState = SI_PB_INIT_STATUS_OK;

    return;
}

VOS_UINT32 SI_PB_AllocPBSpace(VOS_UINT8 **ppPBContent,VOS_UINT32 ulPBSize,VOS_CHAR cDefaultVal)
{
    *ppPBContent = (VOS_UINT8 *)PB_MALLOC(ulPBSize);

    if (VOS_NULL_PTR == *ppPBContent)
    {
        PB_ERROR_LOG("SI_PB_AllocPBSpace:MemAlloc Failed");

        return VOS_ERR;
    }

    VOS_MemSet(*ppPBContent, cDefaultVal, ulPBSize);

    return VOS_OK;
}


VOS_UINT32 SI_PB_CheckXDNPara(PS_USIM_EFMAX_CNF_STRU *pstPBMsg)
{
    if ((PS_USIM_EF_MAX_RECORD_NUM_CNF != pstPBMsg->ulMsgName)
    ||((gstPBInitState.ausFileId[gulPBFileCnt]!= pstPBMsg->usEfId)))
    {
        gstPBInitState.enPBInitState = SI_PB_INIT_MSGTYPE_ERR;

        LogPrint1("\r\nSI_PB_InitXDNSpaceMsgProc:Response Error 0x%x\r\n", (VOS_INT)pstPBMsg->usEfId);

        return VOS_ERR;
    }

    if(VOS_OK != pstPBMsg->ulResult)
    {
        /*���ô���״̬ΪUSIMM�ظ�����*/
        gstPBInitState.enPBInitState = SI_PB_INIT_USIMPBCNF_ERR;

        if (EFBDN == pstPBMsg->usEfId)
        {
            PB_NORMAL_LOG("SI_PB_InitXDNSpaceMsgProc:EFBDN Not Exist");
        }
        else
        {
            PB_ERROR_LOG("SI_PB_InitXDNSpaceMsgProc:Intial Step Error");
        }

        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_VOID SI_PB_CardStatusInd(VOS_VOID)
{
    PS_USIM_STATUS_IND_STRU   *pUsimMsg;
    VOS_UINT8                 ucCardType;
    VOS_UINT8                 ucCardStatus;

    pUsimMsg = (PS_USIM_STATUS_IND_STRU *)VOS_AllocMsg(WUEPS_PID_USIM, sizeof(PS_USIM_STATUS_IND_STRU)-VOS_MSG_HEAD_LENGTH);

    if( VOS_NULL_PTR == pUsimMsg )
    {
        vos_printf("USIMM_CardStatusInd: VOS_AllocMsg is Failed\r\n");

        return;
    }

    USIMM_GetCardType(&ucCardStatus, &ucCardType);

    pUsimMsg->ulMsgName     = PS_USIM_GET_STATUS_IND;    /* ������Ϣ���� */
    pUsimMsg->enCardType    = ucCardType;
    pUsimMsg->enCardStatus  = ucCardStatus;
    pUsimMsg->ulReceiverPid = MAPS_PB_PID;

    (VOS_VOID)VOS_SendMsg(pUsimMsg->ulSenderPid, pUsimMsg);

    return;
}
VOS_UINT32 SI_PB_XDNSpaceCheck(VOS_UINT32 ulPBType, PS_USIM_EFMAX_CNF_STRU *pstPBMsg)
{
    if(PB_ADN_CONTENT == ulPBType)
    {
       gstPBCtrlInfo.astADNInfo[gulPBFileCnt].ucRecordNum = pstPBMsg->ucRecordNum;

       gstPBCtrlInfo.astADNInfo[gulPBFileCnt].ucRecordLen = pstPBMsg->ucRecordLen;

       if((gastPBContent[ulPBType].ucRecordLen*gastPBContent[ulPBType].usTotalNum < SI_PB_XDN_SPACE)
       &&(gastPBContent[ulPBType].usTotalNum <= SI_PB_XDN_SUPPORT_NUM))
       {
          g_ulPbrRecordNum++;
       }
       else
       {
          SI_PB_ReleaseAll();

          /* �������³�ʼ����Ϣ */
          SI_PB_CardStatusInd();

          return VOS_ERR;
       }
    }

    return VOS_OK;
}
VOS_VOID SI_PB_InitXDNSpaceMsgProc(PBMsgBlock *pstMsg)
{
    PS_USIM_EFMAX_CNF_STRU    *pstPBMsg;
    VOS_UINT32                ulPBType = 0;
    VOS_UINT32                ulNextPBType = 0;
    VOS_UINT32                ulResult;

    pstPBMsg = (PS_USIM_EFMAX_CNF_STRU *)pstMsg;

    ulResult = SI_PB_CheckXDNPara(pstPBMsg);

    if( VOS_OK != ulResult )
    {
        PB_ERROR_LOG("SI_PB_InitXDNSpaceMsgProc:check XDN para error");

        return;
    }

    /*��ȡ��ǰ��ʼ���ļ���Ӧ�ĵ绰������*/
    ulResult = SI_PB_GetXDNPBType(&ulPBType, gstPBInitState.ausFileId[gulPBFileCnt]);

    if( VOS_OK != ulResult )
    {
        /*���ô���״̬Ϊ�绰������δ֪����׼��������һ���ļ�������ʼ��*/
        gstPBInitState.enPBInitState = SI_PB_INIT_PBTYPE_UNKOWN;

        PB_ERROR_LOG("SI_PB_InitXDNSpaceMsgProc:Unspecified PB Type");

        return;
    }

    if((SI_PB_DEACTIVE == pstPBMsg->ulFileStatus) && (USIMM_EF_NOTREADABLEORUPDATABLE == pstPBMsg->ulFileReadUpdateFlag))
    {
        /*�Ѷ�Ӧ�绰���ļ���״̬����Ϊδ����*/
        gastPBContent[ulPBType].enActiveStatus = SI_PB_DEACTIVE;

        /*���ô���״̬Ϊ�ļ�δ���׼�������¸��绰��������ʼ��*/
        gstPBInitState.enPBInitState = SI_PB_INIT_FILE_DECATIVE;

        PB_NORMAL_LOG("SI_PB_InitXDNSpaceMsgProc:The File is Decative");

        return;
    }

    /*����USIMM�ظ���Ϣ����¼Ŀǰ���ڳ�ʼ���ĵ绰���������Ϣ*/
    gastPBContent[ulPBType].ucRecordLen = pstPBMsg->ucRecordLen;

    gastPBContent[ulPBType].usTotalNum += pstPBMsg->ucRecordNum;

    gastPBContent[ulPBType].ucNameLen   = pstPBMsg->ucRecordLen - SI_PB_EXCEPT_NAME;

    /*�����ADN�绰������Ҫ��¼ÿ��ADN�ļ��ļ�¼���ͼ�¼����*/
    if(VOS_OK != SI_PB_XDNSpaceCheck(ulPBType,pstPBMsg))
    {
       return;
    }

    /*��ȡ��һ����Ҫ��ʼ���ĵ绰��*/
    SI_PB_GetXDNPBType(&ulNextPBType, gstPBInitState.ausFileId[gulPBFileCnt+1]);

    /*ADN�ļ��ռ��ѳ�ʼ����ϣ�ʣ�µĵ绰������һ���绰����Ӧһ���ļ�*/
    if ( ((PB_ADN_CONTENT == ulPBType)&&(PB_ADN_CONTENT != ulNextPBType))
         || (PB_ADN_CONTENT != ulPBType) )
    {
        ulResult = gastPBContent[ulPBType].ucRecordLen*gastPBContent[ulPBType].usTotalNum;

        gastPBContent[ulPBType].ucNumberLen = SI_PB_NUM_LEN;

        /*������ƷΪ���е绰�����仺��ռ�,ADN�ļ����ܳ���60K(1000����¼)*/
        if ( 0 < ulResult )
        {
            if( ulResult <= SI_PB_XDN_SPACE )
            {
                gastPBContent[ulPBType].pContent = (VOS_UINT8*)PB_MALLOC(ulResult);
            }
            else
            {
                PB_ERROR_LOG("SI_PB_InitADNSpaceMsgProc:PB XDN Memory is Too Big To Support");
                gastPBContent[ulPBType].pContent = VOS_NULL_PTR;
            }

            if ( VOS_NULL_PTR == gastPBContent[ulPBType].pContent )
            {
                /*���ô���״̬Ϊ�����ڴ�ʧ��*/
                gstPBInitState.enPBInitState = SI_PB_INIT_PBMALLOC_FAIL;

                PB_ERROR_LOG("SI_PB_InitADNSpaceMsgProc:MemAlloc Failed");

                return;
            }
            VOS_MemSet(gastPBContent[ulPBType].pContent, (VOS_CHAR)0xFF, ulResult);
        }

        /*���ܻ����Ƿ���ڶ�Ϊ����������ռ䣬��־ÿ����¼��ʹ�����*/
        if (0 < ulResult)
        {
            if ( VOS_OK != SI_PB_AllocPBSpace(&gastPBContent[ulPBType].pIndex,
                (gastPBContent[ulPBType].usTotalNum+7)/8,0x00))
            {
                /*���ô���״̬Ϊ�����ڴ�ʧ��*/
                gstPBInitState.enPBInitState = SI_PB_INIT_PBMALLOC_FAIL;

                PB_ERROR_LOG("SI_PB_InitADNSpaceMsgProc:Index MemAlloc Failed");

                return;
            }
        }
    }

    /*���ô���״̬Ϊ�ɹ�����*/
    gstPBInitState.enPBInitState = SI_PB_INIT_STATUS_OK;

    return;
}
VOS_VOID SI_PB_InitANRSpaceMsgProc(PBMsgBlock *pstMsg)
{
    PS_USIM_EFMAX_CNF_STRU    *pstPBMsg;
    VOS_UINT8                 ucXSuffix; /*��ǰANR�ļ��ڻ����е�x�±�*/
    VOS_UINT8                 ucYSuffix; /*��ǰANR�ļ��ڻ����е�y�±�*/
    VOS_UINT32                ulTotalLen;

    pstPBMsg = (PS_USIM_EFMAX_CNF_STRU *)pstMsg;

    if (PS_USIM_EF_MAX_RECORD_NUM_CNF != pstPBMsg->ulMsgName)
    {
        gstPBInitState.enPBInitState = SI_PB_INIT_MSGTYPE_ERR;

        LogPrint("\r\nSI_PB_InitANRSpaceMsgProc:Wrong Msg\r\n");

        return;
    }

    if(VOS_OK != SI_PB_GetANRSuffix(&ucXSuffix,&ucYSuffix,pstPBMsg->usEfId))
    {
        gstPBInitState.enPBInitState = SI_PB_INIT_PBTYPE_UNKOWN;

        PB_ERROR_LOG("SI_PB_InitANRSpaceMsgProc:PB Type Error");

        return;
    }

    /*����������ʼ��״̬����*/
    if(VOS_OK != pstPBMsg->ulResult)
    {
        /*���ô���״̬ΪUSIMM�ظ�����*/
        gstPBInitState.enPBInitState = SI_PB_INIT_USIMSPBCNF_ERR;

        VOS_MemSet(&gastANRContent[ucYSuffix], 0, sizeof(SI_ANR_CONTENT_STRU));

        PB_ERROR_LOG("SI_PB_InitANRSpaceMsgProc:Intial Step Error");

        return;
    }

    /*����USIMM�ظ���Ϣ����¼Ŀǰ���ڳ�ʼ���ĵ绰���������Ϣ*/
    gastANRContent[ucYSuffix].ucRecordLen = pstPBMsg->ucRecordLen;

    gastANRContent[ucYSuffix].usTotalNum += pstPBMsg->ucRecordNum;

    gastANRContent[ucYSuffix].ucNumberLen = SI_PB_NUM_LEN;

    /*��Ҫ��¼ÿ��ANR�ļ��ļ�¼���ͼ�¼����*/
    gstPBCtrlInfo.astANRInfo[ucXSuffix][ucYSuffix].ucRecordNum = pstPBMsg->ucRecordNum;

    gstPBCtrlInfo.astANRInfo[ucXSuffix][ucYSuffix].ucRecordLen = pstPBMsg->ucRecordLen;

    /*�жϴ���ANR�Ƿ��Ѿ���ʼ�����*/
    if(VOS_OK == SI_PB_GheckANRLast(ucXSuffix,ucYSuffix))
    {
        ulTotalLen = gastANRContent[ucYSuffix].usTotalNum*gastANRContent[ucYSuffix].ucRecordLen;

        /*ANR��3�飬ÿ����¼17�ֽڣ�1000����¼����51K�ڴ�*/
        if(ulTotalLen <= 51000)
        {
            gastANRContent[ucYSuffix].pContent = (VOS_UINT8*)PB_MALLOC(ulTotalLen);
        }
        else
        {
            PB_ERROR_LOG("SI_PB_InitANRSpaceMsgProc:PB ANR Memery is Too Big To Support");
            gastANRContent[ucYSuffix].pContent = VOS_NULL_PTR;
        }

        if ( VOS_NULL_PTR == gastANRContent[ucYSuffix].pContent )
        {
            /*���ô���״̬Ϊ�����ڴ�ʧ��*/
            gstPBInitState.enPBInitState = SI_PB_INIT_SPBMALLOC_FAIL;

            PB_ERROR_LOG("SI_PB_InitANRSpaceMsgProc:MemAlloc Failed");

            return;
        }

        VOS_MemSet(gastANRContent[ucYSuffix].pContent, (VOS_CHAR)0xFF, ulTotalLen);
    }

    /*���ô���״̬Ϊ�ɹ�����*/
    gstPBInitState.enPBInitState = SI_PB_INIT_STATUS_OK;

    return;
}


VOS_VOID SI_PB_InitEMLSpaceMsgProc(PBMsgBlock *pstMsg)
{
    PS_USIM_EFMAX_CNF_STRU    *pstPBMsg;
    VOS_UINT32                ulSuffix; /*��ǰANR�ļ��ڻ����е��±�*/
    VOS_UINT32                ulTotalLen;

    ulSuffix = gulPBFileCnt;/*ANR�ļ�������Ϣ�����X�����ӦADN�ļ�������*/

    pstPBMsg = (PS_USIM_EFMAX_CNF_STRU *)pstMsg;

    if (PS_USIM_EF_MAX_RECORD_NUM_CNF != pstPBMsg->ulMsgName)
    {
        gstPBInitState.enPBInitState = SI_PB_INIT_MSGTYPE_ERR;

        LogPrint("\r\nSI_PB_InitEMLSpaceMsgProc:Wrong Msg\r\n");

        return;
    }

    /*����������ʼ��״̬����*/
    if(VOS_OK != pstPBMsg->ulResult)
    {
        /*���ô���״̬ΪUSIMM�ظ����󣬿�������Ϊ�ļ������Ͳ����ڡ�����������ð�����������������һ��ȥ*/
        gstPBInitState.enPBInitState = SI_PB_INIT_STATUS_OK;

        gstPBCtrlInfo.ulEMLFileNum = 0;

        PB_ERROR_LOG("SI_PB_InitEMLSpaceMsgProc:Init EML Space Error");

        return;
    }

    /*����USIMM�ظ���Ϣ����¼Ŀǰ���ڳ�ʼ���ĵ绰���������Ϣ*/
    gstEMLContent.ucRecordLen = pstPBMsg->ucRecordLen;

    gstEMLContent.usTotalNum += pstPBMsg->ucRecordNum;

    gstEMLContent.ucDataLen = (PB_FILE_TYPE1 == gstPBCtrlInfo.astEMLInfo[ulSuffix].enEMLType)?
                               pstPBMsg->ucRecordLen:(pstPBMsg->ucRecordLen-2);

    /*��Ҫ��¼ÿ��ANR�ļ��ļ�¼���ͼ�¼����*/
    gstPBCtrlInfo.astEMLInfo[ulSuffix].ucRecordNum = pstPBMsg->ucRecordNum;

    gstPBCtrlInfo.astEMLInfo[ulSuffix].ucRecordLen = pstPBMsg->ucRecordLen;

    /*�ж��Ƿ�����Email�ռ��Ѿ���ʼ�����*/
    if((ulSuffix+1) == gstPBCtrlInfo.ulEMLFileNum)
    {
        ulTotalLen = gstEMLContent.usTotalNum*gstEMLContent.ucRecordLen;

        /*EMAIL��1000����¼64�ֽڷ����ڴ�*/
        if(ulTotalLen <= 64000)
        {
            gstEMLContent.pContent = (VOS_UINT8 *)PB_MALLOC(ulTotalLen);
        }
        else
        {
            PB_ERROR_LOG("SI_PB_InitEMLSpaceMsgProc:PB Email Memery is Too Big To Support");
            gstEMLContent.pContent = VOS_NULL_PTR;
        }

        if ( VOS_NULL_PTR == gstEMLContent.pContent )
        {
            /*���ô���״̬Ϊ�����ڴ�ʧ��*/
            gstPBInitState.enPBInitState = SI_PB_INIT_SPBMALLOC_FAIL;

            PB_ERROR_LOG("SI_PB_InitEMLSpaceMsgProc:MemAlloc Failed");

            return;
        }

        VOS_MemSet(gstEMLContent.pContent, (VOS_CHAR)0xFF, ulTotalLen);
    }

    /*���ô���״̬Ϊ�ɹ�����*/
    gstPBInitState.enPBInitState = SI_PB_INIT_STATUS_OK;

    return;
}


VOS_VOID SI_PB_InitIAPSpaceMsgProc(PBMsgBlock *pstMsg)
{
    PS_USIM_EFMAX_CNF_STRU    *pstPBMsg;
    VOS_UINT32                ulSuffix; /*��ǰIAP�ļ��ڻ����е��±�*/
    VOS_UINT32                ulTotalLen;

    ulSuffix = gulPBFileCnt;/*IAP�ļ�������Ϣ�����X�����ӦADN�ļ�������*/

    pstPBMsg = (PS_USIM_EFMAX_CNF_STRU *)pstMsg;

    if (PS_USIM_EF_MAX_RECORD_NUM_CNF != pstPBMsg->ulMsgName)
    {
        gstPBInitState.enPBInitState = SI_PB_INIT_MSGTYPE_ERR;

        LogPrint("\r\nSI_PB_InitIAPSpaceMsgProc:Wrong Msg\r\n");

        return;
    }

    /*����������ʼ��״̬����*/
    if(VOS_OK != pstPBMsg->ulResult)
    {
        /*���ô���״̬ΪUSIMM�ظ�����*/
        gstPBInitState.enPBInitState = SI_PB_INIT_USIMSPBCNF_ERR;

        PB_ERROR_LOG("SI_PB_InitEMLSpaceMsgProc:PB Type Error");

        return;
    }

    /*����USIMM�ظ���Ϣ����¼Ŀǰ���ڳ�ʼ���ĵ绰���������Ϣ*/
    gstIAPContent.ucRecordLen = pstPBMsg->ucRecordLen;

    gstIAPContent.usTotalNum += pstPBMsg->ucRecordNum;

    /*��Ҫ��¼ÿ��IAP�ļ��ļ�¼���ͼ�¼����*/
    gstPBCtrlInfo.astIAPInfo[ulSuffix].ucRecordNum = pstPBMsg->ucRecordNum;

    gstPBCtrlInfo.astIAPInfo[ulSuffix].ucRecordLen = pstPBMsg->ucRecordLen;

    /*�ж��Ƿ�����IAP�ռ��Ѿ���ʼ�����*/
    if((ulSuffix+1) == gstPBCtrlInfo.ulIAPFileNum)
    {
        ulTotalLen = gstIAPContent.usTotalNum*gstIAPContent.ucRecordLen;

        gstIAPContent.pIAPContent = (VOS_UINT8 *)PB_MALLOC(ulTotalLen);

        if ( VOS_NULL_PTR == gstIAPContent.pIAPContent )
        {
            /*���ô���״̬Ϊ�����ڴ�ʧ��*/
            gstPBInitState.enPBInitState = SI_PB_INIT_SPBMALLOC_FAIL;

            PB_ERROR_LOG("SI_PB_InitEMLSpaceMsgProc:MemAlloc Failed");

            return;
        }

        VOS_MemSet(gstIAPContent.pIAPContent, (VOS_CHAR)0xFF, ulTotalLen);
    }

    /*���ô���״̬Ϊ�ɹ�����*/
    gstPBInitState.enPBInitState = SI_PB_INIT_STATUS_OK;

    return;
}
VOS_VOID SI_PB_InitEXTSpaceMsgProc(PBMsgBlock *pstMsg)
{
    PS_USIM_EFMAX_CNF_STRU    *pstPBMsg;
    VOS_UINT32                ulFileSize;
    VOS_UINT32                i;

    pstPBMsg = (PS_USIM_EFMAX_CNF_STRU *)pstMsg;

    if (PS_USIM_EF_MAX_RECORD_NUM_CNF != pstPBMsg->ulMsgName)
    {
        gstPBInitState.enPBInitState = SI_PB_INIT_MSGTYPE_ERR;

        LogPrint("\r\nSI_PB_InitEXTSpaceMsgProc:Wrong Msg\r\n");

        return;
    }

    if(VOS_OK != pstPBMsg->ulResult)
    {
        /*���ô���״̬Ϊ�绰������δ֪����׼��������һ���ļ�������ʼ��*/
        gstPBInitState.enPBInitState = SI_PB_INIT_PBTYPE_UNKOWN;
        gastEXTContent[gulPBFileCnt+1].pExtContent = VOS_NULL_PTR;

        PB_ERROR1_LOG("SI_PB_InitEXTSpaceMsgProc:EXT File Not Exist.",(long)(pstPBMsg->usEfId));

        return;
    }

    ulFileSize = (pstPBMsg->ucRecordNum) * (pstPBMsg->ucRecordLen);

    gastEXTContent[gulPBFileCnt+1].pExtContent = (VOS_UINT8 *)PB_MALLOC(ulFileSize);

    if ( VOS_NULL_PTR == gastEXTContent[gulPBFileCnt+1].pExtContent )
    {
        gstPBInitState.enPBInitState = SI_PB_INIT_PBMALLOC_FAIL;
        gastEXTContent[gulPBFileCnt+1].usExtFlag   = SI_PB_FILE_NOT_EXIST;

        PB_ERROR_LOG("SI_PB_InitEXTSpaceMsgProc:MemAlloc Failed");

        return;
    }

    /* EXT�ļ��ļ�¼���������ж����ʼ��������Ҫ��EXT�ļ��Ļ���ռ�ȫ��������ΪFF */
    VOS_MemSet(gastEXTContent[gulPBFileCnt+1].pExtContent, (VOS_INT8)0xFF, ulFileSize);

    gastEXTContent[gulPBFileCnt+1].usExtFlag     = SI_PB_FILE_EXIST;
    gastEXTContent[gulPBFileCnt+1].usExtTotalNum = pstPBMsg->ucRecordNum;

    gastPBContent[gulPBFileCnt+1].ucNumberLen    += SI_PB_EXT_NUM_LEN;

    /*����ANR�ļ�֧����չ����*/
    if( pstPBMsg->usEfId == gstPBCtrlInfo.usEXT1FileID )
    {
        for(i = 0; i < SI_PB_ANRMAX; i++)
        {
            gastANRContent[i].ucNumberLen +=SI_PB_NUM_LEN;
        }
    }

    /* ����SIM����ADN��MSISDN��EXT�ļ���Ӧ��ͬһ�ļ�*/
    if( EFEXT1 == pstPBMsg->usEfId )
    {
        gastEXTContent[PB_MSISDN_CONTENT].usExtFlag     = SI_PB_FILE_EXIST;
        gastEXTContent[PB_MSISDN_CONTENT].usExtTotalNum = pstPBMsg->ucRecordNum;
        gastEXTContent[PB_MSISDN_CONTENT].pExtContent   = gastEXTContent[PB_ADN_CONTENT].pExtContent;

        gastPBContent[PB_MSISDN_CONTENT].ucNumberLen    = SI_PB_EXT_NUM_LEN;
    }

    /*���ô���״̬Ϊ�ɹ�����*/
    gstPBInitState.enPBInitState = SI_PB_INIT_STATUS_OK;

    return;
}
VOS_VOID SI_PB_InitIAPContentMsgProc(PBMsgBlock *pstMsg)
{
    PS_USIM_GET_FILE_CNF_STRU    *pstPBMsg;
    VOS_UINT32                   ulOffset;
    VOS_UINT16                   usRecordNum;
    VOS_UINT8                    ucTotalRecordNum;
    VOS_UINT8                    i;

    pstPBMsg = (PS_USIM_GET_FILE_CNF_STRU *)pstMsg;

    if (PS_USIM_GET_FILE_RSP != pstPBMsg->ulMsgName)
    {
        gstPBInitState.enPBInitState = SI_PB_INIT_MSGTYPE_ERR;

        LogPrint("\r\nSI_PB_InitIAPContentMsgProc:Wrong Msg\r\n");

        return;
    }

    /*����ظ���FID���ǵ�ǰ���ڳ�ʼ���ĵ绰����FID�����ʼ���������*/
    if ( VOS_OK != pstPBMsg->ulResult )
    {
        PB_ERROR_LOG("SI_PB_InitIAPContentMsgProc:Get File Content Failed");

        if ( VOS_NULL_PTR != gstIAPContent.pIAPContent)
        {
            PB_FREE( gstIAPContent.pIAPContent);
        }

        /*���ô���״̬ΪUSIMM�ظ�����*/
        gstPBInitState.enPBInitState = SI_PB_INIT_USIMSPBCNF_ERR;

        return;
    }

    if ( VOS_NULL_PTR == gstIAPContent.pIAPContent )
    {
        /*���ô���״̬Ϊ�����ڴ�ʧ��*/
        gstPBInitState.enPBInitState = SI_PB_INIT_PBMALLOC_FAIL;

        PB_ERROR_LOG("SI_PB_InitIAPContentMsgProc:Memory is Null");

        return;
    }

    /* ÿ��IAP�ļ��ļ�¼�����ܲ�һ�£�����ƫ����ʱҪ�ۼ� */
    usRecordNum = pstPBMsg->ucRecordNum;

    for (i = 0; i < gulPBFileCnt; i++)
    {
        usRecordNum += gstPBCtrlInfo.astIAPInfo[i].ucRecordNum;
    }

    ulOffset = (usRecordNum - 1) * pstPBMsg->usEfLen;

    VOS_MemCpy(gstIAPContent.pIAPContent + ulOffset,\
                                        pstPBMsg->aucEf, pstPBMsg->usEfLen);

    ucTotalRecordNum = gstPBCtrlInfo.astIAPInfo[gulPBFileCnt].ucRecordNum;

    if (++gulPBRecordCnt > ucTotalRecordNum)/*��ǰ��ʼ���ļ��ļ�¼��ȫ����ʼ�����*/
    {
        gstPBInitState.enPBInitState = SI_PB_INIT_RECORD_EXCEED;
    }
    else
    {
        /*���ô���״̬Ϊ����δ��ʼ���ļ�¼*/
        gstPBInitState.enPBInitState = SI_PB_INIT_RECORD_REMAIN;
    }

    return;
}


VOS_VOID SI_PB_InitXDNContentMsgProc(PBMsgBlock *pstMsg)
{
    PS_USIM_GET_FILE_CNF_STRU    *pstPBMsg;
    VOS_UINT32                   ulPBType = 0;
    VOS_UINT32                   ulNextPBType = 0;
    VOS_UINT32                   ulResult;
    VOS_UINT32                   ulOffset;
    VOS_UINT32                   ulExtInfoNum;
    VOS_UINT16                   usExtRecIdOffset;
    VOS_UINT16                   usIndex;/*��¼ADN�绰����¼�Ŷ�Ӧ�Ļ���������*/
    VOS_UINT8                    ucRecordNum;

    pstPBMsg = (PS_USIM_GET_FILE_CNF_STRU *)pstMsg;

    if (PS_USIM_GET_FILE_RSP != pstPBMsg->ulMsgName)
    {
        gstPBInitState.enPBInitState = SI_PB_INIT_MSGTYPE_ERR;

        LogPrint("\r\nSI_PB_InitXDNContentMsgProc:Wrong Msg\r\n");

        return;
    }

    /*��ȡ��ǰ��ʼ�����ļ���Ӧ�ĵ绰��*/
    ulResult = SI_PB_GetXDNPBType(&ulPBType, gstPBInitState.ausFileId[gulPBFileCnt]);

    if ( VOS_OK != ulResult )
    {
        /*���ô���״̬Ϊ�绰������δ֪*/
        gstPBInitState.enPBInitState = SI_PB_INIT_PBTYPE_UNKOWN;

        PB_ERROR_LOG("SI_PB_InitXDNSpaceMsgProc:Unspecified PB Type");

        return;
    }

    /* SSIM���ظ���������ת���¸��绰������ */
    if  (VOS_OK != pstPBMsg->ulResult)
    {
        PB_WARNING_LOG("SI_PB_InitXDNContentMsgProc:Intial File Error");

        /*���ô���״̬ΪUSIMM�ظ�����*/
        gstPBInitState.enPBInitState = SI_PB_INIT_USIMPBCNF_ERR;

        return;
    }

    /*��ȡ�ظ��ļ�¼�����ڻ����е�λ�ã�ADN�������绰���ֱ���*/
    if ( PB_ADN_CONTENT == ulPBType )
    {
        /*��ȡ��ǰ���ļ��ͼ�¼�Ŷ�Ӧ�Ļ���������*/
        if (VOS_OK != SI_PB_CountADNIndex(pstPBMsg->usEfId,\
                                        pstPBMsg->ucRecordNum, &usIndex))
        {
            /*���ô���״̬Ϊ��ǰ�ļ����м�¼�������*/
            gstPBInitState.enPBInitState = SI_PB_INIT_RECORD_EXCEED;

            PB_ERROR_LOG("SI_PB_InitXDNContentMsgProc:Get Record Index Failed");

            return;
        }

        ucRecordNum = gstPBCtrlInfo.astADNInfo[gulPBFileCnt].ucRecordNum;
    }
    else
    {
        usIndex = (VOS_UINT16)gulPBRecordCnt;

        ucRecordNum = (VOS_UINT8)gastPBContent[ulPBType].usTotalNum;
    }

    ulOffset = (usIndex - 1)*gastPBContent[ulPBType].ucRecordLen;

    if ( VOS_NULL_PTR == gastPBContent[ulPBType].pContent )
    {
        /* ���ô���״̬Ϊ�����ڴ�ʧ�ܣ���һ������ʹ��SEARCH�ķ�ʽ���г�ʼ�� */
        gstPBInitState.enPBInitState = SI_PB_INIT_PBMALLOC_FAIL;
        gstPBInitState.enPBInitStep  = PB_INIT_XDN_SEARCH;

        return;
    }

    /*����XDN�绰���ļ�¼������*/
    VOS_MemCpy(gastPBContent[ulPBType].pContent + ulOffset,
               pstPBMsg->aucEf, gastPBContent[ulPBType].ucRecordLen);

    /*���˼�¼�Ƿ�Ϊ��Ч��¼*/
    if ( VOS_OK == SI_PB_CheckContentValidity(&gastPBContent[ulPBType], pstPBMsg->aucEf))
    {
        gastPBContent[ulPBType].usUsedNum++;

        if (SI_PB_CONTENT_INVALID == SI_PB_GetBitFromBuf(gastPBContent[ulPBType].pIndex,
                                                        usIndex))
        {
            /*���֮ǰ�����ͺ����ֶ�Ϊ�գ�����ǰ������ADN��Ϊ�գ���������ϼ�¼��Ϊ�ǿ�*/
            if(PB_ADN_CONTENT == ulPBType)
            {
                gstPBCtrlInfo.ulTotalUsed++;
            }

            SI_PB_SetBitToBuf(gastPBContent[ulPBType].pIndex,usIndex,SI_PB_CONTENT_VALID);
        }
    }

    usExtRecIdOffset = ((EFBDN == pstPBMsg->usEfId)?\
                        (pstPBMsg->usEfLen - 2):(pstPBMsg->usEfLen - 1));

    ulExtInfoNum = gastPBContent[ulPBType].ulExtInfoNum;

    /* ����Ƿ���EXT�ļ�Ҫ��ȡ��XDN�ļ�����չ��¼�ļ�¼�Ų���Ϊ0��0xFF */
    if( ( 0xFF != pstPBMsg->aucEf[usExtRecIdOffset] )
        && (0 != pstPBMsg->aucEf[usExtRecIdOffset])
        && (SI_PB_FILE_NOT_EXIST != gastEXTContent[ulExtInfoNum].usExtFlag) )
    {
        gstPBInitState.enPBInitState = SI_PB_INIT_EXT_RECORD;
        gstPBInitState.enPBInitStep  = PB_INIT_EXT_CONTENT;
        gulExtRecord                 = pstPBMsg->aucEf[usExtRecIdOffset];
        return;
    }

    if ( ++gulPBRecordCnt > ucRecordNum )/*��ǰ��ʼ���ļ��ļ�¼��ȫ����ʼ�����*/
    {
        /*��ȡ��һ����Ҫ�������ݵĵ绰��*/
        SI_PB_GetXDNPBType(&ulNextPBType, gstPBInitState.ausFileId[gulPBFileCnt+1]);

        /*lint -e774 �޸���:ף�; ������:���� 51137;ԭ��:�����û��������� */
        /*��������һ��ADN�ļ������ADN�绰��<һ���ļ���Ӧһ���绰��>*/
        if ( (PB_ADN_CONTENT != ulPBType) ||
             ((PB_ADN_CONTENT == ulPBType)&&(PB_ADN_CONTENT != ulNextPBType)))
        {
            /*���õ�ǰ�绰����ʼ��״̬Ϊ�ѳ�ʼ��*/
            gastPBContent[ulPBType].enInitialState = PB_INITIALISED;
        }
        /*lint +e774 �޸���:ף�;������:���� 51137; */

        /*���ô���״̬Ϊ��ǰ�ļ������м�¼�ѳ�ʼ�����*/
        gstPBInitState.enPBInitState = SI_PB_INIT_RECORD_EXCEED;
    }
    else
    {
        /*���ô���״̬Ϊ����δ��ʼ���ļ�¼*/
        gstPBInitState.enPBInitState = SI_PB_INIT_RECORD_REMAIN;
    }

    return;
}
VOS_VOID SI_PB_InitEXTContentMsgProc(PBMsgBlock *pstMsg)
{
    PS_USIM_GET_FILE_CNF_STRU    *pstPBMsg;
    VOS_UINT32                   ulOffset;
    VOS_UINT32                   ulPBType;
    VOS_UINT32                   ulNextPBType;
    VOS_UINT32                   ulResult1;
    VOS_UINT32                   ulResult2;
    VOS_UINT8                    ucRecordNum;
    VOS_UINT32                   ulExtInfoNum;

    pstPBMsg = (PS_USIM_GET_FILE_CNF_STRU *)pstMsg;

    if (PS_USIM_GET_FILE_RSP != pstPBMsg->ulMsgName)
    {
        gstPBInitState.enPBInitState = SI_PB_INIT_MSGTYPE_ERR;

        LogPrint("\r\nSI_PB_InitEXTContentMsgProc:Wrong Msg\r\n");

        return;
    }

    /*��ȡ��ǰ��ʼ�����ļ���Ӧ�ĵ绰���ļ�����*/
    if ( VOS_OK != SI_PB_GetXDNPBType(&ulPBType, gstPBInitState.ausFileId[gulPBFileCnt]) )
    {
        /*���ô���״̬Ϊ�绰������δ֪*/
        gstPBInitState.enPBInitState = SI_PB_INIT_PBTYPE_UNKOWN;

         /*AT2D15844*/
        gstPBInitState.enPBInitStep = PB_INIT_XDN_CONTENT;

        PB_ERROR_LOG("SI_PB_InitXDNSpaceMsgProc:Unspecified PB Type");

        return;
    }

    /*��ȡ�ظ��ļ�¼�����ڻ����е�λ�ã�ADN�������绰����EXT�ļ��ֱ���*/
    if ( PB_ADN_CONTENT == ulPBType )
    {
        ucRecordNum = gstPBCtrlInfo.astADNInfo[gulPBFileCnt].ucRecordNum;
    }
    else
    {
        ucRecordNum = (VOS_UINT8)gastPBContent[ulPBType].usTotalNum;
    }

    ulResult1 = (gstPBSearchCtrlInfo.usReadOffset >= gstPBSearchCtrlInfo.usReadNum);    /* ��ǰSEARCH��¼��ȫ����ʼ����� */
    ulResult2 = (++gulPBRecordCnt > ucRecordNum);                                       /* ��ǰ��ʼ���ļ��ļ�¼��ȫ����ʼ����� */

    /* �����ǰ��ȡ��¼�Ѿ�������ǰ�ļ���¼����Ҫ�л�����һ���ļ� */
    if (ulResult1 || ulResult2)
    {
        /*��ȡ��һ����Ҫ�������ݵĵ绰��*/
        SI_PB_GetXDNPBType(&ulNextPBType, gstPBInitState.ausFileId[gulPBFileCnt+1]);

        /*lint -e774 �޸���:ף�; ������:���� 51137;ԭ��:�����û��������� */

        /*��������һ��ADN�ļ������ADN�绰��<һ���ļ���Ӧһ���绰��>*/
        if ( (PB_ADN_CONTENT != ulPBType) ||
             ((PB_ADN_CONTENT == ulPBType)&&(PB_ADN_CONTENT != ulNextPBType)))
        {
            /*���õ�ǰ�绰����ʼ��״̬Ϊ�ѳ�ʼ��*/
            gastPBContent[ulPBType].enInitialState = PB_INITIALISED;
        }
        /*lint +e774 �޸���:ף�;������:���� 51137; */

        /*���ô���״̬Ϊ��ǰ�ļ������м�¼�ѳ�ʼ�����*/
        gstPBInitState.enPBInitState = SI_PB_INIT_RECORD_EXCEED;
    }
    else
    {
        /*���ô���״̬Ϊ����δ��ʼ���ļ�¼*/
        gstPBInitState.enPBInitState = SI_PB_INIT_RECORD_REMAIN;
    }

    /*��һ����ʼ���ļ�ΪXDN�ļ�*/
    /*gstPBInitState.enPBInitStep = PB_INIT_XDN_CONTENT;
 */

    /* ��һ����ʼ���ļ�ΪXDN�ļ������ݲ���״̬���ж�
    ��ʹ��������ȡ�ķ�ʽ�����ò��ҵķ�ʽ����ʼ�� */
    if (PB_SEARCH_ENABLE  == gstPBInitState.enPBSearchState)
    {
        gstPBInitState.enPBInitStep = PB_INIT_XDN_CONTENT2;
    }
    else
    {
        gstPBInitState.enPBInitStep = PB_INIT_XDN_CONTENT;
    }

    if ( VOS_OK != pstPBMsg->ulResult )
    {
        /* ����ڳ�ʼ��EXT�ļ��Ĺ����г��������ȡʧ�ܣ�������������XDN�ĳ�ʼ�� */
        PB_WARNING_LOG("SI_PB_InitEXTContentMsgProc:Read File Content is Failed");

        return;
    }
    else
    {
        ulOffset = (gulExtRecord - 1)*SI_PB_EXT_LEN;

        /*����EXT���ݵ����棬�����¼Ϊ��Ч��¼����ʹ�ü�¼�ż�1*/
        ulExtInfoNum = gastPBContent[ulPBType].ulExtInfoNum;

        /* coverity[overrun-buffer-arg] */
        VOS_MemCpy(gastEXTContent[ulExtInfoNum].pExtContent + ulOffset, pstPBMsg->aucEf, SI_PB_EXT_LEN);

        if( 0 != *(pstPBMsg->aucEf+1))
        {
            gastEXTContent[ulExtInfoNum].usExtUsedNum++;
        }

        return;
    }
}
VOS_VOID SI_PB_InitIAPSearchMsgProc(PBMsgBlock *pstMsg)
{
    PS_USIM_SEARCH_CNF_STRU     *pstPBMsg;

    pstPBMsg = (PS_USIM_SEARCH_CNF_STRU *)pstMsg;

    if (PS_USIM_SEARCH_CNF != pstPBMsg->ulMsgName)
    {
        gstPBInitState.enPBInitState = SI_PB_INIT_MSGTYPE_ERR;

        LogPrint("\r\nSI_PB_InitIAPSearchMsgProc:Wrong Msg\r\n");

        return;
    }

    /* ���ҵĽ��������OKҲ������NORECORD FOUND(��ƥ���¼)�������ظ�������Ǵ���� */
    if ((USIMM_SW_NORECORD_FOUND == pstPBMsg->ulResult)
        || (VOS_OK == pstPBMsg->ulResult))
    {
        gstPBSearchCtrlInfo.usReadNum       = gstPBCtrlInfo.astIAPInfo[gulPBFileCnt].ucRecordNum - pstPBMsg->ucLen;
        gstPBSearchCtrlInfo.usReadOffset    = 0;                        /* ��ȡ����ƥ���б���ƫ������0��ʼ */
        gstPBSearchCtrlInfo.usEfId          = pstPBMsg->usEfId;
        gstPBInitState.enPBInitState        = SI_PB_INIT_SEARCH_OK;
        gstPBInitState.enPBInitStep         = PB_INIT_IAP_CONTENT2;

        SI_PB_SearchResultProc(gstPBSearchCtrlInfo.aucReadString, pstPBMsg->aucContent,
                                pstPBMsg->ucLen, gstPBCtrlInfo.astIAPInfo[gulPBFileCnt].ucRecordNum);

        /* ���û��ƥ��ļ�¼�����ļ���������1��׼��������һ���ļ� */
        if (0 == gstPBSearchCtrlInfo.usReadNum)
        {
            gulPBFileCnt++;
        }
    }
    /* ���ҵ��ļ������ڣ�����ƥ��ļ�¼��Ϊ�㣬������һ�ļ��Ĳ��ҹ��� */
    else if (USIMM_SW_NOFILE_FOUND == pstPBMsg->ulResult)
    {
        gulPBFileCnt++;                                                 /* �ļ���������1��׼��������һ���ļ� */
        gstPBSearchCtrlInfo.usReadNum       = 0;
        gstPBSearchCtrlInfo.usReadOffset    = 0;                        /* ��ȡ����ƥ���б���ƫ������0��ʼ */
        gstPBInitState.enPBInitState        = SI_PB_INIT_SEARCH_OK;
        gstPBInitState.enPBInitStep         = PB_INIT_IAP_CONTENT2;
        gstPBInitState.enPBSearchState      = PB_SEARCH_ENABLE;
    }
    else
    {
        /* ���ô���״̬ΪUSIMM SEARCH�ظ����� */
        gstPBInitState.enPBInitState = SI_PB_INIT_SEARCH_ERR;

        PB_ERROR_LOG("SI_PB_InitIAPSearchMsgProc:Search Result Error.");
    }

    return;
}
VOS_VOID SI_PB_InitEmailSearchMsgProc(PBMsgBlock *pstMsg)
{
    PS_USIM_SEARCH_CNF_STRU     *pstPBMsg;
    VOS_UINT8                    ucSuffix;

    pstPBMsg = (PS_USIM_SEARCH_CNF_STRU *)pstMsg;

    if (PS_USIM_SEARCH_CNF != pstPBMsg->ulMsgName)
    {
        gstPBInitState.enPBInitState = SI_PB_INIT_MSGTYPE_ERR;

        LogPrint("\r\nSI_PB_InitEmailSearchMsgProc:Wrong Msg\r\n");

        return;
    }

    ucSuffix = (VOS_UINT8)gulPBFileCnt;

    /* ���ҵĽ��������OKҲ������NORECORD FOUND(��ƥ���¼)�������ظ�������Ǵ���� */
    if ((USIMM_SW_NORECORD_FOUND == pstPBMsg->ulResult)
        || (VOS_OK == pstPBMsg->ulResult))
    {
        gstPBSearchCtrlInfo.usReadNum       = gstPBCtrlInfo.astEMLInfo[gulPBFileCnt].ucRecordNum - pstPBMsg->ucLen;
        gstPBSearchCtrlInfo.usEfId          = pstPBMsg->usEfId;
        gstPBSearchCtrlInfo.usReadOffset    = 0;                        /* ��ȡ����ƥ���б���ƫ������0��ʼ */
        gstPBInitState.enPBInitState        = SI_PB_INIT_SEARCH_OK;     /* ״̬�����������ļ���������1��׼��������һ���ļ� */
        gstPBInitState.enPBInitStep         = PB_INIT_EML_CONTENT2;

        SI_PB_SearchResultProc(gstPBSearchCtrlInfo.aucReadString, pstPBMsg->aucContent,
                                pstPBMsg->ucLen, gstPBCtrlInfo.astEMLInfo[gulPBFileCnt].ucRecordNum);

        /* ���SEARCH�Ľ����û��Ҫ��ȡ�ļ�¼����Ҫ�жϵ�ǰ�ļ��Ƿ���EMAIL�ļ�������ļ���
           �������Ҫ����Ӧ�Ĵ洢����������Ϊ��ʼ�������. ���û��ƥ��ļ�¼�����ļ�����
           ����1��׼��������һ���ļ�*/
        if (0 == gstPBSearchCtrlInfo.usReadNum)
        {
            gulPBFileCnt++;

            if ((ucSuffix+1) == (VOS_UINT8)gstPBCtrlInfo.ulEMLFileNum)
            {
                gstEMLContent.enInitialState = PB_INITIALISED;
            }
        }
    }
    /* ���ҵ��ļ������ڣ�����ƥ��ļ�¼��Ϊ�㣬������һ�ļ��Ĳ��ҹ��� */
    else if (USIMM_SW_NOFILE_FOUND == pstPBMsg->ulResult)
    {
        gulPBFileCnt++;                                                 /* �ļ���������1��׼��������һ���ļ� */
        gstPBSearchCtrlInfo.usReadNum       = 0;
        gstPBSearchCtrlInfo.usReadOffset    = 0;                        /* ��ȡ����ƥ���б���ƫ������0��ʼ */
        gstPBInitState.enPBInitState        = SI_PB_INIT_SEARCH_OK;
        gstPBInitState.enPBInitStep         = PB_INIT_EML_CONTENT2;
        gstPBInitState.enPBSearchState      = PB_SEARCH_ENABLE;
    }
    else
    {
        /* ���ô���״̬ΪUSIMM�ظ�SEARCH����,��һ���ʼ�����账�� */
        gstPBInitState.enPBInitState = SI_PB_INIT_SEARCH_ERR;

        PB_ERROR_LOG("SI_PB_InitEmailSearchMsgProc:Search Result Error.");
    }

    return;
}


VOS_VOID SI_PB_InitANRSearchMsgProc(PBMsgBlock *pstMsg)
{
    PS_USIM_SEARCH_CNF_STRU     *pstPBMsg;
    VOS_UINT8                    ucXSuffix;
    VOS_UINT8                    ucYSuffix;

    pstPBMsg = (PS_USIM_SEARCH_CNF_STRU *)pstMsg;

    if (PS_USIM_SEARCH_CNF != pstPBMsg->ulMsgName)
    {
        gstPBInitState.enPBInitState = SI_PB_INIT_MSGTYPE_ERR;

        LogPrint("\r\nSI_PB_InitANRSearchMsgProc:Wrong Msg\r\n");

        return;
    }

    /* ��ȡ��ǰ��ʼ�����ļ���Ӧ�ĵ绰�� */
    if (VOS_OK != SI_PB_GetANRSuffix(&ucXSuffix, &ucYSuffix, pstPBMsg->usEfId))
    {
        gstPBInitState.enPBInitState = SI_PB_INIT_PBTYPE_UNKOWN;

        PB_ERROR_LOG("SI_PB_InitANRSearchMsgProc:PB Tpye Error");

        return;
    }

    /* ���ҵĽ��������OKҲ������NORECORD FOUND(��ƥ���¼)�������ظ�������Ǵ���� */
    if ((USIMM_SW_NORECORD_FOUND == pstPBMsg->ulResult)
        || (VOS_OK == pstPBMsg->ulResult))
    {
        gstPBSearchCtrlInfo.usReadNum       = gstPBCtrlInfo.astANRInfo[ucXSuffix][ucYSuffix].ucRecordNum - pstPBMsg->ucLen;
        gstPBSearchCtrlInfo.usEfId          = pstPBMsg->usEfId;
        gstPBSearchCtrlInfo.usReadOffset    = 0;                        /* ��ȡ����ƥ���б���ƫ������0��ʼ */
        gstPBInitState.enPBInitState        = SI_PB_INIT_SEARCH_OK;     /* ״̬�����������ļ���������1��׼��������һ���ļ� */
        gstPBInitState.enPBInitStep         = PB_INIT_ANR_CONTENT2;
        gstPBInitState.enPBSearchState      = PB_SEARCH_ENABLE;

        SI_PB_SearchResultProc(gstPBSearchCtrlInfo.aucReadString, pstPBMsg->aucContent,
                                pstPBMsg->ucLen, gstPBCtrlInfo.astANRInfo[ucXSuffix][ucYSuffix].ucRecordNum);

        /* ���SEARCH�Ľ����û��Ҫ��ȡ�ļ�¼����Ҫ�жϵ�ǰ�ļ��Ƿ���EMAIL�ļ�������ļ���
           �������Ҫ����Ӧ�Ĵ洢����������Ϊ��ʼ�������. ���û��ƥ��ļ�¼�����ļ�����
           ����1��׼��������һ���ļ�*/
        if (0 == gstPBSearchCtrlInfo.usReadNum)
        {
            gulPBFileCnt++;

            if (VOS_OK == SI_PB_GheckANRLast(ucXSuffix, ucYSuffix))
            {
                gstEMLContent.enInitialState = PB_INITIALISED;
                gstPBCtrlInfo.ulANRStorageNum++;
            }
        }
    }
    /* ���ҵ��ļ������ڣ�����ƥ��ļ�¼��Ϊ�㣬������һ�ļ��Ĳ��ҹ��� */
    else if (USIMM_SW_NOFILE_FOUND == pstPBMsg->ulResult)
    {
        gulPBFileCnt++;                                                 /* �ļ���������1��׼��������һ���ļ� */
        gstPBSearchCtrlInfo.usReadNum       = 0;
        gstPBSearchCtrlInfo.usReadOffset    = 0;                        /* ��ȡ����ƥ���б���ƫ������0��ʼ */
        gstPBInitState.enPBInitState        = SI_PB_INIT_SEARCH_OK;
        gstPBInitState.enPBInitStep         = PB_INIT_ANR_CONTENT2;
        gstPBInitState.enPBSearchState      = PB_SEARCH_ENABLE;
    }
    else
    {
        /* ���ô���״̬ΪUSIMM�ظ�����,����ʧ�ܣ������������ȡ�ķ�ʽ�����ANR�ļ��ĳ�ʼ�� */
        gstPBInitState.enPBInitState    = SI_PB_INIT_SEARCH_ERR;
        gstPBInitState.enPBSearchState  = PB_SEARCH_DISABLE;

        PB_ERROR_LOG("SI_PB_InitANRSearchMsgProc:Search Result Error.");
    }

    return;
}


VOS_VOID SI_PB_InitXDNSearchMsgProc(PBMsgBlock *pstMsg)
{
    PS_USIM_SEARCH_CNF_STRU     *pstPBMsg;
    VOS_UINT32                   ulPBType;
    VOS_UINT32                   ulNextPBType;
    VOS_UINT8                    ucRecordNum;

    pstPBMsg = (PS_USIM_SEARCH_CNF_STRU *)pstMsg;

    if (PS_USIM_SEARCH_CNF != pstPBMsg->ulMsgName)
    {
        gstPBInitState.enPBInitState = SI_PB_INIT_MSGTYPE_ERR;

        LogPrint("\r\nSI_PB_InitXDNSearchMsgProc:Wrong Msg\r\n");

        return;
    }

    /* ��ȡ��ǰ��ʼ�����ļ���Ӧ�ĵ绰�� */
    if ( VOS_OK != SI_PB_GetXDNPBType(&ulPBType, pstPBMsg->usEfId) )
    {
        /* ���ô���״̬Ϊ�绰������δ֪ */
        gstPBInitState.enPBInitState = SI_PB_INIT_PBTYPE_UNKOWN;

        PB_ERROR_LOG("SI_PB_InitXDNSearchMsgProc:Unspecified PB Type");

        return;
    }

    /* ���ҵĽ��������OKҲ������NORECORD FOUND(��ƥ���¼)�������ظ�������Ǵ���� */
    if ((USIMM_SW_NORECORD_FOUND == pstPBMsg->ulResult)
        || (VOS_OK == pstPBMsg->ulResult))
    {
        if (PB_ADN_CONTENT == ulPBType)
        {
            ucRecordNum = gstPBCtrlInfo.astADNInfo[gulPBFileCnt].ucRecordNum;
        }
        else
        {
            ucRecordNum = (VOS_UINT8)gastPBContent[ulPBType].usTotalNum;
        }

        gstPBSearchCtrlInfo.usReadNum       = ucRecordNum - pstPBMsg->ucLen;
        gstPBSearchCtrlInfo.usEfId          = pstPBMsg->usEfId;
        gstPBSearchCtrlInfo.usReadOffset    = 0;                        /* ��ȡ����ƥ���б���ƫ������0��ʼ */
        gstPBInitState.enPBInitState        = SI_PB_INIT_SEARCH_OK;     /* ״̬�����������ļ���������1��׼��������һ���ļ� */
        gstPBInitState.enPBInitStep         = PB_INIT_XDN_CONTENT2;
        gstPBInitState.enPBSearchState      = PB_SEARCH_ENABLE;

        SI_PB_SearchResultProc(gstPBSearchCtrlInfo.aucReadString, pstPBMsg->aucContent,
                                pstPBMsg->ucLen, ucRecordNum);

        /* ���SEARCH�Ľ����û��Ҫ��ȡ�ļ�¼����Ҫ�жϵ�ǰ�ļ��Ƿ���XDN�ļ�������ļ���
           �������Ҫ����Ӧ�Ĵ洢����������Ϊ��ʼ������� */
        if (0 == gstPBSearchCtrlInfo.usReadNum)
        {
            gulPBFileCnt++;

            /* ��ȡ��һ����Ҫ�������ݵĵ绰�� */
            SI_PB_GetXDNPBType(&ulNextPBType, gstPBInitState.ausFileId[gulPBFileCnt]);

            /********** ��������һ��ADN�ļ������ADN�绰��<һ���ļ���Ӧһ���绰�� *************/
            if (PB_ADN_CONTENT != ulPBType)
            {
                /* ���õ�ǰ�绰����ʼ��״̬Ϊ�ѳ�ʼ�� */
                gastPBContent[ulPBType].enInitialState = PB_INITIALISED;
            }
            else if(PB_ADN_CONTENT != ulNextPBType)
            {
                /* ���õ�ǰ�绰����ʼ��״̬Ϊ�ѳ�ʼ�� */
                gastPBContent[ulPBType].enInitialState = PB_INITIALISED;
            }
            else
            {
                ;
            }
        }
    }
    /* ���ҵ��ļ������ڣ�����ƥ��ļ�¼��Ϊ�㣬������һ�ļ��Ĳ��ҹ��� */
    else if (USIMM_SW_NOFILE_FOUND == pstPBMsg->ulResult)
    {
        gulPBFileCnt++;                                                 /* �ļ���������1��׼��������һ���ļ� */
        gstPBSearchCtrlInfo.usReadNum       = 0;
        gstPBSearchCtrlInfo.usReadOffset    = 0;                        /* ��ȡ����ƥ���б���ƫ������0��ʼ */
        gstPBInitState.enPBInitState        = SI_PB_INIT_SEARCH_OK;
        gstPBInitState.enPBInitStep         = PB_INIT_XDN_CONTENT2;
        gstPBInitState.enPBSearchState      = PB_SEARCH_ENABLE;

    }
    else
    {
        /* ���ô���״̬ΪUSIMM�ظ�����,����ʧ�ܣ������������ȡ�ķ�ʽ�����XDN�ļ��ĳ�ʼ�� */
        gstPBInitState.enPBInitState    = SI_PB_INIT_SEARCH_ERR;
        gstPBInitState.enPBSearchState  = PB_SEARCH_DISABLE;

        PB_ERROR_LOG("SI_PB_InitXDNSearchMsgProc:Search Result Error.");
    }

    return;
}


VOS_VOID SI_PB_InitPBCSearchMsgProc(PBMsgBlock *pstMsg)
{
    PS_USIM_SEARCH_CNF_STRU     *pstPBMsg;

    pstPBMsg = (PS_USIM_SEARCH_CNF_STRU *)pstMsg;

    if (PS_USIM_SEARCH_CNF != pstPBMsg->ulMsgName)
    {
        gstPBInitState.enPBInitState = SI_PB_INIT_MSGTYPE_ERR;

        LogPrint("\r\nSI_PB_InitPBCSearchMsgProc:Wrong Msg\r\n");

        return;
    }

    /* ���ҵĽ��������OKҲ������NORECORD FOUND(��ƥ���¼)�������ظ�������Ǵ���� */
    if ((USIMM_SW_NORECORD_FOUND == pstPBMsg->ulResult)
        || (VOS_OK == pstPBMsg->ulResult))
    {
        /* ֻ����SEARCH�Ĵ��������и���CNF��Ϣ�д��ص�PBC��¼����������
           �ڷ�SEARCH�Ĵ��������л�����ADN�ļ�¼�����ж� */
        gstPBCtrlInfo.usPBCRecordNum        = pstPBMsg->usTotalRecNum;

        gstPBSearchCtrlInfo.usReadNum       = gstPBCtrlInfo.usPBCRecordNum - pstPBMsg->ucLen;
        gstPBSearchCtrlInfo.usEfId          = pstPBMsg->usEfId;
        gstPBSearchCtrlInfo.usReadOffset    = 0;                        /* ��ȡ����ƥ���б���ƫ������0��ʼ */
        gstPBInitState.enPBInitState        = SI_PB_INIT_SEARCH_OK;     /* ״̬�����������ļ���������1��׼��������һ���ļ� */
        gstPBInitState.enPBInitStep         = PB_INIT_PBC_CONTENT2;
        gstPBInitState.enPBSearchState      = PB_SEARCH_ENABLE;

        SI_PB_SearchResultProc(gstPBSearchCtrlInfo.aucReadString, pstPBMsg->aucContent,
                                pstPBMsg->ucLen, (VOS_UINT8)gstPBCtrlInfo.usPBCRecordNum);
    }
    /* ���ҵ��ļ������ڣ�����ƥ��ļ�¼��Ϊ�㣬������һ�ļ��Ĳ��ҹ��� */
    else if (USIMM_SW_NOFILE_FOUND == pstPBMsg->ulResult)
    {
        gulPBFileCnt++;                                                 /* �ļ���������1��׼��������һ���ļ� */
        gstPBInitState.enPBInitState        = SI_PB_INIT_SEARCH_OK;
        gstPBInitState.enPBInitStep         = PB_INIT_PBC_SEARCH;       /* ��PB_INIT_PBC_SEARCH��Ȼת��3G��ͬ������ */
        gstPBInitState.enPBSearchState      = PB_SEARCH_ENABLE;
        gstPBSearchCtrlInfo.usReadOffset    = 0;
        gstPBSearchCtrlInfo.usReadNum       = 0;
    }
    else
    {
        /* ���ô���״̬ΪUSIMM�ظ�����,����ʧ�ܣ������������ȡ�ķ�ʽ�����PBC�ļ��ĳ�ʼ�� */
        gstPBInitState.enPBInitState    = SI_PB_INIT_SEARCH_ERR;
        gstPBInitState.enPBSearchState  = PB_SEARCH_DISABLE;

        PB_ERROR_LOG("SI_PB_InitPBCSearchMsgProc2:Search Result Error.");
    }

    return;
}


VOS_VOID SI_PB_InitIAPContentMsgProc2(PBMsgBlock *pstMsg)
{
    PS_USIM_GET_FILE_CNF_STRU    *pstPBMsg;
    VOS_UINT32                   ulOffset;
    VOS_UINT16                   usRecordNum;
    VOS_UINT8                    i;

    pstPBMsg = (PS_USIM_GET_FILE_CNF_STRU *)pstMsg;

    if (PS_USIM_GET_FILE_RSP != pstPBMsg->ulMsgName)
    {
        gstPBInitState.enPBInitState = SI_PB_INIT_MSGTYPE_ERR;

        LogPrint("\r\nSI_PB_InitIAPContentMsgProc2:Wrong Msg\r\n");

        return;
    }

    if ( VOS_NULL_PTR == gstIAPContent.pIAPContent )
    {
        /* ���ô���״̬Ϊ�����ڴ�ʧ�� */
        gstPBInitState.enPBInitState = SI_PB_INIT_PBMALLOC_FAIL;

        return;
    }

    if (VOS_OK == pstPBMsg->ulResult)
    {
        /* ÿ��IAP�ļ��ļ�¼�����ܲ�һ�£�����ƫ����ʱҪ�ۼ� */
        usRecordNum = pstPBMsg->ucRecordNum;/*gstPBCtrlInfo.astIAPInfo[gulPBFileCnt].ucRecordNum; */

        for (i = 0; i < gulPBFileCnt; i++)
        {
            usRecordNum += gstPBCtrlInfo.astIAPInfo[i].ucRecordNum;
        }

        ulOffset = (usRecordNum - 1) * gstPBCtrlInfo.astIAPInfo[gulPBFileCnt].ucRecordLen;

        VOS_MemCpy(gstIAPContent.pIAPContent + ulOffset, pstPBMsg->aucEf,
                            gstPBCtrlInfo.astIAPInfo[gulPBFileCnt].ucRecordLen);
    }
    else
    {
        /* �����ȡ��¼ʧ�ܣ��򲻴��������������¼�ĳ�ʼ�� */
    }

    if (++gstPBSearchCtrlInfo.usReadOffset >= gstPBSearchCtrlInfo.usReadNum)
    {
        /* ��ǰ��ʼ���ļ��ļ�¼��ȫ����ʼ����� */
        gstPBInitState.enPBInitState = SI_PB_INIT_RECORD_EXCEED;
    }
    else
    {
        /* ���ô���״̬Ϊ����δ��ʼ���ļ�¼ */
        gstPBInitState.enPBInitState = SI_PB_INIT_RECORD_REMAIN;
    }

    return;
}


VOS_VOID SI_PB_InitEmailContentMsgProc(PBMsgBlock *pstMsg)
{
    PS_USIM_GET_FILE_CNF_STRU    *pstPBMsg;
    VOS_UINT32                   ulOffset;
    VOS_UINT16                   usIndex;       /* ��¼ADN�绰����¼�Ŷ�Ӧ�Ļ��������� */
    VOS_UINT16                   usRecordNum;   /* ��ǰ��ȡ�ļ�¼�����м�¼�еı�ţ���1��ʼ */
    VOS_UINT8                    ucSuffix;
    VOS_UINT32                   i;

    ucSuffix = (VOS_UINT8)gulPBFileCnt;

    pstPBMsg = (PS_USIM_GET_FILE_CNF_STRU *)pstMsg;


    if (PS_USIM_GET_FILE_RSP != pstPBMsg->ulMsgName)
    {
        gstPBInitState.enPBInitState = SI_PB_INIT_MSGTYPE_ERR;

        LogPrint("\r\nSI_PB_InitEmailContentMsgProc:Wrong Msg\r\n");

        return;
    }

    if ( VOS_NULL_PTR == gstEMLContent.pContent )
    {
        /*���ô���״̬Ϊ�����ڴ�ʧ��*/
        gstPBInitState.enPBInitState = SI_PB_INIT_SPBMALLOC_FAIL;

        return;
    }

    if (VOS_OK == pstPBMsg->ulResult)
    {
        /* EMAIL�ļ���ÿ���¼�����ܲ�һ�£�����ƫ����ʱ��Ҫ��ͷ��ʼ�ۼ� */
        usRecordNum = pstPBMsg->ucRecordNum;

        for (i = 0; i < ucSuffix; i++)
        {
            usRecordNum += gstPBCtrlInfo.astEMLInfo[i].ucRecordNum;
        }

        usIndex     = usRecordNum -1;
        ulOffset    = usIndex * pstPBMsg->usEfLen;

        if (VOS_OK == SI_PB_EMLContentProc(pstPBMsg, ucSuffix, usRecordNum))
        {
            /* ����Email�绰���ļ�¼������ */
            VOS_MemCpy(gstEMLContent.pContent + ulOffset, pstPBMsg->aucEf, pstPBMsg->usEfLen);
        }
    }
    else
    {
        /* �����ȡ��¼ʧ�ܣ��򲻴��������������¼�ĳ�ʼ�� */
    }

    if (++gstPBSearchCtrlInfo.usReadOffset >= gstPBSearchCtrlInfo.usReadNum)
    {

        if((ucSuffix+1) == (VOS_UINT8)gstPBCtrlInfo.ulEMLFileNum)
        {
            gstEMLContent.enInitialState = PB_INITIALISED;
        }

        /* ��ǰ��ʼ���ļ��ļ�¼��ȫ����ʼ����� */
        gstPBInitState.enPBInitState = SI_PB_INIT_RECORD_EXCEED;
    }
    else
    {
        /* ���ô���״̬Ϊ����δ��ʼ���ļ�¼ */
        gstPBInitState.enPBInitState = SI_PB_INIT_RECORD_REMAIN;
    }

    return;
}


VOS_VOID SI_PB_InitANRContentMsgProc2(PBMsgBlock *pstMsg)
{
    PS_USIM_GET_FILE_CNF_STRU   *pstPBMsg;
    VOS_UINT32                  ulOffset;
    VOS_UINT32                  ulExtInfoNum;
    VOS_UINT32                  ulResult;
    VOS_UINT16                  usIndex;    /*��¼ADN�绰����¼�Ŷ�Ӧ�Ļ���������*/
    VOS_UINT16                  usExtRecIdOffset;
    VOS_UINT16                  usAnrRecordLen;
    VOS_UINT8                   ucXSuffix;
    VOS_UINT8                   ucYSuffix;
    VOS_UINT8                   *pucANRContent;
    /*VOS_UINT8                   ucRecordNum; */
    VOS_UINT8                   i;


    pstPBMsg = (PS_USIM_GET_FILE_CNF_STRU *)pstMsg;

    if (PS_USIM_GET_FILE_RSP != pstPBMsg->ulMsgName)
    {
        gstPBInitState.enPBInitState = SI_PB_INIT_MSGTYPE_ERR;

        LogPrint("\r\nSI_PB_InitANRContentMsgProc2:Wrong Msg\r\n");

        return;
    }

    /* ��ȡ��ǰ��ʼ�����ļ���ID */
    if (VOS_OK != SI_PB_GetANRSuffix(&ucXSuffix, &ucYSuffix, pstPBMsg->usEfId))
    {
        gstPBInitState.enPBInitState = SI_PB_INIT_PBTYPE_UNKOWN;

        PB_ERROR_LOG("SI_PB_InitANRContentMsgProc2:PB Tpye Error");

        return;
    }

    if ( VOS_NULL_PTR == gastANRContent[ucYSuffix].pContent )
    {
        /* ���ô���״̬Ϊ�����ڴ�ʧ�� */
        gstPBInitState.enPBInitState = SI_PB_INIT_SPBMALLOC_FAIL;

        return;
    }

    if(VOS_OK == pstPBMsg->ulResult)
    {
        /* ÿ��ADN�ļ���Ӧ��ANR��¼�����ܲ�һ�£���Ҫ��ͷ��ʼ�ۼ� */
        usIndex = pstPBMsg->ucRecordNum;

        for (i = 0; i < ucXSuffix; i++)
        {
            usIndex += gstPBCtrlInfo.astANRInfo[i][ucYSuffix].ucRecordNum;
        }

        ulOffset = (usIndex-1)*gstPBCtrlInfo.astANRInfo[ucXSuffix][ucYSuffix].ucRecordLen;

        pucANRContent   = gastANRContent[ucYSuffix].pContent + ulOffset;
        usAnrRecordLen  = gastANRContent[ucYSuffix].ucRecordLen;

        /* ����ANR�绰���ļ�¼������ */
        VOS_MemCpy(pucANRContent, pstPBMsg->aucEf, usAnrRecordLen);

        if (PB_FILE_TYPE2 == gstPBCtrlInfo.astANRInfo[ucXSuffix][ucYSuffix].enANRType)
        {
            usExtRecIdOffset = pstPBMsg->usEfLen - 3;
        }
        else
        {
            usExtRecIdOffset = pstPBMsg->usEfLen - 1;
        }

        ulExtInfoNum = gastPBContent[PB_ADN_CONTENT].ulExtInfoNum;

        /* ���˼�¼�Ƿ�Ϊ��Ч��¼���������Ч��¼��Ҫ��� */
        if (VOS_OK == SI_PB_CheckANRValidity(pucANRContent))
        {
            ulResult = SI_PB_InitANRType2ValidJudge(ucXSuffix,ucYSuffix,pstPBMsg->ucRecordNum,&usIndex);
        }
        else
        {
            ulResult = VOS_ERR;
        }

        /*�ü�¼��Ч*/
        if (VOS_OK == ulResult)
        {
            gastANRContent[ucYSuffix].usUsedNum++;

            /* ���֮ǰ�����ͺ����ֶ�Ϊ�գ�����ǰ������second number��Ϊ�գ���������ϼ�¼��Ϊ�ǿ� */
            if (SI_PB_CONTENT_INVALID == SI_PB_GetBitFromBuf(gastPBContent[PB_ADN_CONTENT].pIndex, usIndex))
            {
                gstPBCtrlInfo.ulTotalUsed++;

                SI_PB_SetBitToBuf(gastPBContent[PB_ADN_CONTENT].pIndex,usIndex,SI_PB_CONTENT_VALID);
            }
        }
        else
        {
            /* ����ü�¼��Ч�������ø�EXT��ϢΪ��Ч */
            pstPBMsg->aucEf[usExtRecIdOffset] = 0xFF;

            /* ͬʱ����ANR�ڴ�����Ϊ��Ч */
            /*pucANRContent[0] = 0xFF; */
            VOS_MemSet(pucANRContent, (VOS_CHAR)0xFF, usAnrRecordLen);
        }

        /* ����Ƿ���EXT�ļ�Ҫ��ȡ��XDN�ļ�����չ��¼�ļ�¼�Ų���Ϊ0��0xFF */
        if ((0xFF != pstPBMsg->aucEf[usExtRecIdOffset])
            && (0 != pstPBMsg->aucEf[usExtRecIdOffset])
            && (SI_PB_FILE_NOT_EXIST != gastEXTContent[ulExtInfoNum].usExtFlag))
        {
            gstPBInitState.enPBInitState = SI_PB_INIT_EXT_RECORD;
            gstPBInitState.enPBInitStep  = PB_INIT_EXTR_CONTENT;
            gulExtRecord                 = pstPBMsg->aucEf[usExtRecIdOffset];
            gulPBRecordCnt               = 0;   /* �����壬��ֹ��EXT��������ʱ���� */
            ++gstPBSearchCtrlInfo.usReadOffset; /* ���ҽ��ƫ������1����EXT��¼������ʱ���ؼ�1 */
            return;
        }
    }
    else
    {
        /* �����ȡ��¼ʧ�ܣ��򲻴��������������¼�ĳ�ʼ�� */
    }

    /* ��ǰ��Ҫ���ҵļ�¼��ȫ����ʼ����� */
    if ( ++gstPBSearchCtrlInfo.usReadOffset >= gstPBSearchCtrlInfo.usReadNum )
    {
        if (VOS_OK == SI_PB_GheckANRLast(ucXSuffix,ucYSuffix))
        {
            gastANRContent[ucYSuffix].enInitialState = PB_INITIALISED;

            gstPBCtrlInfo.ulANRStorageNum++;
        }

        /* ���ô���״̬Ϊ��ǰ�ļ������м�¼�ѳ�ʼ����� */
        gstPBInitState.enPBInitState = SI_PB_INIT_RECORD_EXCEED;
    }
    else
    {
        /* ���ô���״̬Ϊ����δ��ʼ���ļ�¼ */
        gstPBInitState.enPBInitState = SI_PB_INIT_RECORD_REMAIN;
    }

    return;
}
VOS_VOID SI_PB_InitXDNContentMsgProc2(PBMsgBlock *pstMsg)
{
    PS_USIM_GET_FILE_CNF_STRU    *pstPBMsg;
    VOS_UINT32                   ulPBType;
    VOS_UINT32                   ulOffset;
    VOS_UINT32                   ulExtInfoNum;
    VOS_UINT32                   ulNextPBType = 0;
    VOS_UINT16                   usIndex;       /*��¼ADN�绰����¼�Ŷ�Ӧ�Ļ���������*/
    VOS_UINT16                   usExtRecIdOffset;

    pstPBMsg = (PS_USIM_GET_FILE_CNF_STRU *)pstMsg;

    if (PS_USIM_GET_FILE_RSP != pstPBMsg->ulMsgName)
    {
        gstPBInitState.enPBInitState = SI_PB_INIT_MSGTYPE_ERR;

        LogPrint("\r\nSI_PB_InitXDNContentMsgProc2:Wrong Msg\r\n");

        return;
    }

    /* ��ȡ��ǰ��ʼ�����ļ���Ӧ�ĵ绰���ļ����� */
    if (VOS_OK != SI_PB_GetXDNPBType(&ulPBType, pstPBMsg->usEfId))
    {
        PB_ERROR_LOG("SI_PB_InitXDNContentMsgProc2:Unspecified PB Type");

        /*���ô���״̬Ϊ�绰������δ֪*/
        gstPBInitState.enPBInitState = SI_PB_INIT_PBTYPE_UNKOWN;
        gstPBInitState.enPBInitStep  = PB_INIT_XDN_CONTENT2;

        return;
    }

    if (VOS_NULL_PTR == gastPBContent[ulPBType].pContent)
    {
        PB_ERROR_LOG("SI_PB_InitXDNContentMsgProc2:Null Ptr");

        /* ���ô���״̬Ϊ�����ڴ�ʧ�� */
        gstPBInitState.enPBInitState = SI_PB_INIT_PBMALLOC_FAIL;

        return;
    }

    if (VOS_OK == pstPBMsg->ulResult)
    {
        /* ��ȡ�ظ��ļ�¼�����ڻ����е�λ�ã�ADN�������绰���ֱ��� */
        if ( PB_ADN_CONTENT == ulPBType )
        {
            /* ��ȡ��ǰ���ļ��ͼ�¼�Ŷ�Ӧ�Ļ��������� */
            if (VOS_OK != SI_PB_CountADNIndex(pstPBMsg->usEfId, pstPBMsg->ucRecordNum, &usIndex))
            {
                /* ���ô���״̬Ϊ��ǰ�ļ����м�¼������� */
                gstPBInitState.enPBInitState = SI_PB_INIT_RECORD_EXCEED;

                PB_ERROR_LOG("SI_PB_InitXDNContentMsgProc2:Get Record Index Failed");

                return;
            }
        }
        else
        {
            usIndex     = pstPBMsg->ucRecordNum;
        }

        ulOffset = (usIndex - 1) * gastPBContent[ulPBType].ucRecordLen;

        /* ����XDN�绰���ļ�¼������ */
        VOS_MemCpy(gastPBContent[ulPBType].pContent + ulOffset,
                   pstPBMsg->aucEf, gastPBContent[ulPBType].ucRecordLen);

        /* ���˼�¼�Ƿ�Ϊ��Ч��¼ */
        if (VOS_OK == SI_PB_CheckContentValidity(&gastPBContent[ulPBType], pstPBMsg->aucEf))
        {
            gastPBContent[ulPBType].usUsedNum++;

            if (SI_PB_CONTENT_INVALID ==
                        SI_PB_GetBitFromBuf(gastPBContent[ulPBType].pIndex, usIndex))
            {
                /* ���֮ǰ�����ͺ����ֶ�Ϊ�գ�����ǰ������ADN��Ϊ�գ���������ϼ�¼��Ϊ�ǿ� */
                if(PB_ADN_CONTENT == ulPBType)
                {
                    gstPBCtrlInfo.ulTotalUsed++;
                }

                SI_PB_SetBitToBuf(gastPBContent[ulPBType].pIndex,usIndex,SI_PB_CONTENT_VALID);
            }
        }

        usExtRecIdOffset = ((EFBDN == pstPBMsg->usEfId)?\
                            (pstPBMsg->usEfLen - 2):(pstPBMsg->usEfLen - 1));

        ulExtInfoNum = gastPBContent[ulPBType].ulExtInfoNum;

        /* ����Ƿ���EXT�ļ�Ҫ��ȡ��XDN�ļ�����չ��¼�ļ�¼�Ų���Ϊ0��0xFF */
        if( ( 0xFF != pstPBMsg->aucEf[usExtRecIdOffset] )
            && (0 != pstPBMsg->aucEf[usExtRecIdOffset])
            && (SI_PB_FILE_NOT_EXIST != gastEXTContent[ulExtInfoNum].usExtFlag) )
        {
            gstPBInitState.enPBInitState = SI_PB_INIT_EXT_RECORD;
            gstPBInitState.enPBInitStep  = PB_INIT_EXT_CONTENT;
            gulExtRecord                 = pstPBMsg->aucEf[usExtRecIdOffset];
            gulPBRecordCnt               = 0;   /* �����壬��ֹ��EXT��������ʱ���� */
            ++gstPBSearchCtrlInfo.usReadOffset; /* ���ҽ��ƫ������1����EXT��¼������ʱ���ؼ�1 */
            return;
        }
    }
    else
    {
        /* �����ȡ��¼ʧ�ܣ��򲻴��������������¼�ĳ�ʼ�� */
    }

    /* ��ǰ��Ҫ���ҵļ�¼��ȫ����ʼ����� */
    if ( ++gstPBSearchCtrlInfo.usReadOffset >= gstPBSearchCtrlInfo.usReadNum )
    {
        /* ��ȡ��һ����Ҫ�������ݵĵ绰�� */
        SI_PB_GetXDNPBType(&ulNextPBType, gstPBInitState.ausFileId[gulPBFileCnt+1]);

        /********** ��������һ��ADN�ļ������ADN�绰��<һ���ļ���Ӧһ���绰�� *************/
        if (PB_ADN_CONTENT != ulPBType)
        {
            /* ���õ�ǰ�绰����ʼ��״̬Ϊ�ѳ�ʼ�� */
            gastPBContent[ulPBType].enInitialState = PB_INITIALISED;
        }
        else if(PB_ADN_CONTENT != ulNextPBType)
        {
            /* ���õ�ǰ�绰����ʼ��״̬Ϊ�ѳ�ʼ�� */
            gastPBContent[ulPBType].enInitialState = PB_INITIALISED;
        }
        else
        {
            ;
        }

        /* ���ô���״̬Ϊ��ǰ�ļ������м�¼�ѳ�ʼ����� */
        gstPBInitState.enPBInitState = SI_PB_INIT_RECORD_EXCEED;
    }
    else
    {
        /* ���ô���״̬Ϊ����δ��ʼ���ļ�¼ */
        gstPBInitState.enPBInitState = SI_PB_INIT_RECORD_REMAIN;
    }

    return;
}
VOS_VOID SI_PB_InitPBCContentMsgProc2(PBMsgBlock *pstMsg)
{
    PS_USIM_GET_FILE_CNF_STRU    *pstPBMsg;
    VOS_UINT8                    aucEF[2];

    pstPBMsg = (PS_USIM_GET_FILE_CNF_STRU *)pstMsg;

    if (PS_USIM_GET_FILE_RSP != pstPBMsg->ulMsgName)
    {
        gstPBInitState.enPBInitState = SI_PB_INIT_MSGTYPE_ERR;

        LogPrint("\r\nSI_PB_InitPBCContentMsgProc2:Wrong Msg\r\n");

        return;
    }

    if (VOS_OK != pstPBMsg->ulResult)
    {
        VOS_MemSet(aucEF, 0x00, sizeof(aucEF));
    }
    else
    {
        VOS_MemCpy(aucEF, pstPBMsg->aucEf, sizeof(aucEF));
    }

    /* coverity[uninit_use_in_call] */
    SI_PB_DecodeEFPBCRecord(aucEF, pstPBMsg->ucRecordNum);

    /* ��ǰ��Ҫ���ҵļ�¼��ȫ����ʼ����� */
    if (++gstPBSearchCtrlInfo.usReadOffset >= gstPBSearchCtrlInfo.usReadNum)
    {
        /* ���м�¼����ȡ��� */
        gstPBInitState.enPBInitState = SI_PB_INIT_RECORD_EXCEED;
    }
    else
    {
        gstPBInitState.enPBInitState = SI_PB_INIT_RECORD_REMAIN;
    }

    return;
}
VOS_VOID SI_PB_InitPBCContentMsgProc(PBMsgBlock *pstMsg)
{
    PS_USIM_GET_FILE_CNF_STRU    *pstPBMsg;
    VOS_UINT8                    aucEF[2];

    pstPBMsg = (PS_USIM_GET_FILE_CNF_STRU *)pstMsg;

    if (PS_USIM_GET_FILE_RSP != pstPBMsg->ulMsgName)
    {
        gstPBInitState.enPBInitState = SI_PB_INIT_MSGTYPE_ERR;

        LogPrint("\r\nSI_PB_InitPBCContentMsgProc:Wrong Msg\r\n");

        return;
    }

    if (VOS_OK != pstPBMsg->ulResult)
    {
        VOS_MemSet(aucEF, 0x00, sizeof(aucEF));
    }
    else
    {
        VOS_MemCpy(aucEF, pstPBMsg->aucEf, sizeof(aucEF));
    }

    /* coverity[uninit_use_in_call] */
    SI_PB_DecodeEFPBCRecord(aucEF, pstPBMsg->ucRecordNum);

    /* ��ǰ��Ҫ��ȡ�ļ�¼��ȫ����ʼ����� */
    if (++gulPBRecordCnt > gstPBCtrlInfo.astADNInfo[0].ucRecordNum)
    {
        /* ���м�¼����ȡ��� */
        gstPBInitState.enPBInitState = SI_PB_INIT_RECORD_EXCEED;
    }
    else
    {
        gstPBInitState.enPBInitState = SI_PB_INIT_RECORD_REMAIN;
    }

    return;
}
VOS_UINT32 SI_PB_InitANRType2ValidJudge(VOS_UINT8 ucXSuffix, VOS_UINT8 ucYSuffix,
                                                    VOS_UINT8 ucRecordNum,VOS_UINT16 *pusRealIndex)
{
    VOS_UINT32 i;

    if(PB_FILE_TYPE2 == gstPBCtrlInfo.astANRInfo[ucXSuffix][ucYSuffix].enANRType)
    {
        for(i = 0; i < gstPBCtrlInfo.astIAPInfo[ucXSuffix].ucRecordNum; i++)
        {
            /*IAP�����ҵ��ü�¼��Ӧ��ϵ*/
            if(gstIAPContent.pIAPContent[(ucXSuffix*(gstPBCtrlInfo.astIAPInfo[0].ucRecordLen*gstPBCtrlInfo.astIAPInfo[0].ucRecordNum))
                    +(i*gstPBCtrlInfo.astIAPInfo[0].ucRecordLen)+(gstPBCtrlInfo.astANRInfo[ucXSuffix][ucYSuffix].ulANRTagNum-1)] == ucRecordNum)
            {
                *pusRealIndex = (VOS_UINT16)((ucXSuffix*gstPBCtrlInfo.astIAPInfo[0].ucRecordNum) + (i+1));

                return VOS_OK;
            }
        }

        return VOS_ERR;
    }
    else
    {
        return VOS_OK;
    }
}
VOS_VOID SI_PB_InitANRContentMsgProc(PBMsgBlock *pstMsg)
{
    PS_USIM_GET_FILE_CNF_STRU    *pstPBMsg;
    VOS_UINT32                   ulOffset;
    VOS_UINT32                   ulExtInfoNum;
    VOS_UINT32                   ulResult = VOS_OK;
    VOS_UINT16                   usExtRecIdOffset;
    VOS_UINT16                   usAnrRecordLen;
    VOS_UINT16                   usIndex;/*��¼ADN�绰����¼�Ŷ�Ӧ�Ļ���������*/
    VOS_UINT8                    ucRecordNum;
    VOS_UINT8                    ucXSuffix;
    VOS_UINT8                    ucYSuffix;
    VOS_UINT8                    *pucANRContent;
    VOS_UINT8                    i;

    pstPBMsg = (PS_USIM_GET_FILE_CNF_STRU *)pstMsg;

    if (PS_USIM_GET_FILE_RSP != pstPBMsg->ulMsgName)
    {
        gstPBInitState.enPBInitState = SI_PB_INIT_MSGTYPE_ERR;

        LogPrint("\r\nSI_PB_InitANRContentMsgProc:Wrong Msg\r\n");

        return;
    }

    /*��ȡ��ǰ��ʼ�����ļ���Ӧ�ĵ绰��*/
    if(VOS_OK != SI_PB_GetANRSuffix(&ucXSuffix,&ucYSuffix,pstPBMsg->usEfId))
    {
        gstPBInitState.enPBInitState = SI_PB_INIT_PBTYPE_UNKOWN;

        PB_ERROR_LOG("SI_PB_InitANRContentMsgProc:PB Tpye Error");

        return;
    }

    /*����������ʼ��״̬����*/
    if(VOS_OK != pstPBMsg->ulResult)
    {
        /*���ô���״̬ΪUSIMM�ظ�����*/
        gstPBInitState.enPBInitState = SI_PB_INIT_USIMSPBCNF_ERR;

        PB_ERROR_LOG("SI_PB_InitANRContentMsgProc:Intial Step Error");

        return;
    }

    if ( VOS_NULL_PTR == gastANRContent[ucYSuffix].pContent )
    {
        /*���ô���״̬Ϊ�����ڴ�ʧ��*/
        gstPBInitState.enPBInitState = SI_PB_INIT_SPBMALLOC_FAIL;

        return;
    }

    /* ÿ��ADN�ļ���Ӧ��ANR��¼�����ܲ�һ�£���Ҫ��ͷ��ʼ�ۼ� */
    usIndex = pstPBMsg->ucRecordNum;

    for (i = 0; i < ucXSuffix; i++)
    {
        usIndex += gstPBCtrlInfo.astANRInfo[i][ucYSuffix].ucRecordNum;
    }

    ulOffset = (usIndex-1)*gstPBCtrlInfo.astANRInfo[ucXSuffix][ucYSuffix].ucRecordLen;

    pucANRContent = gastANRContent[ucYSuffix].pContent + ulOffset;
    usAnrRecordLen  = gastANRContent[ucYSuffix].ucRecordLen;

    /*����ANR�绰���ļ�¼������*/
    VOS_MemCpy(pucANRContent, pstPBMsg->aucEf, usAnrRecordLen);

    if(PB_FILE_TYPE2 == gstPBCtrlInfo.astANRInfo[ucXSuffix][ucYSuffix].enANRType)
    {
        usExtRecIdOffset = pstPBMsg->usEfLen - 3;
    }
    else
    {
        usExtRecIdOffset = pstPBMsg->usEfLen - 1;
    }

    ulExtInfoNum = gastPBContent[PB_ADN_CONTENT].ulExtInfoNum;

    /*���˼�¼�Ƿ�Ϊ��Ч��¼*/
    if(VOS_OK == SI_PB_CheckANRValidity(pucANRContent))
    {
        ulResult = SI_PB_InitANRType2ValidJudge(ucXSuffix,ucYSuffix,pstPBMsg->ucRecordNum,&usIndex);
    }
    else
    {
        ulResult = VOS_ERR;
    }

    /*�ü�¼��Ч*/
    if(VOS_OK == ulResult)
    {
        gastANRContent[ucYSuffix].usUsedNum++;

        /*���֮ǰ�����ͺ����ֶ�Ϊ�գ�����ǰ������second number��Ϊ�գ���������ϼ�¼��Ϊ�ǿ�*/
        if (SI_PB_CONTENT_INVALID == SI_PB_GetBitFromBuf(gastPBContent[PB_ADN_CONTENT].pIndex, usIndex))
        {
            gstPBCtrlInfo.ulTotalUsed++;

            SI_PB_SetBitToBuf(gastPBContent[PB_ADN_CONTENT].pIndex,usIndex,SI_PB_CONTENT_VALID);
        }
    }
    else
    {
        /*����ü�¼��Ч�������ø�EXT��ϢΪ��Ч*/
        pstPBMsg->aucEf[usExtRecIdOffset] = 0xFF;

        /*ͬʱ����ANR�ڴ�����Ϊ��Ч*/
        pucANRContent[0] = 0xFF;
    }

    /* ����Ƿ���EXT�ļ�Ҫ��ȡ��XDN�ļ�����չ��¼�ļ�¼�Ų���Ϊ0��0xFF */
    if( ( 0xFF != pstPBMsg->aucEf[usExtRecIdOffset] )
        && (0 != pstPBMsg->aucEf[usExtRecIdOffset])
        && (SI_PB_FILE_NOT_EXIST != gastEXTContent[ulExtInfoNum].usExtFlag) )
    {
        gstPBInitState.enPBInitState = SI_PB_INIT_EXT_RECORD;
        gstPBInitState.enPBInitStep  = PB_INIT_EXTR_CONTENT;
        gulExtRecord                 = pstPBMsg->aucEf[usExtRecIdOffset];
        return;
    }

    ucRecordNum = gstPBCtrlInfo.astANRInfo[ucXSuffix][ucYSuffix].ucRecordNum;

    if ( ++gulPBRecordCnt > ucRecordNum )/*��ǰ��ʼ���ļ��ļ�¼��ȫ����ʼ�����*/
    {
        if(VOS_OK == SI_PB_GheckANRLast(ucXSuffix,ucYSuffix))
        {
            gastANRContent[ucYSuffix].enInitialState = PB_INITIALISED;

            gstPBCtrlInfo.ulANRStorageNum++;
        }

        /*���ô���״̬Ϊ��ǰ�ļ������м�¼�ѳ�ʼ�����*/
        gstPBInitState.enPBInitState = SI_PB_INIT_RECORD_EXCEED;
    }
    else
    {
        /*���ô���״̬Ϊ����δ��ʼ���ļ�¼*/
        gstPBInitState.enPBInitState = SI_PB_INIT_RECORD_REMAIN;
    }

    return;
}


VOS_VOID SI_PB_InitEXTRContentMsgProc(PBMsgBlock *pstMsg)
{
    PS_USIM_GET_FILE_CNF_STRU    *pstPBMsg;
    VOS_UINT32                   ulOffset;
    VOS_UINT32                   ulResult1;
    VOS_UINT32                   ulResult2;
    VOS_UINT8                    ucRecordNum;
    VOS_UINT16                   usANRFileId;
    VOS_UINT32                   ulExtInfoNum;
    VOS_UINT8                    ucXSuffix;
    VOS_UINT8                    ucYSuffix;

    pstPBMsg = (PS_USIM_GET_FILE_CNF_STRU *)pstMsg;

    if (PS_USIM_GET_FILE_RSP != pstPBMsg->ulMsgName)
    {
        gstPBInitState.enPBInitState = SI_PB_INIT_MSGTYPE_ERR;

        LogPrint("\r\nSI_PB_InitEXTRContentMsgProc:Wrong Msg\r\n");

        return;
    }

    if (VOS_OK != SI_PB_GetANRFid((gulPBFileCnt+1),&usANRFileId))
    {
        /*���ô���״̬Ϊ�绰������δ֪*/
        gstPBInitState.enPBInitState = SI_PB_INIT_PBTYPE_UNKOWN;

        /*AT2D15844*/
        gstPBInitState.enPBInitStep = PB_INIT_ANR_CONTENT;

        PB_ERROR_LOG("SI_PB_InitEXTRContentMsgProc:Unspecified PB Type");

        return;
    }

    if(VOS_OK != SI_PB_GetANRSuffix(&ucXSuffix, &ucYSuffix, usANRFileId))
    {
        /*���ô���״̬Ϊ�绰������δ֪*/
        gstPBInitState.enPBInitState = SI_PB_INIT_PBTYPE_UNKOWN;

        /*AT2D15844*/
        gstPBInitState.enPBInitStep = PB_INIT_ANR_CONTENT;

        PB_ERROR_LOG("SI_PB_InitEXTRContentMsgProc:Suffix Error");

        return;
    }

    ucRecordNum = gstPBCtrlInfo.astANRInfo[ucXSuffix][ucYSuffix].ucRecordNum;

    ulResult1 = (gstPBSearchCtrlInfo.usReadOffset >= gstPBSearchCtrlInfo.usReadNum);    /* ��ǰSEARCH��¼��ȫ����ʼ����� */
    ulResult2 = (++gulPBRecordCnt > ucRecordNum);                                       /* ��ǰ��ʼ���ļ��ļ�¼��ȫ����ʼ����� */

    /* �����ǰ��ȡ��¼�Ѿ�������ǰ�ļ���¼����Ҫ�л�����һ���ļ� */
    if (ulResult1 || ulResult2)
    {
        if(VOS_OK == SI_PB_GheckANRLast(ucXSuffix,ucYSuffix))
        {
            gastANRContent[ucYSuffix].enInitialState = PB_INITIALISED;
            gstPBCtrlInfo.ulANRStorageNum++;
        }

        /*���ô���״̬Ϊ��ǰ�ļ������м�¼�ѳ�ʼ�����*/
        gstPBInitState.enPBInitState = SI_PB_INIT_RECORD_EXCEED;
    }
    else
    {
        /*���ô���״̬Ϊ����δ��ʼ���ļ�¼*/
        gstPBInitState.enPBInitState = SI_PB_INIT_RECORD_REMAIN;
    }

    /* ��һ����ʼ���ļ�ΪANR�ļ������ݲ���״̬���ж�
    ��ʹ��������ȡ�ķ�ʽ�����ò��ҵķ�ʽ����ʼ�� */
    if (PB_SEARCH_ENABLE  == gstPBInitState.enPBSearchState)
    {
        gstPBInitState.enPBInitStep = PB_INIT_ANR_CONTENT2;
    }
    else
    {
        gstPBInitState.enPBInitStep = PB_INIT_ANR_CONTENT;
    }

    if ( VOS_OK != pstPBMsg->ulResult )
    {
        /* ����ڳ�ʼ��EXT�ļ��Ĺ����г��������ȡʧ�ܣ�������������ANR�ĳ�ʼ�� */
        PB_ERROR_LOG("SI_PB_InitEXTRContentMsgProc:Intial Msg Return Error");
        return;
    }
    else
    {
        ulOffset = (gulExtRecord - 1)*SI_PB_EXT_LEN;

        /*����EXT���ݵ����棬�����¼Ϊ��Ч��¼����ʹ�ü�¼�ż�1*/
        ulExtInfoNum = gastPBContent[PB_ADN_CONTENT].ulExtInfoNum;

        /* coverity[overrun-buffer-arg] */
        VOS_MemCpy(gastEXTContent[ulExtInfoNum].pExtContent + ulOffset, pstPBMsg->aucEf, SI_PB_EXT_LEN);

        if( 0 != *(pstPBMsg->aucEf+1))
        {
            gastEXTContent[ulExtInfoNum].usExtUsedNum++;
        }

        return;
    }
}
VOS_VOID SI_PB_InitEMLContentMsgProc(PBMsgBlock *pstMsg)
{
    PS_USIM_GET_FILE_CNF_STRU    *pstPBMsg;
    VOS_UINT32                   ulOffset;
    VOS_UINT16                   usIndex;/*��¼ADN�绰����¼�Ŷ�Ӧ�Ļ���������*/
    VOS_UINT8                    ucRecordNum;
    VOS_UINT8                    ucSuffix;
    VOS_UINT8                    i;

    ucSuffix = (VOS_UINT8)gulPBFileCnt;

    pstPBMsg = (PS_USIM_GET_FILE_CNF_STRU *)pstMsg;

    if (PS_USIM_GET_FILE_RSP != pstPBMsg->ulMsgName)
    {
        gstPBInitState.enPBInitState = SI_PB_INIT_MSGTYPE_ERR;

        LogPrint("\r\nSI_PB_InitEMLContentMsgProc:Wrong Msg\r\n");

        return;
    }

    /*����������ʼ��״̬����*/
    if(VOS_OK != pstPBMsg->ulResult)
    {
        /*���ô���״̬ΪUSIMM�ظ�����*/
        gstPBInitState.enPBInitState = SI_PB_INIT_USIMSPBCNF_ERR;

        PB_ERROR_LOG("SI_PB_InitEMLContentMsgProc:Intial Step Error");

        return;
    }

    if ( VOS_NULL_PTR == gstEMLContent.pContent )
    {
        /*���ô���״̬Ϊ�����ڴ�ʧ��*/
        gstPBInitState.enPBInitState = SI_PB_INIT_SPBMALLOC_FAIL;

        PB_ERROR_LOG("SI_PB_InitEMLContentMsgProc:The Memory is Null");

        return;
    }

    usIndex     = pstPBMsg->ucRecordNum;

    for (i = 0; i < ucSuffix; i++)
    {
        usIndex += gstPBCtrlInfo.astEMLInfo[i].ucRecordNum;
    }

    ucRecordNum = gstPBCtrlInfo.astEMLInfo[ucSuffix].ucRecordNum;
    ulOffset    = (usIndex - 1) * gstPBCtrlInfo.astEMLInfo[ucSuffix].ucRecordLen;

    if (VOS_OK == SI_PB_EMLContentProc(pstPBMsg, ucSuffix, usIndex))
    {
        /*����Email�绰���ļ�¼������*/
        VOS_MemCpy(gstEMLContent.pContent + ulOffset, pstPBMsg->aucEf, pstPBMsg->usEfLen);
    }

    if ( ++gulPBRecordCnt > ucRecordNum )/*��ǰ��ʼ���ļ��ļ�¼��ȫ����ʼ�����*/
    {
        if((ucSuffix+1) == (VOS_UINT8)gstPBCtrlInfo.ulEMLFileNum)
        {
            gstEMLContent.enInitialState = PB_INITIALISED;
        }

        /*���ô���״̬Ϊ��ǰ�ļ������м�¼�ѳ�ʼ�����*/
        gstPBInitState.enPBInitState = SI_PB_INIT_RECORD_EXCEED;
    }
    else
    {
        /*���ô���״̬Ϊ����δ��ʼ���ļ�¼*/
        gstPBInitState.enPBInitState = SI_PB_INIT_RECORD_REMAIN;
    }

    return;
}


VOS_VOID SI_PB_InitSYNCHProc(PBMsgBlock *pstMsg)
{
    if(PS_USIM_GET_FILE_RSP == pstMsg->ulMsgName)
    {
        /*����3G��ͬ��ʱ�Ķ��ļ�����ظ�*/
        SI_PB_InitSYNCHGetProc(pstMsg);

        return;
    }

    if(PS_USIM_SET_FILE_RSP == pstMsg->ulMsgName)
    {
        /*����3G��ͬ��ʱ��д�ļ�����ظ�*/
        SI_PB_InitSYNCHSetProc(pstMsg);
    }

    return;
}


VOS_UINT16  SI_PB_TransferFileCnt2ExtFileID(VOS_UINT32 PBFileCnt,
                                                    VOS_UINT32 *pulPBType)
{
    VOS_UINT8       ucCardStatus;
    VOS_UINT8       ucCardType;
    VOS_UINT32      ulPBType;

    USIMM_GetCardType(&ucCardStatus, &ucCardType);

    SI_PB_GetXDNPBType(&ulPBType, gstPBInitState.ausFileId[PBFileCnt]);

    /* coverity[uninit_use] */
    *pulPBType = ulPBType;

    /*���ΪSIM������д��Ҫ��ʼ�����ļ��ͳ�ʼ����Ϣ*/
    if ( USIMM_CARD_SIM == ucCardType )
    {
        if(PB_ADN_CONTENT == ulPBType)/*ADN*/
        {
            return gastEXTContent[PB_ADN_CONTENT].usExtFileId;
        }
        else if(PB_FDN_CONTENT == ulPBType)/*FDN*/
        {
            return gastEXTContent[PB_FDN_CONTENT].usExtFileId;
        }
        else if(PB_BDN_CONTENT == ulPBType)/*BDN*/
        {
            return gastEXTContent[PB_BDN_CONTENT].usExtFileId;
        }
        else/*MSISDN*/
        {
            return gastEXTContent[PB_ADN_CONTENT].usExtFileId;
        }
    }
    else
    {
        if(PB_ADN_CONTENT ==ulPBType)/*ADN*/
        {
            return gastEXTContent[PB_ADN_CONTENT].usExtFileId;
        }
        else if(PB_FDN_CONTENT == ulPBType )/*FDN*/
        {
            return gastEXTContent[PB_FDN_CONTENT].usExtFileId;
        }
        else if(PB_BDN_CONTENT == ulPBType)/*BDN*/
        {
            return gastEXTContent[PB_BDN_CONTENT].usExtFileId;
        }
        else/*MSISDN*/
        {
            return gastEXTContent[PB_MSISDN_CONTENT].usExtFileId;
        }
    }
}


VOS_VOID SI_PB_SearchResultProc(VOS_UINT8 *pucReadString, VOS_UINT8 *pucSearchResult,
                                            VOS_UINT8 ucMatchNum, VOS_UINT8 ucTotalNum)
{
    VOS_UINT32  ulOffset;
    VOS_UINT8   ucBit;
    VOS_UINT8   i;
    VOS_UINT8   usBitNo;
    VOS_UINT8   aucBitMap[32];

    VOS_MemSet(pucReadString, (VOS_CHAR)0xFF, 256);

    /* ���һ��ƥ��ļ�¼��û�У�����Ҫ��ͷ����β */
    if (0 == ucMatchNum)
    {
        for (i = 0; i < 254; i++)
        {
            *pucReadString++ = (i+1);
        }
        return;
    }

    VOS_MemSet(aucBitMap, (VOS_CHAR)0xFF, sizeof(aucBitMap));

    /* ��ƥ������Ӧ��λ������Ϊ�� */
    for (i = 0; i < ucMatchNum; i++)
    {
        usBitNo     = pucSearchResult[i];
        ulOffset    = (usBitNo - 1) / 8;
        ucBit       = (VOS_UINT8)((usBitNo - 1) % 8);

        aucBitMap[ulOffset] &= (~(VOS_UINT8)(0x1 << ucBit));
    }

    /* �Ѳ�Ϊ���λ��ȡ������Ϊ��һ����ȡ�ļ�¼�� */
    for(i = 0; i < ucTotalNum; i++)
    {
        usBitNo     = i;
        ulOffset    = usBitNo / 8;
        ucBit       = (VOS_UINT8)(usBitNo % 8);

        if ((aucBitMap[ulOffset] >> ucBit) & 0x1)
        {
            *pucReadString++ = (i + 1);
        }
    }
}
VOS_UINT32 SI_PB_EMLContentProc(PS_USIM_GET_FILE_CNF_STRU *pstPBMsg, VOS_UINT8 ucSuffix,
                                            VOS_UINT16 usEmailIndex)
{
    VOS_UINT32          ulResult = VOS_ERR;
    VOS_UINT32          ulIAPOffset;
    VOS_UINT32          ulEmlTag;
    VOS_UINT32          i;
    VOS_UINT16          usIAPRecordNum;
    VOS_UINT16          usIndex;

    /* ���EMAIL�ļ���TYPE1���͵��ļ���ֱ��ʹ��usEmailIndex������Ҫ��IAP�ļ���ȥ���� */
    usIndex     = usEmailIndex;

    if (0xFF != pstPBMsg->aucEf[0])
    {
        if (PB_FILE_TYPE2 == gstPBCtrlInfo.astEMLInfo[ucSuffix].enEMLType)
        {
            /* ÿ��IAP�ļ��ļ�¼�����ܲ�һ�£���Ҫ��ͷ��ʼ�ۼ� */
            usIAPRecordNum = 0;

            for (i = 0; i < ucSuffix; i++)
            {
                usIAPRecordNum += gstPBCtrlInfo.astIAPInfo[i].ucRecordNum;
            }

            ulEmlTag = gstPBCtrlInfo.astEMLInfo[ucSuffix].ulEMLTagNum;

            /* ���û��IAP��Ӧ���ü�¼������Ϊ�ü�¼��Ч */
            for (i = 0; i < gstPBCtrlInfo.astIAPInfo[ucSuffix].ucRecordNum; i++)
            {
                ulIAPOffset = (usIAPRecordNum + i) * gstIAPContent.ucRecordLen;

                if (gstIAPContent.pIAPContent[ulIAPOffset +(ulEmlTag - 1)] == pstPBMsg->ucRecordNum)
                {
                    PB_INFO_LOG("Check IAP & EMAIL OK, Valid Record.");

                    gstEMLContent.usUsedNum++;

                    /* ��ȡ����Email��¼��Ӧ�������� */
                    usIndex = (VOS_UINT16)(usIAPRecordNum + i + 1);

                    ulResult = VOS_OK;

                    break;
                }
            }
        }
        else
        {
            ulResult    = VOS_OK;
            gstEMLContent.usUsedNum++;
        }

        if (VOS_OK == ulResult)
        {
            if (SI_PB_CONTENT_INVALID == SI_PB_GetBitFromBuf(gastPBContent[PB_ADN_CONTENT].pIndex, usIndex))
            {
                gstPBCtrlInfo.ulTotalUsed++;

                SI_PB_SetBitToBuf(gastPBContent[PB_ADN_CONTENT].pIndex, usIndex, SI_PB_CONTENT_VALID);
            }
        }

        return ulResult;
    }
    else
    {
        return ulResult;
    }
}


VOS_VOID SI_PB_RefreshUIDFile_Stub(VOS_UINT8 ucAdnIndex)
{
    VOS_UINT8                   aucIndex[2];
    VOS_UINT8                   i;
    VOS_UINT32                  ulResult;
    VOS_UINT16                  usIndex = 1;
    USIMM_SET_FILE_INFO_STRU    stUpdateReq;

    if(0xFFFF != gstPBCtrlInfo.usPUIDValue)
    {
        return;
    }

    aucIndex[0] = 0;

    aucIndex[1] = 1;

    for(i=1; i<=10; i++)
    {
        ulResult = SI_PB_GetBitFromBuf(gastPBContent[PB_ADN_CONTENT].pIndex, usIndex);

        if((ulResult == SI_PB_CONTENT_VALID)&&(i != ucAdnIndex))
        {
            stUpdateReq.enAppType       = USIMM_PB_APP;
            stUpdateReq.pucEfContent    = aucIndex;
            stUpdateReq.ucRecordNum     = i;
            stUpdateReq.ulEfLen         = sizeof(aucIndex);
            stUpdateReq.usEfId          = gstPBCtrlInfo.astUIDInfo[0].usFileID;

            ulResult = USIMM_SetFileReq(MAPS_PB_PID, 0, &stUpdateReq);

            if(VOS_OK != ulResult)
            {
                PB_ERROR_LOG("SI_PB_RefreshUIDFile_Stub: Update the UID File Failed");

                return;
            }

            aucIndex[1]++;
        }

        usIndex++;
    }

    gstPBCtrlInfo.usPUIDValue = gstPBCtrlInfo.usUIDMaxValue;

    aucIndex[1] = (VOS_UINT8)(gstPBCtrlInfo.usUIDMaxValue&0xFF);

    stUpdateReq.enAppType       = USIMM_PB_APP;
    stUpdateReq.pucEfContent    = aucIndex;
    stUpdateReq.ucRecordNum     = 2;
    stUpdateReq.ulEfLen         = sizeof(aucIndex);
    stUpdateReq.usEfId          = EFPUID;

    ulResult = USIMM_SetFileReq(MAPS_PB_PID, 0, &stUpdateReq);

    if(VOS_OK != ulResult)
    {
        PB_ERROR_LOG("SI_PB_RefreshUIDFile_Stub: Update the PUID File Failed");

        return;
    }

    stUpdateReq.enAppType       = USIMM_PB_APP;
    stUpdateReq.pucEfContent    = aucIndex;
    stUpdateReq.ucRecordNum     = ucAdnIndex;
    stUpdateReq.ulEfLen         = sizeof(aucIndex);
    stUpdateReq.usEfId          = gstPBCtrlInfo.astUIDInfo[0].usFileID;

    ulResult = USIMM_SetFileReq(MAPS_PB_PID, 0, &stUpdateReq);

    if(VOS_OK != ulResult)
    {
        PB_ERROR_LOG("SI_PB_RefreshUIDFile_Stub: Update the UID File Last Record Failed");

        return;
    }

    return;
}


VOS_VOID SI_PB_IncreaceCCValue(VOS_UINT16 usIncValue,VOS_UINT8 ucAdnIndex)
{
    VOS_UINT16                  usCCTemp;
    VOS_UINT8                   aucContent[4];
    VOS_UINT8                   ucCardType;
    USIMM_SET_FILE_INFO_STRU    stUpdateReq;

    USIMM_GetCardType(VOS_NULL_PTR, &ucCardType);

    /*USIM������3G��ͬ��*/
    if(USIMM_CARD_USIM != ucCardType)
    {
        return;
    }

    usCCTemp  = usIncValue + gstPBCtrlInfo.usCCValue;

    if(usCCTemp < gstPBCtrlInfo.usCCValue)  /*���ڷ����������������Ӻ��ֵС��ԭ��ֵ*/
    {
        gstPBCtrlInfo.usCCValue = 1;/*����CC�ļ��������¼��㣬ÿ�δ�1������0��ʼ������������Ҫ������1*/

        usCCTemp = 1;

        /*����PSC������*/
        gstPBCtrlInfo.ulPSCValue++;
        gstPBCtrlInfo.ulPSCValue = ((0 == gstPBCtrlInfo.ulPSCValue) ? 1 : (gstPBCtrlInfo.ulPSCValue));

        aucContent[0] = (VOS_UINT8)((gstPBCtrlInfo.ulPSCValue>>24)&0x000000FF);
        aucContent[1] = (VOS_UINT8)((gstPBCtrlInfo.ulPSCValue>>16)&0x000000FF);
        aucContent[2] = (VOS_UINT8)((gstPBCtrlInfo.ulPSCValue>>8)&0x000000FF);
        aucContent[3] = (VOS_UINT8)(gstPBCtrlInfo.ulPSCValue&0x000000FF);

        stUpdateReq.enAppType       = USIMM_PB_APP;
        stUpdateReq.pucEfContent    = aucContent;
        stUpdateReq.ucRecordNum     = 0;
        stUpdateReq.ulEfLen         = sizeof(aucContent);
        stUpdateReq.usEfId          = EFPSC;

        if(VOS_OK != USIMM_SetFileReq(MAPS_PB_PID, 0, &stUpdateReq))
        {
            PB_ERROR_LOG("SI_PB_IncreaceCCValue: Update the EFPSC File is Error");
            return;
        }

        SI_PB_RefreshUIDFile_Stub(ucAdnIndex);    /*ˢ��UID������*/
    }

    aucContent[0] = (VOS_UINT8)((usCCTemp>>8)&0x00FF);
    aucContent[1] = (VOS_UINT8)(usCCTemp&0x00FF);

    stUpdateReq.enAppType       = USIMM_PB_APP;
    stUpdateReq.pucEfContent    = aucContent;
    stUpdateReq.ucRecordNum     = 0;
    stUpdateReq.ulEfLen         = 2;
    stUpdateReq.usEfId          = EFCC;

    if(VOS_OK != USIMM_SetFileReq(MAPS_PB_PID, 0, &stUpdateReq))
    {
        PB_ERROR_LOG("SI_PB_IncreaceCCValue: Update the EFCC File is Error");

        return;
    }

    return;
}


#ifdef __cplusplus
  #if __cplusplus
  }
  #endif
#endif


