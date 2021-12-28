#pragma once
#include"stdafx.h"

//Singletone Pattern
//하나의 인스턴스만 생성되도록 보장하고
//전역에서 접근할 수 있게 하기위해 사용하는 디자인 패턴
class Settings final	//싱글톤
{

public:
	static Settings& Get()  //포인터로 만들어서 가지고 있을수도 있다
							//내부에 스테틱으로 만들었기때문에 단 한번만 만들어진다
							//정적 지역변수이기 때문에 사라지지않는다
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