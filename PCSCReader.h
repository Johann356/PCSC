#pragma once
#include<string>
#include<atlstr.h>
#include <openssl/des.h>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/ec.h>
#include <openssl/obj_mac.h>
#include "STEFFile.h"
#include "EFFile.h"
#include "MRTD.h"
#define EF_CARDACCESS_FILENAME	    "USB_TEMP\\EF_CardAccess.dat"
#define EF_COM_FILENAME	    "USB_TEMP\\EF_COM.dat"
#define EF_SOD_FILENAME	    "USB_TEMP\\EF_SOD.dat"

using namespace std;
std::string BYTE2string(byte* bByte, UINT iLength);
char ToLetter(byte bNum);
extern void MakeFullPath1(char* fullpath, const char* path);

class PCSCReader
{
public:
	MRTD mrtd;
	unsigned char CardType;
	struct ChipAuthenticData
	{
		int BAC;
		int PACE;
		int CA;
		int AA;
		int PA;
		int PASOD;
		int PADS;
		int PADGHash;
	} ChipAuthenticResult;

	struct ChipData_Doc9303
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
public:
	PCSCReader();
	~PCSCReader();
	char SetCardType(unsigned char type) {
		this->CardType = type;
		return true;
	}
	//��ʼ��������
	int Initalize();
	//���Ӷ�����
	int Connect(string& atr );
	//����apdu����
	int Apdusend(string& sendData, BYTE* RecvBuff, DWORD& RecvLen);
	//��ȡ��λ��Ϣ
	int Getatr(string& atr);
	int DissConnect();
	char BuildKencAndKmac(const std::string& mrzInfo,
		std::string& Kenc,
		std::string& Kmac);
	char BuildIFD(std::string& RND_IFD,
		std::string& Kifd);

	char ICCMutualAuthenticate(std::string& RND_IFD,
		std::string& RND_ICC,
		std::string& Kifd,
		std::string& Kenc,
		std::string& Kmac,
		std::string& KSenc,
		std::string& KSmac);

	char ActiveAuthentication(char* DG15_file_path);

	char PassiveAuthentication(char* SOD_file_path);

	char BuildSSC(std::string& RND_IFD,
		std::string& RND_ICC,
		std::string& SSC);

	char ICCReadEF(std::string& KSenc, std::string& KSmac, std::string& SSC, EF_NAME name,
		std::string& EF_Data);
	char ICCReadEFPACE(std::string& KSenc, std::string& KSmac, std::string& SSC, EF_NAME name,
		std::string& EF_Data,int keyLength,const std::string&);
	char SecureCommunicationSeletAPPPACE(
		std::string& cmdData,
		const std::string& KSenc,
		const std::string& KSmac,
		std::string& SSC,
		int keyLength,
		const std::string& cipherAlgorithm);
	/**
 * ������һϵ�нӿ�ʱΪ������һЩ���յ�֧�֣����໤��û�а�ȫ���ܣ�����Ҫ���ݻ����뽨����ȫͨ��
 * ֱ��ͨ��APDU�����ȡоƬ
 */
	char DirectReadEF(
		EF_NAME name,
		std::string& EF_Data);

	//STEFFile
	char DirectCommunication(
		STEFFile* file,
		std::string& data);

	//STEFFile
	char DirectSelectFile(
		STEFFile* file);

	char DirectReadFile(
		unsigned short offset,
		unsigned short len,
		std::string& data);

	char DirectReadBinary(
		unsigned short offset,
		unsigned short chunkSize,
		std::string& data);


	char SecureCommunication(
		STEFFile* file,
		const std::string& KSenc,
		const std::string& KSmac,
		std::string& SSC,
		std::string& data);

	char SecureCommunicationSelectFile(
		STEFFile* file,
		const std::string& KSenc,
		const std::string& KSmac,
		std::string& SSC);

	char SecureCommunicationReadBinary(
		const std::string& KSenc,
		const std::string& KSmac,
		std::string& SSC,
		unsigned short offset,
		unsigned short chunkSize,
		std::string& data);

	char SecureCommunicationReadFile(
		const std::string& KSenc,
		const std::string& KSmac,
		std::string& SSC,
		unsigned short offset,
		unsigned short len,
		std::string& data);

	char ReadEchipInfo(std::string& codetonfc);
	BOOL ReadEChipInfoPACE(std::string& codetonfc);
	BOOL SelectPACE(std::string& oid);
	BOOL ICCRequestRandomNumberPACE(__out std::string& Z_ICC);
	char PostPassportCommand(std::string& request, std::string& response);

	BOOL BuildKpai(
		__in const std::string& mrzInfo,
		__out std::string& Kpai,
		__out std::string& hash_aogorithm);

	BOOL BuildKencandKmacPACE(
		__in const std::string& KA,
		__in int keyLength,
		__in std::string cipherAlgorithm,
		__out std::string& Kenc,
		__out std::string& Kmac);

	BOOL map_G_hat(EC_POINT*& G_hat,std::string& S_ICC, std::string& cipherAlgorithm, int keyLength);

	BOOL map_G_hat(BIGNUM*& G_hat, std::string& S_ICC, std::string& cipherAlgorithm, int keyLength);

	BOOL BuildMapKey(std::string& PKmap, std::string& SKmap, int ecc_id);

	BOOL RandomNumberMap(std::string& PKmap_IC, const std::string& PKmap, const std::string& SKmap);

	BOOL MutualAuthenticate(EC_POINT* G_hat, std::string& PKDH_IC, std::string& SKDH_IFD, std::string& PKDH_IFD, int ecc_id);

	BOOL MutualAuthenticate(BIGNUM*& G_hat, std::string& PKDH_IC, std::string& SKDH_IFD, std::string& PKDH_IFD, BIGNUM*& prime);

	BOOL ExchangeT(std::string& TIFD, std::string& TICC_my);

	void ChipReaderReadFileResultOperate(EF_NAME name, char* result, unsigned char type, int length);

	char GetResult(EF_NAME efName, string& retData);

	char EF_DG2_SetResultPath(string path);
	char SecureCommunicationPACE(
		STEFFile* file,
		const std::string& KSenc,
		const std::string& KSmac,
		std::string& SSC,
		std::string& data,
		int keyLength,
		const std::string& cipherAlgorithm);

	char SecureCommunicationSelectFilePACE(
		STEFFile* file,
		const std::string& KSenc,
		const std::string& KSmac,
		std::string& SSC,
		int keyLength,
		const std::string& cipherAlgorithm);

	char SecureCommunicationReadBinaryPACE(
		const std::string& KSenc,
		const std::string& KSmac,
		std::string& SSC,
		unsigned short offset,
		unsigned short chunkSize,
		std::string& data,
		int keyLength,
		const std::string& cipher_Algorithm);

	char SecureCommunicationReadFilePACE(
		const std::string& KSenc,
		const std::string& KSmac,
		std::string& SSC,
		unsigned short offset,
		unsigned short len,
		std::string& data,
		int keyLength,
		const std::string& cipher_Algorithm);
	int Buildsecp256r1(EC_GROUP*& ec_group);
	int BuildGFP(DH*& dh,int id);
	std::string baseFolder;

private:
	//����������
	CString ReaderName;
	//�����ܿ����ӵľ��
	SCARDHANDLE	hCard;
	DWORD m_dAttrib;
	SCARDCONTEXT hContext;
	STEFFileSystem st_efs;
	BOOL ReadBinary_CardAccess(
		__in UINT16 offset,
		__in UINT16 chunkSize,
		__out std::string& data);


};





