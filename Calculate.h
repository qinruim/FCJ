#pragma once

class Calcu
{
public:
	Calcu();
	~Calcu();
	void toQin();
	void toQave();
	void toQgen();
private:

};

Calcu::Calcu()
{
}

Calcu::~Calcu()
{
}

void Calcu::toQin() {
	for (int i = 0; i < Intval; i++) {
		seek sk;
		resvr[i].Qin = (sk.ZtoV(resvr[i + 1].Zup) - sk.ZtoV(resvr[i].Zup)) / 3600 + resvr[i].Qout;
	}
}

void Calcu::toQave() {
	int n = 0;
	int ini = 0;
	double Qtotal = 0;
	for (int i = 0; i < Intval; i++) {
		if (n == 0) {
			ini = i;
		}
		n++;
		Qtotal += resvr[i].Qin;
		if (resvr[i].t == 23) {
			for (int j = 0; j < n; j++) {
				resvr[j + ini].Qin = Qtotal / n;
			}
			n = 0;
			Qtotal = 0;
		}
	}
	for (int i = 0; i < Intval; i++) {
		SimuR[i].Qin = resvr[i].Qin + battery[i];
	}
}

void Calcu::toQgen() {
	A = 0.0086;
	seek sk;
	for (int i = 0; i < Intval; i++) {
		double h = (resvr[i].Zup + resvr[i + 1].Zup) / 2 - sk.QtoZ(resvr[i].Qout);
		resvr[i].Qgen = resvr[i].N / A / h;
	}
}