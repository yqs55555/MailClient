#pragma once
#include <WinSock2.h>
#include <windows.h>
#include <WS2tcpip.h>
#include <map>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <stdarg.h>
#include "Util.h"
#include <iostream>
#include <vector>
#include <fstream>
#pragma comment(lib, "ws2_32.lib")	/*����ws2_32.lib��̬���ӿ�*/
#pragma comment(lib, "libssl.lib")
// #pragma comment(lib, "openssl.lib")

namespace yqs
{
	#define MAX_BUFFER_SIZE 4096

	enum AddressFamily
	{
		InterNetWork = AF_INET,
	};

	enum SocketType
	{
		Stream = SOCK_STREAM,
		Dgram = SOCK_DGRAM
	};

	enum ProtocalType
	{
		IP = IPPROTO_IP,
		Tcp = IPPROTO_TCP,
		Udp = IPPROTO_UDP
	};

	enum MailServer
	{
		QQMail = 1,
		NetEase163Mail
	};
	
	class MailClientSocket
	{
	public:
		MailClientSocket();
		MailServer GetMailServerType() const;
		void SetService(MailServer server, bool use_ssl);
		bool Connect();
		bool Login(char* user_name, char* password, bool relogin = false);
		void SetHead(char* mail_receiver, char* mail_title, char* your_nickname);
		void SetContent(char* content);
		bool AddFile(char* file); 
		bool SendMail();
		bool SendFinish();
		bool ListAllMail();
		bool UseSSL() const;
		~MailClientSocket();
	public:
		const static unsigned int SMTP_PORT_NO_SSL = 25;
		const static unsigned int SMTP_PORT_USE_SSL = 465;
		const static unsigned int POP3_PORT_NO_SSL = 110;
		const static unsigned int POP3_PORT_USE_SSL = 995;
	private:
		int RecvSMTPAndCout(unsigned short state_code, const char* error_message);
		/**
		 * \brief ���մ�
		 * \param with_length 
		 * \return 
		 */
		int RecvPOP3AndCout(bool with_length = true);
		/**
		 * \brief ��SMTP��������������
		 * \param format Ҫ���͵����ݵĸ�ʽ
		 * \param ... �����������������format�е�����
		 * \return ��SMTP�����������˶���btye����
		 */
		int SendSMTP(const char* format, ...);
		/**
		* \brief ��POP3��������������
		* \param format Ҫ���͵����ݵĸ�ʽ
		* \param ...	�����������������format�е�����
		* \return ��POP3�����������˶���btye����
		*/
		int SendPOP3(const char* format, ...);
		void RecvPOP3UntilFinish();
		bool InitSSL(SOCKET socket, SSL_CTX* &ctx, SSL* &ssl);
		void CloseSSL(SSL_CTX* &ctx, SSL* &ssl);
		void DeleteConnect();
		bool ReConnect();
	private:
		int m_reconnect_time_pop3 = 0;
		int m_reconnect_time_smtp = 0;
		SOCKET m_smtp_socket;
		SOCKET m_pop3_socket;
		MailServer m_mail_server;
		char* m_user_name;
		char* m_password;
		char m_receive_buffer[MAX_BUFFER_SIZE + 1];
		char m_send_buffer[MAX_BUFFER_SIZE + 1];
		SSL_CTX* m_pop3_ssl_ctx;
		SSL_CTX* m_smtp_ssl_ctx;
		SSL* m_pop3_ssl;
		SSL* m_smtp_ssl;
		bool m_use_ssl;
		std::vector<char*> m_files;
		char* m_title;
		char* m_content;
		char* m_user_nickname;
		char* m_mail_receiver;
	};
}
