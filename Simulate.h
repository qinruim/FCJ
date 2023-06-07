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
		// 非高峰发电阶段
		if (SimuR[i].t < 8 || (SimuR[i].t >= 12 && SimuR[i].t <= 16)) {
			flag = 1;
			// 判断末水位是否大于平水位
			if (sk.ZtoV(SimuR[i].Zup) + SimuR[i].Qin * 3600 > vflat) {
				// 大于平水位，则时段末水位为平水位，其他水用于发电
				SimuR[i + 1].Zup = Zflat;
				SimuR[i].Qout = SimuR[i].Qin - (vflat - sk.ZtoV(SimuR[i].Zup)) / 3600;
				SimuR[i].Qgen = SimuR[i].Qout;
				// 计算水头
				SimuR[i].Hd = (SimuR[i].Zup + SimuR[i + 1].Zup) / 2 - sk.QtoZ(SimuR[i].Qout);

				if(SimuR[i].Hd >= 8){
					// 计算出力，并判断是否大于机组最大出力
					SimuR[i].N = A * SimuR[i].Qgen * SimuR[i].Hd;
					// 大于最大出力，机组发电为最大，并根据最大处理计算发电流量
					if (SimuR[i].N > Nmax) {
						SimuR[i].N = Nmax;
						SimuR[i].Qgen = Nmax / A / SimuR[i].Hd;
					}
				}else{// 水头小于8m，则机组停机，多余的水全部当作弃水
					SimuR[i].Qgen = 0;
					SimuR[i].N = 0;
				}
				
			}
			else {// 小于平水位，蓄水
				SimuR[i + 1].Zup = sk.VtoZ(sk.ZtoV(SimuR[i].Zup) + SimuR[i].Qin * 3600);
				SimuR[i].Qout = 0;
				SimuR[i].N = 0;
			}
		}
		
		// 第二个平峰
		else if (SimuR[i].t >= 21 && SimuR[i].t <= 23) {
			flag = 1;
			// 来多少发多少
			SimuR[i + 1].Zup = SimuR[i].Zup;
			SimuR[i].Qout = SimuR[i].Qin;
			SimuR[i].Qgen = SimuR[i].Qout;
			// 计算水头
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
						// 将水位蓄到平水位，还多的流量
						SimuR[i + 1].Zup = sk.VtoZ(sk.ZtoV(SimuR[i].Zup) + (SimuR[i].Qin - SimuR[i].Qout) * 3600);
						SimuR[i].Hd = (SimuR[i].Zup + SimuR[i + 1].Zup) / 2 - sk.QtoZ(SimuR[i].Qout);
						loop++;
						if (loop > 5) {
							break;
						}
					}

					double Q = (sk.ZtoV(SimuR[i].Zup) + SimuR[i].Qin * 3600 - sk.ZtoV(Zflat)) / 3600;
					// 多的流量比实际发电流量大，则还有弃水
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

					// 时段末死水位控制
					if (SimuR[i + 1].Zup < Zmin) {
						SimuR[i].Qout = 0;
						SimuR[i + 1].Zup = sk.VtoZ(sk.ZtoV(SimuR[i].Zup) + (SimuR[i].Qin - SimuR[i].Qout) * 3600);
						SimuR[i].N = 0;
					}
				}
			}else{// 水头低于8m
				if (sk.ZtoV(SimuR[i].Zup) + SimuR[i].Qin * 3600 > vflat) {
					// 大于平水位
					SimuR[i + 1].Zup = Zflat;
					SimuR[i].Qout = SimuR[i].Qin - (vflat - sk.ZtoV(SimuR[i].Zup)) / 3600;
					// SimuR[i].Hd = (SimuR[i].Zup + SimuR[i + 1].Zup) / 2 - sk.QtoZ(SimuR[i].Qout);
					SimuR[i].N = 0;
					SimuR[i].Qgen = 0;
				} else {// 小于平水位，蓄水
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
					// 将水位蓄到平水位，还多的流量
					SimuR[i + 1].Zup = sk.VtoZ(sk.ZtoV(SimuR[i].Zup) + (SimuR[i].Qin - SimuR[i].Qout) * 3600);
					SimuR[i].Hd = (SimuR[i].Zup + SimuR[i + 1].Zup) / 2 - sk.QtoZ(SimuR[i].Qout);
					loop++;
					if (loop > 5) {
						break;
					}
				}

				double Q = (sk.ZtoV(SimuR[i].Zup) + SimuR[i].Qin * 3600 - sk.ZtoV(Zflat)) / 3600;
				// 多的流量比实际发电流量大，则还有弃水
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
				
				// 时段末死水位控制
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

		if (SimuR[i].t >= 0 && SimuR[i].t <= 7) {//谷电（0.14）
			SimuR[i].profit = 1000 * 0.14 * SimuR[i].N;
		}
		else if (SimuR[i].t >= 8 && SimuR[i].t <= 10) {//腰电（0.32）
			SimuR[i].profit = 1000 * 0.32 * SimuR[i].N;
		}
		else if (SimuR[i].t >= 17 && SimuR[i].t <= 21) {//腰电（0.32）
			SimuR[i].profit = 1000 * 0.32 * SimuR[i].N;
		}
		else {//峰电（0.54）
			SimuR[i].profit = 1000 * 0.54 * SimuR[i].N;
		}
	}
}