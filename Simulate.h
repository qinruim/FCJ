#pragma once

class Simu
{
public:
	Simu();
	~Simu();
	void simulate();
private:

};

Simu::Simu()
{
}

Simu::~Simu()
{
}

void Simu::simulate() {
	seek sk;
	double vmin = sk.ZtoV(Zmin);
	double vflat = sk.ZtoV(Zflat);
	double vmax = sk.ZtoV(Zmax);
	int flag = 1;
	for (int i = 0; i < Intval; i++) {
		// �Ǹ߷巢��׶�
		if (SimuR[i].t < 8 || (SimuR[i].t >= 12 && SimuR[i].t <= 16)) {
			flag = 1;
			// �ж�ĩˮλ�Ƿ����ƽˮλ
			if (sk.ZtoV(SimuR[i].Zup) + SimuR[i].Qin * 3600 > vflat) {
				// ����ƽˮλ����ʱ��ĩˮλΪƽˮλ������ˮ���ڷ���
				SimuR[i + 1].Zup = Zflat;
				SimuR[i].Qout = SimuR[i].Qin - (vflat - sk.ZtoV(SimuR[i].Zup)) / 3600;
				SimuR[i].Qgen = SimuR[i].Qout;
				// ����ˮͷ
				SimuR[i].Hd = (SimuR[i].Zup + SimuR[i + 1].Zup) / 2 - sk.QtoZ(SimuR[i].Qout);

				if(SimuR[i].Hd >= 8){
					// ������������ж��Ƿ���ڻ���������
					SimuR[i].N = A * SimuR[i].Qgen * SimuR[i].Hd;
					// ���������������鷢��Ϊ��󣬲�������������㷢������
					if (SimuR[i].N > Nmax) {
						SimuR[i].N = Nmax;
						SimuR[i].Qgen = Nmax / A / SimuR[i].Hd;
					}
				}else{// ˮͷС��8m�������ͣ���������ˮȫ��������ˮ
					SimuR[i].Qgen = 0;
					SimuR[i].N = 0;
				}
				
			}
			else {// С��ƽˮλ����ˮ
				SimuR[i + 1].Zup = sk.VtoZ(sk.ZtoV(SimuR[i].Zup) + SimuR[i].Qin * 3600);
				SimuR[i].Qout = 0;
				SimuR[i].N = 0;
			}
		}
		
		// �ڶ���ƽ��
		else if (SimuR[i].t >= 21 && SimuR[i].t <= 23) {
			flag = 1;
			// �����ٷ�����
			SimuR[i + 1].Zup = SimuR[i].Zup;
			SimuR[i].Qout = SimuR[i].Qin;
			SimuR[i].Qgen = SimuR[i].Qout;
			// ����ˮͷ
			SimuR[i].Hd = (SimuR[i].Zup + SimuR[i + 1].Zup) / 2 - sk.QtoZ(SimuR[i].Qout);
			if(SimuR[i].Hd > 8) {
				SimuR[i].N = A * SimuR[i].Qgen * SimuR[i].Hd;
				if(SimuR[i].N > Nmax) {
					double h = 14;
					SimuR[i].Qout = SimuR[i].Qgen = Nmax / A / h;
					SimuR[i + 1].Zup = sk.VtoZ(sk.ZtoV(SimuR[i].Zup) + (SimuR[i].Qin - SimuR[i].Qout) * 3600);
					SimuR[i].Hd = (SimuR[i].Zup + SimuR[i + 1].Zup) / 2 - sk.QtoZ(SimuR[i].Qout);
					int loop = 0;
					while (abs(SimuR[i].Hd - h) > 0.1) {
						h = SimuR[i].Hd;
						SimuR[i].Qout = SimuR[i].Qgen = Nmax / A / h;
						// ��ˮλ�ƽˮλ�����������
						SimuR[i + 1].Zup = sk.VtoZ(sk.ZtoV(SimuR[i].Zup) + (SimuR[i].Qin - SimuR[i].Qout) * 3600);
						SimuR[i].Hd = (SimuR[i].Zup + SimuR[i + 1].Zup) / 2 - sk.QtoZ(SimuR[i].Qout);
						loop++;
						if (loop > 5) {
							break;
						}
					}

					double Q = (sk.ZtoV(SimuR[i].Zup) + SimuR[i].Qin * 3600 - sk.ZtoV(Zflat)) / 3600;
					// ���������ʵ�ʷ���������������ˮ
					if(Q > SimuR[i].Qout) {
						SimuR[i].Qout = Q;
						SimuR[i + 1].Zup = Zflat;
						SimuR[i].Hd = (SimuR[i].Zup + SimuR[i + 1].Zup) / 2 - sk.QtoZ(SimuR[i].Qout);
					}

					SimuR[i].N = Nmax;

					if(SimuR[i].Hd < 8 || SimuR[i].Qin > 12000) {
						SimuR[i].N = 0;
						SimuR[i].Qgen = 0;
						if (sk.ZtoV(SimuR[i].Zup) + SimuR[i].Qin * 3600 > vflat) {
							SimuR[i].Qout = SimuR[i].Qin - (vflat - sk.ZtoV(SimuR[i].Zup)) / 3600;
						}else {
							SimuR[i].Qout = 0;
						}
					}

					// ʱ��ĩ��ˮλ����
					if (SimuR[i + 1].Zup < Zmin) {
						SimuR[i].Qout = 0;
						SimuR[i + 1].Zup = sk.VtoZ(sk.ZtoV(SimuR[i].Zup) + (SimuR[i].Qin - SimuR[i].Qout) * 3600);
						SimuR[i].N = 0;
					}
				}
			}else{// ˮͷ����8m
				if (sk.ZtoV(SimuR[i].Zup) + SimuR[i].Qin * 3600 > vflat) {
					// ����ƽˮλ
					SimuR[i + 1].Zup = Zflat;
					SimuR[i].Qout = SimuR[i].Qin - (vflat - sk.ZtoV(SimuR[i].Zup)) / 3600;
					// SimuR[i].Hd = (SimuR[i].Zup + SimuR[i + 1].Zup) / 2 - sk.QtoZ(SimuR[i].Qout);
					SimuR[i].N = 0;
					SimuR[i].Qgen = 0;
				} else {// С��ƽˮλ����ˮ
					SimuR[i + 1].Zup = sk.VtoZ(sk.ZtoV(SimuR[i].Zup) + SimuR[i].Qin * 3600);
					SimuR[i].Qout = 0;
					SimuR[i].Qgen = 0;
					SimuR[i].N = 0;
				}
			}
		}
		
		else {
			if (flag) {
				double h = 14;
				SimuR[i].Qout = SimuR[i].Qgen = Nmax / A / h;
				SimuR[i + 1].Zup = sk.VtoZ(sk.ZtoV(SimuR[i].Zup) + (SimuR[i].Qin - SimuR[i].Qout) * 3600);
				SimuR[i].Hd = (SimuR[i].Zup + SimuR[i + 1].Zup) / 2 - sk.QtoZ(SimuR[i].Qout);
				int loop = 0;
				while (abs(SimuR[i].Hd - h) > 0.1) {
					h = SimuR[i].Hd;
					SimuR[i].Qout = SimuR[i].Qgen = Nmax / A / h;
					// ��ˮλ�ƽˮλ�����������
					SimuR[i + 1].Zup = sk.VtoZ(sk.ZtoV(SimuR[i].Zup) + (SimuR[i].Qin - SimuR[i].Qout) * 3600);
					SimuR[i].Hd = (SimuR[i].Zup + SimuR[i + 1].Zup) / 2 - sk.QtoZ(SimuR[i].Qout);
					loop++;
					if (loop > 5) {
						break;
					}
				}

				double Q = (sk.ZtoV(SimuR[i].Zup) + SimuR[i].Qin * 3600 - sk.ZtoV(Zflat)) / 3600;
				// ���������ʵ�ʷ���������������ˮ
				if(Q > SimuR[i].Qout) {
					SimuR[i].Qout = Q;
					SimuR[i + 1].Zup = Zflat;
					SimuR[i].Hd = (SimuR[i].Zup + SimuR[i + 1].Zup) / 2 - sk.QtoZ(SimuR[i].Qout);
				}

				SimuR[i].N = Nmax;
				

				if(SimuR[i].Hd < 8 || SimuR[i].Qin > 12000) {
					SimuR[i].N = 0;
					SimuR[i].Qgen = 0;
					if (sk.ZtoV(SimuR[i].Zup) + SimuR[i].Qin * 3600 > vflat) {
						SimuR[i].Qout = SimuR[i].Qin - (vflat - sk.ZtoV(SimuR[i].Zup)) / 3600;
					}else {
						SimuR[i].Qout = 0;
					}
				}
				
				// ʱ��ĩ��ˮλ����
				if (SimuR[i + 1].Zup < Zmin) {
					SimuR[i].Qout = 0;
					SimuR[i + 1].Zup = sk.VtoZ(sk.ZtoV(SimuR[i].Zup) + (SimuR[i].Qin - SimuR[i].Qout) * 3600);
					SimuR[i].N = 0;
				}
			}
			else {
				SimuR[i].Qout = 0;
				SimuR[i + 1].Zup = sk.VtoZ(sk.ZtoV(SimuR[i].Zup) + (SimuR[i].Qin - SimuR[i].Qout) * 3600);
				SimuR[i].N = 0;
			}
			double v = 0;
			int timeFlag = (SimuR[i].t < 12) ? 12 : 21;
			for (int j = 0; j < timeFlag - SimuR[i].t; j++) {
				v += SimuR[i + j].Qin * 3600;
			}
			if (v + sk.ZtoV(SimuR[i].Zup) < vmin) {
				flag = 0;
			}
		}

		// if (SimuR[i].t >= 0 && SimuR[i].t <= 7) {//�ȵ磨0.14��
		// 	SimuR[i].profit = 1000 * 0.14 * SimuR[i].N;
		// }
		// else if (SimuR[i].t >= 8 && SimuR[i].t <= 10) {//���磨0.32��
		// 	SimuR[i].profit = 1000 * 0.32 * SimuR[i].N;
		// }
		// else if (SimuR[i].t >= 17 && SimuR[i].t <= 21) {//���磨0.32��
		// 	SimuR[i].profit = 1000 * 0.32 * SimuR[i].N;
		// }
		// else {//��磨0.54��
		// 	SimuR[i].profit = 1000 * 0.54 * SimuR[i].N;
		// }

		if (SimuR[i].t >= 0 && SimuR[i].t <= 7) {//低谷0.14
			SimuR[i].profit = 1000 * 0.14 * SimuR[i].N;
		}
		else if ((SimuR[i].t >= 8 && SimuR[i].t <= 11) || (SimuR[i].t >= 17 && SimuR[i].t <= 20)) {//高峰0.54
			SimuR[i].profit = 1000 * 0.54 * SimuR[i].N;
		}
		else {//其他平峰0.32
			SimuR[i].profit = 1000 * 0.32 * SimuR[i].N;
		}
	}
}