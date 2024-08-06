char PCSCReader::SecureCommunicationInternalAuthenticate(const std::string& KSenc,
	const std::string& KSmac,
	std::string& SSC,
	std::string& data,
	std::string& RND_IFD,
	std::string& cipherAlgorithm,
	int keyLength) {
	// ��δ�ܱ�����APDU����������
	std::string unprotectedAPDU("\x0C\x88\x00\x00", 4);
	std::string CmdHeader(unprotectedAPDU);
	if (cipherAlgorithm == "AES")
		AesAddPaddingBytes(CmdHeader);
	else if (cipherAlgorithm == "DESede")
		DesAddPaddingBytes(CmdHeader);
	std::string strFillData = RND_IFD;


	if (cipherAlgorithm == "AES")
		AesAddPaddingBytes(strFillData);
	else if (cipherAlgorithm == "DESede")
		DesAddPaddingBytes(strFillData);
	// ��SKenc��������
	std::string strEncData;
	//KencTDES(strFillData, KSenc, strEncData, DES_ENCRYPT);
	//����SSC
	std::string iv = "";
	if (cipherAlgorithm == "AES")
		iv = "00000000000000000000000000000000";
	iv = HexStringToBinary(iv);
	IncreaseSSC(SSC); //SSC += 1
	auto KSenc_hex = BinaryToHexString(KSenc);
	if (cipherAlgorithm == "AES")
	{
		std::string SSC_IV;
		aes_cbc_encode(KSenc, SSC, SSC_IV, iv);
		auto SSC_IV_hex = BinaryToHexString(SSC_IV);
		auto strFillData_hex = BinaryToHexString(strFillData);
		aes_cbc_encode(KSenc, strFillData, strEncData, SSC_IV);
		auto strEncData_hex = BinaryToHexString(strEncData);
	}
	else if (cipherAlgorithm == "DESede")
	{
		KencTDES(strFillData, KSenc, strEncData, DES_ENCRYPT);
	}

	//����d097
	std::string DO97;
	unsigned char x97 = 0x97;
	DO97.push_back(x97);
	DO97.push_back(1);
	DO97.push_back(0);

	// ����DO87,����cmdheader��DO87�õ�M
	std::string DO87;
	unsigned char L = (unsigned char)strFillData.size() + 1;
	unsigned char x87 = 0x87;
	DO87.push_back(x87);
	DO87.push_back(L);
	DO87.push_back(0x01);
	DO87.append(strEncData);
	std::string M = CmdHeader + DO87 + DO97;


	// ����SSC��M�����������õ�N
	std::string N;
	N.append(SSC.data(), SSC.size());
	N.append(M.data(), M.size());
	if (cipherAlgorithm == "AES")
		AesAddPaddingBytes(N);
	else if (cipherAlgorithm == "DESede")
		DesAddPaddingBytes(N);
	// ��SKmac����N����Ϣ��֤��MAC
	std::string CCN;
	//KmacDES(N, KSmac, CCN);
	if (cipherAlgorithm == "AES")
		AESmac(KSmac, N, CCN, keyLength);
	else if (cipherAlgorithm == "DESede")
		KmacDES(N, KSmac, CCN);
	// ��CCN����DO8E
	std::string DO8E("\x8E\x08", 2);
	DO8E.append(CCN.data(), CCN.size());

	// �����ܱ�����APDU
	std::string APDU;
	std::string unprotectedAPDU2("\x0C\x88\x00\x00", 4);
	APDU.append(unprotectedAPDU2.data(), unprotectedAPDU2.size());
	unsigned char le_ = (unsigned char)DO87.size() + (unsigned char)DO8E.size() + (unsigned char)DO97.size();
	APDU.push_back(le_);//DO87+DO8E
	APDU.append(DO87.data(), DO87.size());
	APDU.append(DO97.data(), DO97.size());
	APDU.append(DO8E.data(), DO8E.size());
	/*
	 ����Ӧ�ö��һ���ֽڣ��й����տ��Բ���Ҫ�����ǹ���Ļ����������
	 ����ֽڣ����û�ʧ��
	 */
	APDU.push_back(0);
	// ����APDU
	int dwLen = APDU.size();
	auto APDU_hex = BinaryToHexString(APDU);
	BYTE  RecvBuff[300];
	UINT SendLen;
	DWORD RecvLen;
	memset(RecvBuff, 0, sizeof(RecvBuff));
	RecvLen = sizeof(RecvBuff);
	this->Apdusend(APDU, RecvBuff, RecvLen);
	//cout << "SelectFile RAPDU�� " << BYTE2string(RecvBuff, (UINT)RecvLen) << endl;
	std::string s((char*)&RecvBuff[0], RecvLen);
	std::string RAPDU;//����Ӧ���ݣ�ֻ��SW1 SW2��DO99+8E+08+CC+SW1+SW2
	RAPDU.append(s);
	auto RAPDU_hex = BinaryToHexString(RAPDU);
#define DO99_COMPATIBLE 1
	// ------------------------------------------------------------
	// h.ͨ������DO87��DO99���õ�MAC, ��֤RAPDU CC
	int tlLen = 0;
	std::string RAPDU_DO87 = RAPDUParse(RAPDU, 0x87, &tlLen);
	//    LOGI("SecureCommunicationReadBinary tlLen:%d", tlLen);
		/* ��ʱ������200�ֽڵĴ��ʱ����д�����ص����ݰ���û��DO99 TLV��ǩ��
		   ֻ��DO87��DO8E��Ŀǰ����DO99�ǹ̶���4���ֽ�\x99\E02\x90\x00������
		   ֱ��ʹ��4�ֽڳ���������ȥRAPDU��ȡDO99������ʧ�ܡ�
		   ���淶Doc9303 Part1 Vol2.PDF֮��IV�ڹ�Կ������ʩIV-45��������DO99
		   Ӧ���Ǳ���ǿ�ƴ��ڵģ������ṩ�����Դ���
		   */
#ifdef DO99_COMPATIBLE
	std::string RAPDU_DO99("\x99\x02\x90\x00", 4);
#else
	std::string RAPDU_DO99 = RAPDUParse(RAPDU, 0x99);
#endif
	std::string RAPDU_DO8E = RAPDUParse(RAPDU, 0x8E);
	RAPDU_DO99 = RAPDUParse(RAPDU, 0x99);

	// h.1 ��1ΪSSC��ֵ
	IncreaseSSC(SSC);

	// h.2 ����SSC, DO87��DO99�����������
	std::string K(SSC);
	K += RAPDU_DO87;
	K += RAPDU_DO99;
	if (cipherAlgorithm == "AES")
		AesAddPaddingBytes(K);
	else if (cipherAlgorithm == "DESede")
		DesAddPaddingBytes(K);
	// h.3 ��KSmac����MAC
	std::string CC2;
	if (cipherAlgorithm == "AES")
		AESmac(KSmac, K, CC2, keyLength);
	else if (cipherAlgorithm == "DESede")
		KmacDES(K, KSmac, CC2);
	// h.4 ��CC��RAPDU��DO8E�������Ƚ�
	if (memcmp(CC2.data(), RAPDU_DO8E.data() + 2, 8) != 0) {
#if USE_LOG_LEVEL1
		LOGV("DO8E and CC2 mismatch in Reading binary");
#endif
		return false;
	}
	// i. ��KSenc����DO87����
	std::string RAPDU_DO87_DATA = RAPDU_DO87.substr(tlLen + 1);
	//KencTDES(RAPDU_DO87_DATA, KSenc, data, DES_DECRYPT);
	std::string iv_hex = "";
	std::string SSC_IV;
	if (cipherAlgorithm == "AES")
	{
		iv_hex = "00000000000000000000000000000000";
		auto iv = HexStringToBinary(iv_hex);
		aes_cbc_encode(KSenc, SSC, SSC_IV, iv);
		auto RAPDU_DO87_DATA_hex = BinaryToHexString(RAPDU_DO87_DATA);
		auto KSenc_hex = BinaryToHexString(KSenc);
		auto SSC_IV_hex = BinaryToHexString(SSC_IV);
		aes_cbc_decode(KSenc, RAPDU_DO87_DATA, data, SSC_IV, keyLength);
	}

	else if (cipherAlgorithm == "DESede")
	{
		auto RAPDU_DO87_DATA_hex = BinaryToHexString(RAPDU_DO87_DATA);
		auto KSenc_hex = BinaryToHexString(KSenc);
		KencTDES(RAPDU_DO87_DATA, KSenc, data, DES_DECRYPT);
	}
	DesRemovePaddingBytes(data);

	return true;

}