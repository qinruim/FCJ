#pragma once

class dataToA
{
public:
	dataToA();
	~dataToA();
	void ToA_Simple();
	void toProfit();
	void AToQout();
	void QoutToQin();
	void QinToQave();
private:

};

dataToA::dataToA()
{
}

dataToA::~dataToA()
{
}

//简单计算A
void dataToA::ToA_Simple() {
	double Ntotal = 0;
	double Qtotal = 0;
	double Htotal = 0;
	int nh = 0;
	for (int i = 0; i < Intval; i++) {
		Ntotal += resvr[i].N;
		Qtotal += resvr[i].Qgen;
		resvr[i].Hd = (resvr[i].Zup + resvr[i + 1].Zup) / 2 - resvr[i].Zdn;
		if (resvr[i].N > 1) {
			/*seek sk;
			resvr[i].Hd = (resvr[i].Zup + resvr[i + 1].Zup) / 2 - sk.QtoZ(resvr[i].Qout);*/
			Htotal += resvr[i].Hd;
			nh++;
		}
	}
	Htotal /= nh;
	A = Ntotal / Qtotal / Htotal;
}

void dataToA::toProfit() {
	for (int i = 0; i < Intval; i++) {
		if (resvr[i].t >= 0 && resvr[i].t <= 7) {//谷电（0.14）
			resvr[i].profit = 1000*0.14 * resvr[i].N;
		}
		else if (resvr[i].t >= 8 && resvr[i].t <= 10) {//腰电（0.32）
			resvr[i].profit = 1000 * 0.32 * resvr[i].N;
		}
		else if (resvr[i].t >= 17 && resvr[i].t <= 21) {//腰电（0.32）
			resvr[i].profit = 1000 * 0.32 * resvr[i].N;
		}
		else {//峰电（0.54）
			resvr[i].profit = 1000 * 0.54 * resvr[i].N;
		}
	}
}

void dataToA::AToQout() {
	for (int i = 0; i < Intval; i++) {
		seek sk;
		if (resvr[i].N != 0) {
			double h = 0;
			int loop = 0;
			while (abs(h - resvr[i].Hd) > 0.1) {
				if (h > 0) {
					resvr[i].Hd = h;
				}
				resvr[i].Qout = resvr[i].N / A / resvr[i].Hd;
				h = (resvr[i].Zup + resvr[i + 1].Zup) / 2 - sk.QtoZ(resvr[i].Qout);
				loop++;
				if (loop > 5) {
					break;
				}
			}
			resvr[i].Qout = resvr[i].N / A / resvr[i].Hd;
		}
		else
		{
			resvr[i].Qout = 0;
		}
	}
}

void dataToA::QoutToQin() {
	for (int i = 0; i < Intval; i++) {
		seek sk;
		// SimuR[i].Qin = resvr[i].Qin = (sk.ZtoV(resvr[i + 1].Zup) - sk.ZtoV(resvr[i].Zup)) / 3600 + resvr[i].Qout;
		resvr[i].Qin = (sk.ZtoV(resvr[i + 1].Zup) - sk.ZtoV(resvr[i].Zup)) / 3600 + resvr[i].Qout;
		SimuR[i].Qin = resvr[i].Qin + battery[i];
	}
}

void dataToA::QinToQave() {
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