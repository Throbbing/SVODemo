/**
*　　　　　　　　�芥掘　　？芥�+ +
*　　　　　　　�芥食潯ォォォ食潯� + +
*　　　　　　　�А　　　　　　？� 　
*　　　　　　　�А　　？ァ　　？� ++ + + +
*　　　　　　 ���������エ������� ��+
*　　　　　　　�А　　　　　　？� +
*　　　　　　　�А　　？漾　　？�
*　　　　　　　�А　　　　　　？� + +
*　　　　　　　�皐ォ掘　　？芥ォ�
*　　　　　　　　　�А　　？А　　　　　　　　　　�
*　　　　　　　　　�А　　？� + + + +
*　　　　　　　　　�А　　？А　　　�Code is far away from bug with the animal protecting　　　　　　　
*　　　　　　　　　�А　　？� + 　　　　舞舗隠嗷,旗鷹涙bug　　
*　　　　　　　　　�А　　？�
*　　　　　　　　　�А　　？А　�+　　　　　　　　　
*　　　　　　　　　�А� 　　�皐ォォォ� + +
*　　　　　　　　　�� 　　　　　　　�禰�
*　　　　　　　　　�� 　　　　　　　�芥�
*　　　　　　　　　�皐粂粂芥ォ廩粂芥� + + + +
*　　　　　　　　　　�З路蓮？З路�
*　　　　　　　　　　�皐潯拭�
*/
#pragma once
#include"JmxRConfig.h"

namespace jmxRCore
{
	class Demo
	{
	public:
		Demo(){}
		virtual ~Demo(){}

		virtual void init() = 0;
		virtual void update(f32 dt) = 0;
		virtual void run() = 0;
		virtual void calFPS(f32 dt) = 0;



	};
}

