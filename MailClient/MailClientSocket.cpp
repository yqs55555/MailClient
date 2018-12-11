#include "MailClientSocket.h"


namespace yqs
{
	const std::map<MailServer, const char*> m_smtp_address = {
		std::map<MailServer, const char*>::value_type(QQMail, "smtp.qq.com"),
		std::map<MailServer, const char*>::value_type(NetEase163Mail, "smtp.163.com")
	};
	const std::map<MailServer, const char*> m_pop3_address = {
		std::map<MailServer, const char*>::value_type(QQMail, "pop.qq.com"),
		std::map<MailServer, const char*>::value_type(NetEase163Mail, "pop.163.com")
	};

	MailClientSocket::MailClientSocket()
	{
		m_smtp_socket = socket(AddressFamily::InterNetWork, SocketType::Stream, ProtocalType::Tcp);
		m_pop3_socket = socket(AddressFamily::InterNetWork, SocketType::Stream, ProtocalType::Tcp);
	}

	MailServer MailClientSocket::GetMailServerType() const
	{
		return this->m_mail_server;
	}

	void MailClientSocket::SetService(MailServer server, bool use_ssl)
	{
		m_mail_server = server;
		m_use_ssl = use_ssl;
		if (m_mail_server == QQMail && !use_ssl)
		{
			std::cout << "QQ邮箱不支持不开启SSL的POP3连接，已自动开启SSL" << std::endl;
			m_use_ssl = true;
		}
	}

	bool MailClientSocket::Connect()
	{
		sockaddr_in addrSrv;
		hostent* pHostent;
		pHostent = gethostbyname(m_smtp_address.at(m_mail_server));					//得到有关于域名的信息
		addrSrv.sin_addr.S_un.S_addr = *reinterpret_cast<ULONG*>(pHostent->h_addr_list[0]);	//得到smtp服务器的网络字节序的ip地址 
		addrSrv.sin_family = AF_INET;				//地址类型
		addrSrv.sin_port = htons(m_use_ssl ? SMTP_PORT_USE_SSL : SMTP_PORT_NO_SSL);		//解析一个16bit的端口号
		if (connect(m_smtp_socket, reinterpret_cast<SOCKADDR*>(&addrSrv), sizeof(SOCKADDR)) == 0)   //向服务器发送请求 
		{
			std::cout << "成功连接至" << (m_mail_server == QQMail ? "QQ邮箱" : "163邮箱") << "SMTP服务器"
				<< (m_use_ssl ? SMTP_PORT_USE_SSL : SMTP_PORT_NO_SSL) << "号端口" << std::endl;
		}
		else
		{
			std::cout << "连接至" << (m_mail_server == QQMail ? "QQ邮箱" : "163邮箱") << "SMTP服务器"
				<< (m_use_ssl ? SMTP_PORT_USE_SSL : SMTP_PORT_NO_SSL) << "号端口失败" << std::endl;
			return false;
		}

		pHostent = gethostbyname(m_pop3_address.at(m_mail_server));					//得到有关于域名的信息
		addrSrv.sin_addr.S_un.S_addr = *reinterpret_cast<ULONG*>(pHostent->h_addr_list[0]);	//得到pop3服务器的网络字节序的ip地址 
		addrSrv.sin_family = AF_INET;				//地址类型
		addrSrv.sin_port = htons(m_use_ssl ? POP3_PORT_USE_SSL : POP3_PORT_NO_SSL);		//解析一个16bit的端口号
		if (connect(m_pop3_socket, reinterpret_cast<SOCKADDR*>(&addrSrv), sizeof(SOCKADDR)) == 0)   //向服务器发送请求  
		{
			std::cout << "成功连接至" << (m_mail_server == QQMail ? "QQ邮箱" : "163邮箱") << "POP3服务器"
				<< (m_use_ssl ? POP3_PORT_USE_SSL : POP3_PORT_NO_SSL) << "号端口" << std::endl;
		}
		else
		{
			std::cout << "连接至" << (m_mail_server == QQMail ? "QQ邮箱" : "163邮箱") << "POP3服务器"
				<< (m_use_ssl ? POP3_PORT_USE_SSL : POP3_PORT_NO_SSL) << "号端口失败" << std::endl;
			return false;
		}

		return true;
	}

