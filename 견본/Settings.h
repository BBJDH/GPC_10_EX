#pragma once
#include"stdafx.h"

//Singletone Pattern
//�ϳ��� �ν��Ͻ��� �����ǵ��� �����ϰ�
//�������� ������ �� �ְ� �ϱ����� ����ϴ� ������ ����
class Settings final	//�̱���
{

public:
	static Settings& Get()  //�����ͷ� ���� ������ �������� �ִ�
							//���ο� ����ƽ���� ������⶧���� �� �ѹ��� ���������
							//���� ���������̱� ������ ��������ʴ´�
	{
		static Settings instance;
		return instance;
	}
	auto GetWindowHandle() const ->HWND {return handle;}
	void SetWindowHandle(HWND handle) {this->handle = handle;}

	auto GetWidth() const -> const float& {return width;}
	auto SetWidth(float const& width )  { this->width = width;}

	auto GetHeight() const ->  float const& {return height;}
	auto SetHeight(float const& height) { this->height = height; }


private:
	Settings() = default;
	~Settings() = default;

private:
	HWND handle  = nullptr;
	float width  = 0.0f;
	float height = 0.0f;

};