#include <iostream>
#include "MailClientSocket.h"
#include "Util.h"

#define CLEAR_BUFFER_BEFORE(delim) std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), delim);

#define USE_TEST
#define TEST_QQMAIL
#define TEST_163MAIL

char receiver[128] = { "849561570@qq.com" };
char mail_title[512] = { "�����ҵĲ����ĵ�" };
char mail_body[4096] = { "�����ҵĲ����ĵ�����ע����գ������ϻظ�" };
char mailsender_name[64] = { "yqs" };
std::vector<char*> files;
bool use_ssl = true;

#ifdef TEST_QQMAIL
char user_name[256] = { "849561570" };
char password[256] = { "vrwlsaixaoqcbbhb" };
#else
char user_name[128] = { "18020876239" };
char password[128] = { "y5q5s5" };
#endif

std::map<unsigned short, const char*> mail_server = {
	{std::map<unsigned short,const char*>::value_type(1, "QQ����") },
	{std::map<unsigned short,const char*>::value_type(2, "163����") }
};

bool Test()
{
	use_ssl = true;
	// files.push_back(const_cast<char*>("G://SE//test.txt"));
	// files.push_back(const_cast<char*>("G://SE//����//Chapter3.rar"));

	yqs::MailClientSocket* client_socket = new yqs::MailClientSocket();
	client_socket->SetService(
#ifdef TEST_QQMAIL
		yqs::QQMail, use_ssl
#else 
		yqs::NetEase163Mail, use_ssl
#endif
	);

	if (!client_socket->Login(user_name, password))
	{
		std::cout << "��¼�˺�ʧ��" << std::endl;
		return false;
	}
	while (true)
	{
		int state_code = 0;
		do
		{
			std::cout << "\n��ѡ���ܣ�\n1�������ʼ�\n2�������ʼ�\n3���˳�����\n";
			std::cin >> state_code;
		} while (state_code < 1 || state_code > 3);
		if (state_code == 1)
		{
			client_socket->SetHead(receiver, mail_title, mailsender_name);
			client_socket->SetContent(mail_body);
			for (std::vector<char*>::iterator it = files.begin(); it != files.end(); ++it)
			{
				if (!client_socket->AddFile(*it))
				{
					return false;
				}
			}
			if (!client_socket->SendMail())
			{
				return false;
			}
			if (!client_socket->SendFinish())
			{
				return false;
			}
		}
		else if (state_code == 2)
		{
			if (!client_socket->ListAllMail())
			{
				return false;
			}
		}
		else if (state_code == 3)
			break;
	}
	delete client_socket;
	return true;
};

bool CheckForYes()
{
	char input[256];
	memset(input, 0, sizeof(input));
	std::cout << "����y��ȷ�������������" << std::endl;
	std::cin.getline(input, sizeof(input));

	if (strcmp(input, "y") == 0 || strcmp(input, "Y") == 0)
	{
		return true;
	}
	return false;
}

int main()
{
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	WSAStartup(wVersionRequested, &wsaData);

#ifdef USE_TEST
	Test();
#else
	char input[256];
	while (true)
	{
		memset(input, 0, sizeof(input));

		bool refresh = false;
		std::cout << "����exit�˳����䣬�������������ַ��������䣺\n";
		std::cin.getline(input, sizeof(input));
		if (strcmp(input, "exit") == 0)
		{
			break;
		}
		system("cls");
		yqs::MailClientSocket* client_socket = new yqs::MailClientSocket();

		unsigned short mail_server_type;
		do
		{
			std::cout << "��ѡ���������ͣ�\n1��QQ����\n2������163����\n";
			std::cin >> mail_server_type;
			CLEAR_BUFFER_BEFORE('\n');	//���������
		} while (mail_server_type < 1 || mail_server_type > 2);

		int state_code = 0;
		do
		{
			std::cout << "�Ƿ�����SSL��\n1������\n2��������\n";
			std::cin >> state_code;
			use_ssl = (state_code == 1 ? true : false);
			CLEAR_BUFFER_BEFORE('\n');
		} while (state_code != 1 && state_code != 2);

		switch (mail_server_type)
		{
		case 1:
			client_socket->SetService(yqs::QQMail, use_ssl);
			break;
		case 2:
			client_socket->SetService(yqs::NetEase163Mail, use_ssl);
			break;
		default: break;
		}

		state_code = 0;
		do
		{
			std::cout << "�������˺ţ�";
			std::cin.getline(user_name, sizeof(user_name));
			std::cout << "���������루��Ȩ�룩��";
			std::cin.getline(password, sizeof(password));
			if (client_socket->Login(user_name, password))	//��¼
			{
				break;
			}
			else
			{
				std::cout << "�����µ�¼" << std::endl;
				refresh = true;
				break;
			}
		} while (true);
		if (refresh == true)
		{
			delete client_socket;
			continue;
		}
		state_code = 0;

		while (true)
		{
			do
			{
				std::cout << "\n��ѡ���ܣ�\n1�������ʼ�\n2�������ʼ�\n3���˳�����\n";
				std::cin >> state_code;
				CLEAR_BUFFER_BEFORE('\n');
			} while (state_code < 1 || state_code > 3);
			if (state_code == 1)
			{
				while (true)
				{
					std::cout << "�������ռ��˵�ַ��" << std::endl;
					std::cin.getline(receiver, sizeof(receiver));
					std::cout << "����������ʼ���Ŀ��" << std::endl;
					std::cin.getline(mail_title, sizeof(mail_title));
					std::cout << "����������ǳƣ�" << std::endl;
					std::cin.getline(mailsender_name, sizeof(mailsender_name));
					if (CheckForYes())
					{
						client_socket->SetHead(receiver, mail_title, mailsender_name);
						break;
					}
				}
				while (true)
				{
					std::cout << "�������ʼ����ݣ�" << std::endl;
					std::cin.getline(mail_body, sizeof(mail_body));
					if (CheckForYes())
					{
						client_socket->SetContent(mail_body);
						break;
					}
				}
				while (true)
				{
					char file_path[512];
					std::cout << "�������ʼ��ʼ�������ַ���ʼ�����������base64���ܣ�������exit�����������룺" << std::endl;
					std::cin >> file_path;
					if (strcmp(file_path, "exit") == 0)
					{
						break;
					}
					if (CheckForYes())
					{
						client_socket->AddFile(file_path);
					}
				}
				if (client_socket->SendMail())
				{
				}
				client_socket->SendFinish();
			}
			else if (state_code == 2)
			{
				client_socket->ListAllMail();
			}
			else if (state_code == 3)
				break;
		}

		delete client_socket;
	};
	WSACleanup();
#endif

	return 0;
}