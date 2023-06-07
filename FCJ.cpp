#include"Define.h"
#include"Seek.h"
#include"IOput.h"
#include"Calculate.h"
#include"DataToA.h"
#include"Simulate.h"

int main() {
	IOPUT* io = new IOPUT();
	io->ReadFromTxt();
	/*Calcu* ca = new Calcu();
	ca->toQin();
	ca->toQave();
	ca->toQgen();
	delete ca;*/
	dataToA* da = new dataToA();
	da->ToA_Simple();
	/*da->AToQout();*/
	da->QoutToQin();
	da->toProfit();//计算效益

	io->OutQio();
	io->OutN();//计算拟合，输出出力，效益

	/*da->QinToQave();*/
	delete da;

	Simu* si = new Simu();
	si->simulate();
	delete si;

	io->OutputToTxt();
	delete io;
}