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
	da->toProfit();//����Ч��

	io->OutQio();
	io->OutN();//������ϣ����������Ч��

	/*da->QinToQave();*/
	delete da;

	Simu* si = new Simu();
	si->simulate();
	delete si;

	io->OutputToTxt();
	delete io;
}