	bool MailClientSocket::Login(char* user_name, char * password, bool relogin)
	{
		Connect();
		if (m_mail_server == QQMail)
		{
			if (strstr(user_name, "@qq.com") == nullptr)
			{
				strcat(user_name, "@qq.com");
			}
		}
		else if (m_mail_server == NetEase163Mail)
		{
			if (strstr(user_name, "@163.com") == nullptr)
			{
				strcat(user_name, "@163.com");
			}
		}
		m_user_name = user_name;
		m_password = password;
		if (m_use_ssl)
		{
			InitSSL(m_smtp_socket, m_smtp_ssl_ctx, m_smtp_ssl);
			InitSSL(m_pop3_socket, m_pop3_ssl_ctx, m_pop3_ssl);
		}

		if (!relogin)
		{
			RecvSMTPAndCout(220, "");
			RecvPOP3AndCout();
		}

		SendSMTP("HELO %s\r\n", user_name);
		RecvSMTPAndCout(250, "请输入正确的用户名");	//250

		SendSMTP("AUTH LOGIN\r\n");
		RecvSMTPAndCout(334, "请求登录失败");

		char base64_user_name[1024];
		EncodeBase64(user_name, base64_user_name, strlen(user_name));
		SendSMTP("%s\r\n", base64_user_name);
		RecvSMTPAndCout(334, "");

		char base64_password[1024];
		EncodeBase64(password, base64_password, strlen(password));
		SendSMTP("%s\r\n", base64_password);
		if (!RecvSMTPAndCout(235, "用户名或密码错误"))
		{
			return false;
		}

		SendPOP3("USER %s\r\n", m_user_name);
		RecvPOP3AndCout();

		SendPOP3("PASS %s\r\n", password);
		RecvPOP3AndCout();

		std::cout << "\n登录成功" << std::endl;

		return true;
	}

	void MailClientSocket::SetHead(char * mail_receiver, char * mail_title, char* your_nickname)
	{
		m_mail_receiver = mail_receiver;
		m_title = mail_title;
		m_user_nickname = your_nickname;
	}

	void MailClientSocket::SetContent(char *content)
	{
		m_content = content;
	}

	bool MailClientSocket::AddFile(char* file)
	{
		std::ifstream file_reader;
		file_reader.open(file);

		if (file_reader.is_open())
		{
			std::vector<char*>::iterator it = std::find(m_files.begin(), m_files.end(), file);
			if (it == m_files.end())
			{
				m_files.push_back(file);
				std::cout << "附件" << file << "添加成功" << std::endl;
				return true;
			}
			else
			{
				std::cout << "待发送列表中已存在" << file << std::endl;
				return false;
			}
		}
		else
		{
			std::cout << "无法找到或打开" << file << std::endl;
			return false;
		}
		return true;
	}

