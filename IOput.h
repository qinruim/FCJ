#pragma once
#include<iostream>
#include<fstream>
using namespace std;

class IOPUT {
public:
	IOPUT();
	~IOPUT();

	void ReadFromTxt();
	void OutQio();
	void OutN();
	void OutputToTxt();
};

IOPUT::IOPUT() {

}

IOPUT::~IOPUT() {

}

void IOPUT::ReadFromTxt() {
	ifstream infile(".\\IniData.txt");
	infile >> Zmin >> Zmax >> Zflat >> Intval >> Nmax;
	infile.close();

	infile.open(".\\PWCurve.txt");
	infile >> Nzv;
	for (int i = 0; i < Nzv; i++) {
		infile >> zv[i].Z >> zv[i].V;
		zv[i].V *= coef;
	}
	infile >> Nqz;
	for (int i = 0; i < Nqz; i++) {
		infile >> qz[i].Z >> qz[i].Q;
	}
	infile.close();

	infile.open(".\\OpProcess.txt");
	for (int i = 0; i < Intval; i++) {
		infile >> resvr[i].t >> resvr[i].Zup >>resvr[i].Zdn >> resvr[i].N >>resvr[i].Qout >> resvr[i].Qgen >> battery[i];
		SimuR[i].t = resvr[i].t;
	}
	infile >> resvr[Intval].Zup;
	infile.close();
	SimuR[0].Zup = resvr[0].Zup;
}

void IOPUT::OutQio() {
	ofstream outfile(".\\Qio.txt");
	for (int i = 0; i < Intval; i++) {
		outfile << resvr[i].Qout << "\t" << resvr[i].Qin << "\t" << resvr[i].Qgen << endl;
	}
	outfile.close();
}

void IOPUT::OutN() {
	ofstream outfile(".\\Nout.txt");
	seek sk;
	for (int i = 0; i < Intval; i++) {
		double n = A * resvr[i].Qgen * ((resvr[i].Zup + resvr[i + 1].Zup) / 2 - sk.QtoZ(resvr[i].Qout));
		if (n > 357.2) {
			n = 357.2;
		}
		outfile << n << "\t" << resvr[i].profit << endl;
	}
	outfile.close();
}

void IOPUT::OutputToTxt() {
	ofstream outfile(".\\OutProcess.txt");
	for (int i = 0; i < Intval; i++) {
		outfile << SimuR[i].Zup << "\t" << SimuR[i].N << "\t" << SimuR[i].Qout << "\t" << SimuR[i].profit << endl;
	}
}