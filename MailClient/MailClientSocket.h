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
#pragma comment(lib, "ws2_32.lib")	/*链接ws2_32.lib动态链接库*/
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
		 * \brief 接收从
		 * \param with_length 
		 * \return 
		 */
		int RecvPOP3AndCout(bool with_length = true);
		/**
		 * \brief 向SMTP服务器发送数据
		 * \param format 要发送的数据的格式
		 * \param ... 不定参数，用于填充format中的数据
		 * \return 向SMTP服务器发送了多少btye数据
		 */
		int SendSMTP(const char* format, ...);
		/**
		* \brief 向POP3服务器发送数据
		* \param format 要发送的数据的格式
		* \param ...	不定参数，用于填充format中的数据
		* \return 向POP3服务器发送了多少btye数据
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
