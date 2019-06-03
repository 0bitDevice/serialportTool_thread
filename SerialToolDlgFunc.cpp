// SerialToolDlgFunc.cpp: implementation of the CSerialToolDlgFunc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "serialTool.h"
#include "SerialToolDlgFunc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define COMCOUNT	16

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSerialToolDlgFunc::CSerialToolDlgFunc()
{

}

CSerialToolDlgFunc::~CSerialToolDlgFunc()
{

}
void CSerialToolDlgFunc::QueryComm(CStringArray& strArrCom)
{
	HANDLE hCom;
	CString strComName,strCom;

	for (int i = 1; i <= COMCOUNT; ++i)
	{
		strComName.Format("\\\\.\\COM%d", i);
		hCom = CreateFile(strComName, GENERIC_READ | GENERIC_WRITE, 0, 0,  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, 0);
		if(INVALID_HANDLE_VALUE != hCom )
		{
			//能打开该串口，则添加该串口
			strCom.Format("COM%d", i);
			strArrCom.Add(strCom);

			CloseHandle(hCom);
		}
	}
}

int CSerialToolDlgFunc::ConvertHex2CString(CString hexStr, CString dstStr)
{
	return -1;
}

char CSerialToolDlgFunc::ConvertHexChar(char ch)  
{  
	if((ch>='0')&&(ch<='9')) 
		return ch-0x30;  
	else if((ch>='A')&&(ch<='F')) 
		return ch-'A'+10;  
	else if((ch>='a')&&(ch<='f')) 
		return ch-'a'+10;
	else return (-1); 
}

int CSerialToolDlgFunc::ConvertCString2Hex(CString SrcStr,CByteArray &senddata)
{
	int hexData,lowhexData;
	int hexDataLen = 0;
	int SrcStrLen = SrcStr.GetLength();
	
	if(1 == SrcStrLen%3)
	{
		if (' ' != SrcStr.GetAt(SrcStrLen-1))	//防止输入字符时的疏忽：最后一个字符不是空格
		{
			SrcStr.SetAt(SrcStrLen-1, ' ');
		}
		senddata.SetSize((SrcStrLen+1)/2);			
	}
	else
	{
		senddata.SetSize(SrcStrLen/2);			
	}

	for(int i = 0;i<SrcStrLen;)
	{
		if(i >= SrcStrLen)
			break;
		
		char hstr = SrcStr[i++];
		if(hstr == ' ')
		{
			continue;
		}

		if(i >= SrcStrLen)
			break;
		
		char lstr = SrcStr[i++];
		hexData = ConvertHexChar(hstr);
		lowhexData = ConvertHexChar(lstr);
		
		if((hexData == -1)||(lowhexData == -1))
			break;
		else
			hexData = hexData*16 + lowhexData;

		senddata[hexDataLen++] = (char)hexData;
	}
	senddata.SetSize(hexDataLen);
	return hexDataLen;
}

int CSerialToolDlgFunc::ProcessingData(CStdioFile& file, CString& strData)
{
	CString strLine;
	int phraseCount = 0;
	LONG lActual;
	lActual = file.GetPosition();
	if(file.ReadString(strLine))
	{
		if(strLine.Left(6).Compare("$GNGGA") == 0)
		{
			strData += strLine + "\r\n";
			++phraseCount;
		}
		else
		{
			file.Seek(lActual, CFile::begin);
		}
	}
	lActual = file.GetPosition();
	if(file.ReadString(strLine))
	{
		if (strLine.Left(6).Compare("$GNGSA") == 0)
		{
			strData += strLine + "\r\n";
			++phraseCount;
		}
		else
		{
			file.Seek(lActual, CFile::begin);
		}
	}
	lActual = file.GetPosition();
	if(file.ReadString(strLine))
	{
		if (strLine.Left(6).Compare("$GNGSA") == 0)
		{
			strData += strLine + "\r\n";
			++phraseCount;
		}
		else
		{
			file.Seek(lActual, CFile::begin);
		}
	}
	lActual = file.GetPosition();
	if(file.ReadString(strLine))
	{
		if (strLine.Left(6).Compare("$GNRMC") == 0)
		{
			strData += strLine + "\r\n";
			++phraseCount;
		}
		else
		{
			file.Seek(lActual, CFile::begin);
		}
	}
	lActual = file.GetPosition();
	if(file.ReadString(strLine))
	{
		if (strLine.Left(6).Compare("$GNVTG") == 0)
		{
			strData += strLine + "\r\n";
			++phraseCount;
		}
		else
		{
			file.Seek(lActual, CFile::begin);
		}
	}
	lActual = file.GetPosition();
	if(file.ReadString(strLine))
	{
		if (strLine.Left(6).Compare("$GNDHV") == 0)
		{
			strData += strLine + "\r\n";
			++phraseCount;
		}
		else
		{
			file.Seek(lActual, CFile::begin);
		}
	}
	

	return phraseCount;
}

int CSerialToolDlgFunc::RecordData(CStdioFile& file, CString& strData)
{
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	CString strTime, strDataTimed;
	strTime.Format("[%d:%d:%d.%d]", sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);

	strDataTimed = strTime + strData;
	file.Write(strDataTimed, strDataTimed.GetLength());

	return 0;
}