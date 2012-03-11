//---------------------------------------------------------------------------

#include "inet.h"
#include "document.h"
/*
function GetUrlInfo(const AUrl: string; var AUrlInfo: TUrlInfo): boolean;
var
  URLComponents: TURLComponents;
  Scheme: array[0..INTERNET_MAX_SCHEME_LENGTH] of Char;
  HostName: array[0..INTERNET_MAX_HOST_NAME_LENGTH] of Char;
  UserName: array[0..INTERNET_MAX_USER_NAME_LENGTH] of Char;
  Password: array[0..INTERNET_MAX_PASSWORD_LENGTH] of Char;
  UrlPath: array[0..INTERNET_MAX_PATH_LENGTH] of Char;
  ExtraInfo: array[0..255] of Char;
begin
  Result := False;
  if Length(AUrl) > INTERNET_MAX_URL_LENGTH then Exit;

  URLComponents.dwStructSize := SizeOf(URLComponents); //�T�C�Y�m��

  URLComponents.nScheme := INTERNET_SCHEME_DEFAULT;
  URLComponents.lpszScheme        := Scheme;    //Scheme�̗̈�̊m��
  URLComponents.dwSchemeLength    := SizeOf(Scheme);

  URLComponents.lpszHostName      := HostName;  //HostName�̗̈�̊m��
  URLComponents.dwHostNameLength  := SizeOf(HostName);

  URLComponents.lpszUserName      := UserName;  //UserName�̗̈�̊m��
  URLComponents.dwUserNameLength  := SizeOf(UserName);

  URLComponents.lpszPassword      := Password;  //Password�̗̈�̊m��
  URLComponents.dwPasswordLength  := SizeOf(Password);

  URLComponents.lpszUrlPath       := UrlPath;   //UrlPath�̗̈�̊m��
  URLComponents.dwUrlPathLength   := SizeOf(UrlPath);

  URLComponents.lpszExtraInfo     := ExtraInfo; //ExtraInfo�̗̈�̊m��
  URLComponents.dwExtraInfoLength := SizeOf(ExtraInfo);

  Result := InternetCrackUrl(PChar(AUrl), Length(AUrl), ICU_DECODE,
              URLComponents);
  with AUrlInfo do
  begin
    HostName := URLComponents.lpszHostName;
    UrlPath := URLComponents.lpszUrlPath;
    Scheme := URLComponents.lpszScheme;
  end;
end;

void SetTimeOut(HINTERNET hSession) //TimeOut�̐ݒ�
{
    int TimeOut = 3 * 1000; //30�b
    InternetSetOption(hSession, INTERNET_OPTION_RECEIVE_TIMEOUT, &TimeOut,
        sizeof(TimeOut));
}

void GetHttpStatus(int &Result) //StatusCode�̎擾
{
    DWORD Reserved = 0;
    DWORD Len = sizeof(Result);
    HttpQueryInfo(hRequest, HTTP_QUERY_STATUS_CODE or HTTP_QUERY_FLAG_NUMBER,
        &Result, Len, Reserved);
}

void GetContentSize(int &Result, int &ContentLen); //StatusCode�̎擾
{
    DWORD Reserved = 0;
    DWORD Len = SizeOf(Result);
    HttpQueryInfo(hRequest, HTTP_QUERY_CONTENT_LENGTH or HTTP_QUERY_FLAG_NUMBER,
        &ContentLen, Len, Reserved);
}
//---------------------------------------------------------------------------
int GetFileFromInternet(AnsiString AUrl, void *P, int Len, TMemoryStream *AStream);
var
  hSession, hConnect, hRequest: hInternet;
  RequestMethod, Header: string;
  Buf:array[0..1023] of Char;
  ReadCount:Cardinal;
  UrlInfo: TUrlInfo;
  ContentLen:Integer;
  TotalCount:Integer;
{
    int Result = 0;
    if not GetUrlInfo(AUrl, UrlInfo) then Exit;
    hSession := InternetOpen(nil, INTERNET_OPEN_TYPE_PRECONFIG, nil, nil, 0);
    try
      SetTimeOut;
      hConnect := InternetConnect(hSession, PChar(UrlInfo.HostName),
        INTERNET_DEFAULT_HTTP_PORT, nil, nil, INTERNET_SERVICE_HTTP, 0, 0);
      try
        if P = nil then
          RequestMethod := 'GET'
        else
          RequestMethod := 'POST';
        hRequest := HttpOpenRequest(hConnect, PChar(RequestMethod),
                      PChar(UrlInfo.UrlPath), 'HTTP/1.0', nil, nil,
                      INTERNET_FLAG_RELOAD, 0);
        try
          if P = nil then
            Header := ''
          else
            Header := 'Content-type: application/x-www-form-urlencoded';
          HttpSendRequest(hRequest, PChar(Header),
            Length(Header), P, Len);
          GetHttpStatus;
          if Result <> HTTP_STATUS_OK then Exit;
          GetContentSize;
  //        ShowMessage(Chr(13)+Chr(13)+Chr(13)+Chr(13)+Chr(13)+Chr(13)+Chr(13)+Chr(13)+Chr(13)+Chr(13)+Chr(13)+Chr(13)+IntToStr(ContentLen));
          TotalCount:=0;
          while True do //�X�g���[���ɏ�������
          begin
            if not InternetReadFile(hRequest, @Buf, SizeOf(Buf), ReadCount) then
            begin
              Break;
            end;
            if ReadCount = 0 then
            begin
              Break;
            end
            else
            begin
              AStream.Write(Buf, ReadCount);
              Inc(TotalCount,ReadCount);
              if TotalCount>ContentLen then
              begin
                ContentLen:=TotalCount;
              end;
              //ProgressWaitMessage((TotalCount*100) div ContentLen);
            end;
          end;
        finally
          InternetCloseHandle(hRequest);
        end;
      finally
        InternetCloseHandle(hConnect);
      end;
    finally
      InternetCloseHandle(hSession);
    end;
end;
//---------------------------------------------------------------------------
bool GetHtmlSourceF(AnsiString FileName, AnsiString url)
{
    TMemoryStream *MemStream = new TMemoryStream();  //HTML�\�[�X�̊i�[�ꏊ�Ƀ��������g��
    TFileStream *FileStream = new TFileStream(FileName, fmCreate);
    bool result = GetFileFromInternet(url, NULL, 0, MemStream) == HTTP_STATUS_OK;
    if (result){//HTML�\�[�X���擾
        MemStream->Position = 0;  //�ǂݎ��ʒu��擪�Ɏ����Ă���
        FileStream->CopyFrom(MemStream, MemStream.Size);  //File�ɓǂ݂���
    }
    delete MemStream;
    delete FileStream;
}
*/
//---------------------------------------------------------------------------
bool IsInternetConnected()
{
    DWORD ConnectType = INTERNET_CONNECTION_MODEM + INTERNET_CONNECTION_LAN +
                        INTERNET_CONNECTION_PROXY;
    return InternetGetConnectedState(&ConnectType, 0);
}
//---------------------------------------------------------------------------
void GetHtml(AnsiString Url, AnsiString &html)
{
    HINTERNET hSession;
    hSession = InternetOpen(AppTitle, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if( hSession ){
        HINTERNET hService;
        hService = InternetOpenUrl( hSession, Url.c_str(), NULL, 0, 0, 0 );
        if(hService){
            while(1){
                char lpBuffer[1024+1]; //��x�ɓǂݍ��ރo�C�g��
                DWORD dwBytesRead;     //�ǂݍ���byte��
                InternetReadFile(hService, lpBuffer, 1024, &dwBytesRead);
                if (dwBytesRead == 0){
                    break; //�ǂݍ���byte����0�ɂȂ����烋�[�v�𔲂���
                }
                lpBuffer[dwBytesRead] = 0; //�ǂݍ��񂾃f�[�^�̍Ō��0���������ݕ�����
                html += lpBuffer; //html�ɒǉ�
            }
        }
        InternetCloseHandle(hService);
    }
    InternetCloseHandle(hSession);
}
//---------------------------------------------------------------------------
#pragma package(smart_init)
