#pragma once

#define RF_RECV_IDCARD_INFO_LEN    0x100
#define RF_RECV_IDCARD_PHOTO_LEN   0x400
#define RF_RECV_IDCARD_INFO_POS1    0x04
#define PCSCDLL extern"C" _declspec(dllexport)
typedef struct tagIDCardInfor
{
	unsigned char name[30];				// ����
	unsigned char sex[2];				// �Ա�
	unsigned char nation[4];			// ����
	unsigned char birthday[16];			// �������� YYYYMMDD
	unsigned char addr[70];				// ��ַ
	unsigned char IDSn[36];				// ���֤��
	unsigned char signOrg[30];			// ǩ����λ
	unsigned char startDate[16];		// ǩ������
	unsigned char endDate[16];			// ��Ч����
	unsigned char passID[18];			// ͨ��֤����
	unsigned char issuesNum[4];			// ǩ������
	unsigned char rfuData1[6];			// Ԥ������
	unsigned char typeFlag[2];			// ֤�����ͱ�ʶ
	unsigned char rfuData2[6];			// Ԥ������
} t_idcardinfor;

typedef struct ChipAuthenticData
{
	int BAC;
	int PACE;
	int CA;
	int AA;
	int PA;
	int PASOD;
	int PADS;
	int PADGHash;
} chipAuthenticData;

typedef struct ChipData_Doc9303
{
	char pDG1[128];
	char pDG2[81920];
	char pDG3[81920];
	char pDG4[4096];
	char pDG5[4096];
	char pDG6[1024];
	char pDG7[20734];
	char pDG8[1024];
	char pDG9[1024];
	char pDG10[1024];
	char pDG11[1024];
	char pDG12[1024];
	char pDG13[1024];
	char pDG14[1024];
	char pDG15[1024];
	char pDG16[1024];
	char pCardAccess[1024];
	char pCardSecurity[1024];
	char pSOD[8192];
	int iDG1;
	int iDG2;
	int iDG3;
	int iDG4;
	int iDG5;
	int iDG6;
	int iDG7;
	int iDG8;
	int iDG9;
	int iDG10;
	int iDG11;
	int iDG12;
	int iDG13;
	int iDG14;
	int iDG15;
	int iDG16;
	int iSOD;
	int iCardAcess;
	int iCardSecurity;
} ChipData_Doc9303_Result;
PCSCDLL int PCSCGetChip_given_three_parts_PACE(std::string serialnum, std::string birthdate, std::string expiredate, int cardType, ChipAuthenticData& chipAuthenticData, ChipData_Doc9303& chipData_9303);
PCSCDLL int PCSCGetChip_given_three_parts_BAC(std::string serialnum, std::string birthdate, std::string expiredate, int cardType, ChipAuthenticData& chipAuthenticData, ChipData_Doc9303& chipData_9303);
PCSCDLL int PCSCGetChipBAC(std::string mrz, int cardType, ChipAuthenticData& chipAuthenticData, ChipData_Doc9303& chipData_9303);
PCSCDLL int PCSCGetChipPACE(std::string mrz, int cardType, ChipAuthenticData& chipAuthenticData, ChipData_Doc9303& chipData_9303);
int PCSCGecChipAA();
int PCSCGetChip(std::string& mrz, int cardType, ChipAuthenticData& chipAuthenticData, ChipData_Doc9303& chipData_9303);
int getEChipDG11(std::string& name, std::string& selfID);
int PCSC_GetIDCard();
std::string BYTE2string1(byte* bByte, UINT iLength);