	bool MailClientSocket::SendMail()
	{
		int times = 5;
		int state = 0;
		while(true)
		{
			SendSMTP("MAIL FROM:<%s>\r\n", m_user_name);
			state = RecvSMTPAndCout(250, "");
			if(state <= 0 && times > 0)
			{
				ReConnect();
				times--;
			}
			else
			{
				break;
			}
		}

		SendSMTP("RCPT TO: <%s>\r\n", m_mail_receiver);
		RecvSMTPAndCout(250, "");

		SendSMTP("DATA\r\n");
		RecvSMTPAndCout(250, "");

		if (m_files.size() == 0)
		{
			SendSMTP("\
From: \"%s\"<%s>\r\n\
To: %s\r\n\
Subject: %s\r\n\
MIME-Version: 1.0\r\n\
Content-Type: multipart/mixed;\r\n\
		boundary=\"mail_detail\"\r\n\r\n", m_user_nickname, m_user_name, m_mail_receiver, m_title);

			SendSMTP("--mail_detail\r\nContent-Type: text/plain;\r\n  charset=\"gb2312\"\r\n\r\n%s\r\n\r\n", m_content);

			SendSMTP("--mail_detail--\r\n\r\n");
		}
		else
		{
			SendSMTP("\
From: \"%s\"<%s>\r\n\
To: %s\r\n\
Subject: %s\r\n\
MIME-Version: 1.0\r\n\
Content-Type: multipart/mixed;\r\n\
		boundary=\"mail_file\"\r\n\r\n", m_user_nickname, m_user_name, m_mail_receiver, m_title);

			SendSMTP("\
--mail_file\r\n\
Content-Type: multipart/alternative;\r\n\
		boundary=\"mail_detail\";\r\n\r\n");
			SendSMTP("--mail_detail\r\nContent-Type: text/plain;\r\n  charset=\"gb2312\"\r\n\r\n%s\r\n\r\n", m_content);

			SendSMTP("--mail_detail--\r\n\r\n");

			for (int i = 0; i < m_files.size(); i++)
			{
				std::ifstream file_reader;
				file_reader.open(m_files[i], std::ios::binary);
				if (!file_reader.is_open())
				{
					std::cout << "打开文件" << m_files[i] << "失败" << std::endl;
					return false;
				}

				memset(m_send_buffer, 0, sizeof(m_send_buffer));
				sprintf_s(m_send_buffer, "\
--mail_file\r\n\
Content-Type: application/octet-stream;\r\n\
	charset=\"gb18030\";\r\n\
	name=\"%s\"\r\n\
Content-Disposition: attachment; filename=\"%s\"\r\n\
Content-Transfer-Encoding: base64\r\n\r\n", GetFileName(m_files[i]).c_str(), GetFileName(m_files[i]).c_str());
				m_use_ssl ? SSL_write(m_smtp_ssl, m_send_buffer, strlen(m_send_buffer)) : send(m_smtp_socket, m_send_buffer, strlen(m_send_buffer), 0);

				file_reader.seekg(0, std::ios::end);
				long file_length = file_reader.tellg();
				file_reader.seekg(0, std::ios::beg);
				char file_buffer[1024];		//将文件读至此buffer
				char file_send_buffer[1024];	//将要发送此buffer至服务器
				long current_send_length = 0;

				std::cout << GetFileName(m_files[i]).c_str() << "(" << file_length << "btyes) : 已发送";
				int real_btye = 1;
				while (real_btye != 0)
				{
					memset(file_buffer, 0, sizeof(file_buffer));
					memset(file_send_buffer, 0, sizeof(file_send_buffer));
					file_reader.read(file_buffer, 390);						//需要是3的倍数，如果不是3的倍数进行base64加密时会多出‘=’符号
					real_btye = file_reader.gcount();
					EncodeBase64(file_buffer, file_send_buffer, real_btye);	//进行base64加密
					if (real_btye == 0)
					{
						file_send_buffer[real_btye] = '\r';
						file_send_buffer[real_btye + 1] = '\n';
						file_send_buffer[real_btye + 2] = '\0';
						(m_use_ssl ? SSL_write(m_smtp_ssl, file_send_buffer, 2) : send(m_smtp_socket, file_send_buffer, 2, 0));
					}
					else
					{
						(m_use_ssl ? SSL_write(m_smtp_ssl, file_send_buffer, strlen(file_send_buffer)) : send(m_smtp_socket, file_send_buffer, strlen(file_send_buffer), 0));
					}

					/*发送进度显示 Start*/
					current_send_length += real_btye;
					int precent = (static_cast<double>(current_send_length) / file_length) * 100;
					std::cout << precent << "/100%";
					int str_cout = std::to_string(precent).size() + 5;
					while (str_cout-- != 0)
					{
						std::cout << "\b";
					}
					/*发送进度显示 Finish*/
				}
				std::cout << "\nFinish" << std::endl;
				file_reader.close();
			}
			SendSMTP("--mail_file--\r\n\r\n");
		}
		return true;
	}

	bool MailClientSocket::SendFinish()
	{
		SendSMTP(".\r\n");
		RecvSMTPAndCout(250, "发送失败");

		SendSMTP("QUIT\r\n");
		RecvSMTPAndCout(221, "");

		std::cout << "发送邮件成功" << std::endl;

		return true;
	}

	bool MailClientSocket::ListAllMail()
	{
		int times = 5;
		int state = 0;
		while (true)
		{
			SendPOP3("STAT\r\n");
			std::cout << "邮件总数：";
			state = RecvPOP3AndCout();
			if (state <= 0 && times > 0)
			{
				ReConnect();
				times--;
			}
			else
			{
				break;
			}
		}

		std::cout << "各封邮件及其大小：";
		SendPOP3("LIST\r\n");
		RecvPOP3UntilFinish();

		std::cout << "请选择一封邮件查看 : \n";
		int select;
		std::cin >> select;

		SendPOP3("RETR %d\r\n", select);
		RecvPOP3UntilFinish();

		return true;
	}

	bool MailClientSocket::UseSSL() const
	{
		return m_use_ssl;
	}


	MailClientSocket::~MailClientSocket()
	{
		DeleteConnect();
	}

	int MailClientSocket::RecvSMTPAndCout(unsigned short state_code, const char* error_message)
	{
		memset(m_receive_buffer, 0, sizeof(m_receive_buffer));
		int len = m_use_ssl ? SSL_read(m_smtp_ssl, m_receive_buffer, MAX_BUFFER_SIZE) : recv(m_smtp_socket, m_receive_buffer, MAX_BUFFER_SIZE, 0);
		std::cout << "\nFrom smtp :" << m_receive_buffer;

		int recv_code = 100 * (m_receive_buffer[0] - '0') + 10 * (m_receive_buffer[1] - '0') + (m_receive_buffer[2] - '0');
		if (recv_code != state_code && error_message != "")
		{
			std::cout << error_message << std::endl;
		}
		return len;
	}

	int MailClientSocket::RecvPOP3AndCout(bool with_length)
	{
		memset(m_receive_buffer, 0, sizeof(m_receive_buffer));
		int len = m_use_ssl ? SSL_read(m_pop3_ssl, m_receive_buffer, MAX_BUFFER_SIZE) : recv(m_pop3_socket, m_receive_buffer, MAX_BUFFER_SIZE, 0);
		if (with_length)
		{
			std::cout << "\nFrom pop3(len = " << len << "):" << m_receive_buffer;
		}
		else
		{
			std::cout << m_receive_buffer;
		}
		return len;
	}

	int MailClientSocket::SendSMTP(const char * format, ...)
	{
		va_list arg_list;
		va_start(arg_list, format);

		memset(m_send_buffer, 0, sizeof(m_send_buffer));
		vsprintf_s(m_send_buffer, format, arg_list);
		va_end(arg_list);
		int len = (m_use_ssl ? SSL_write(m_smtp_ssl, m_send_buffer, strlen(m_send_buffer)) : send(m_smtp_socket, m_send_buffer, strlen(m_send_buffer), 0));
		if (len == -1)
		{
			std::cout << "已断开连接" << std::endl;
		}
		return len;
	}

	int MailClientSocket::SendPOP3(const char * format, ...)
	{
		va_list arg_list;
		va_start(arg_list, format);

		memset(m_send_buffer, 0, sizeof(m_send_buffer));
		vsprintf_s(m_send_buffer, format, arg_list);
		va_end(arg_list);

		int len = (m_use_ssl ? SSL_write(m_pop3_ssl, m_send_buffer, strlen(m_send_buffer)) : send(m_pop3_socket, m_send_buffer, strlen(m_send_buffer), 0));
		return len;
	}

	void MailClientSocket::RecvPOP3UntilFinish()
	{
		std::cout << "Recv begin" << std::endl;
		while (true)
		{
			int len = RecvPOP3AndCout(false);
			if (m_receive_buffer[len - 3] == '.' && m_receive_buffer[len - 2] == '\r' && m_receive_buffer[len - 1] == '\n')
			{
				std::cout << "\nRecv finish" << std::endl;
				break;
			}
		}
	}
	bool MailClientSocket::InitSSL(SOCKET socket, SSL_CTX* &ctx, SSL* &ssl)
	{
		SSL_library_init();
		SSL_load_error_strings();
		ctx = SSL_CTX_new(SSLv23_client_method());
		if (ctx == nullptr)
		{
			std::cout << "SSL CTX ERROR" << std::endl;
		}
		ssl = SSL_new(ctx);
		SSL_set_fd(ssl, socket);
		SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);

		SSL_connect(ssl);
		//std::cout << SSL_get_cipher(m_pop3_ssl) << std::endl;

		return true;
	}
	void MailClientSocket::CloseSSL(SSL_CTX* &ctx, SSL* &ssl)
	{
		if (ssl != nullptr)
		{
			SSL_shutdown(ssl);  /* send SSL/TLS close_notify */
			// SSL_free(ssl);
			// ssl = nullptr;
		}
		if (ctx != nullptr)
		{
			SSL_CTX_free(ctx);
			ERR_free_strings();
			EVP_cleanup();
			CRYPTO_cleanup_all_ex_data();
		}
	}
	void MailClientSocket::DeleteConnect()
	{
		if (m_use_ssl)
		{
			CloseSSL(m_smtp_ssl_ctx, m_smtp_ssl);
			CloseSSL(m_pop3_ssl_ctx, m_pop3_ssl);
		}
		// shutdown(m_smtp_socket, 2);
		// shutdown(m_pop3_socket, 2);
		closesocket(m_smtp_socket);
		closesocket(m_pop3_socket);

		m_smtp_socket = socket(AddressFamily::InterNetWork, SocketType::Stream, ProtocalType::Tcp);
		m_pop3_socket = socket(AddressFamily::InterNetWork, SocketType::Stream, ProtocalType::Tcp);
	}

	bool MailClientSocket::ReConnect()
	{
		DeleteConnect();
		if (!Login(m_user_name, m_password))
		{
			std::cout << "登录失败，请重新开启" << std::endl;
			return false;
		}
		return true;
	}
}
