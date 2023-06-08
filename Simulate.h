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
		// 低谷期及第一个平峰 优先蓄水 到23才发电
		if (SimuR[i].t < 8 || (SimuR[i].t >= 12 && SimuR[i].t <= 16)) {
			flag = 1;
			// 如果水位高于23 发电
			if (sk.ZtoV(SimuR[i].Zup) + SimuR[i].Qin * 3600 > vflat) {
				// 直接让下一时段水位 = 23   多余水发电  
				SimuR[i + 1].Zup = Zflat;
				SimuR[i].Qout = SimuR[i].Qin - (vflat - sk.ZtoV(SimuR[i].Zup)) / 3600;
				SimuR[i].Qgen = SimuR[i].Qout;
				// 水头
				SimuR[i].Hd = (SimuR[i].Zup + SimuR[i + 1].Zup) / 2 - sk.QtoZ(SimuR[i].Qout);
				//水头控制
				if(SimuR[i].Hd >= 8){
					// 如果水头高于8m 正常发电
					SimuR[i].N = A * SimuR[i].Qgen * SimuR[i].Hd;
					// 出力大于Nmax则按Nmax出力  其余弃水
					if (SimuR[i].N > Nmax) {
						SimuR[i].N = Nmax;
						SimuR[i].Qgen = Nmax / A / SimuR[i].Hd;
					}
				}else{// 如果水头低于8m 停机  多余水弃掉
					SimuR[i].Qgen = 0;
					SimuR[i].N = 0;
				}
				
			}
			else {// 水位低于23  蓄水
				SimuR[i + 1].Zup = sk.VtoZ(sk.ZtoV(SimuR[i].Zup) + SimuR[i].Qin * 3600);
				SimuR[i].Qout = 0;
				SimuR[i].Qgen = 0;
				SimuR[i].N = 0;
			}
		}
		
		// 第二个平峰21-24  优先发电 来多少发多少
		else if (SimuR[i].t >= 21 && SimuR[i].t <= 23) {
			flag = 1;
			// 来多少发多少 水位不变
			SimuR[i + 1].Zup = SimuR[i].Zup;
			SimuR[i].Qout = SimuR[i].Qin;
			SimuR[i].Qgen = SimuR[i].Qout;
			// 水头
			SimuR[i].Hd = (SimuR[i].Zup + SimuR[i + 1].Zup) / 2 - sk.QtoZ(SimuR[i].Qout);
			//水头控制 高于8m 正常发电
			if(SimuR[i].Hd > 8) {
				//由发电流量确定出力
				SimuR[i].N = A * SimuR[i].Qgen * SimuR[i].Hd;
				//处理出力高于Nmax的情况  即来水太多   按最大出力 还需再蓄水或者弃水
				if(SimuR[i].N > Nmax) {
					//此时 发电流量、出库流量、水头都不确定 
					double h = 14;
					SimuR[i].Qout = SimuR[i].Qgen = Nmax / A / h;
					SimuR[i + 1].Zup = sk.VtoZ(sk.ZtoV(SimuR[i].Zup) + (SimuR[i].Qin - SimuR[i].Qout) * 3600);
					SimuR[i].Hd = (SimuR[i].Zup + SimuR[i + 1].Zup) / 2 - sk.QtoZ(SimuR[i].Qout);
					int loop = 0;
					while (abs(SimuR[i].Hd - h) > 0.1) {
						h = SimuR[i].Hd;
						SimuR[i].Qout = SimuR[i].Qgen = Nmax / A / h;
						SimuR[i + 1].Zup = sk.VtoZ(sk.ZtoV(SimuR[i].Zup) + (SimuR[i].Qin - SimuR[i].Qout) * 3600);
						SimuR[i].Hd = (SimuR[i].Zup + SimuR[i + 1].Zup) / 2 - sk.QtoZ(SimuR[i].Qout);
						loop++;
						if (loop > 5) {
							break;
						}
					}
					//假定 Qout为 Nmax对应流量   水量平衡，水位控制到23m 的多余水  可为负  
					double Q = (sk.ZtoV(SimuR[i].Zup) + SimuR[i].Qin * 3600 - sk.ZtoV(Zflat)) / 3600;
					// 多余水大于 Nmax 对应流量  则多余水为出库流量  下一时段水位到23  计算水头
					if(Q > SimuR[i].Qout) {
						SimuR[i].Qout = Q;
						SimuR[i + 1].Zup = Zflat;
						SimuR[i].Hd = (SimuR[i].Zup + SimuR[i + 1].Zup) / 2 - sk.QtoZ(SimuR[i].Qout);
					}

					
					SimuR[i].N = Nmax;

					//水头低于8 停机
					if(SimuR[i].Hd < 8 || SimuR[i].Qin > 12000) {
						SimuR[i].N = 0;
						SimuR[i].Qgen = 0;
						if (sk.ZtoV(SimuR[i].Zup) + SimuR[i].Qin * 3600 > vflat) {
							SimuR[i].Qout = SimuR[i].Qin - (vflat - sk.ZtoV(SimuR[i].Zup)) / 3600;
						}else {
							SimuR[i].Qout = 0;
						}
					}

					// 死水位控制
					if (SimuR[i + 1].Zup < Zmin) {
						SimuR[i].Qout = 0;
						SimuR[i].Qgen = 0;
						SimuR[i + 1].Zup = sk.VtoZ(sk.ZtoV(SimuR[i].Zup) + (SimuR[i].Qin - SimuR[i].Qout) * 3600);
						SimuR[i].N = 0;
					}
				}
			}

			// 水头低于 8m 停机 优先蓄水到23  多余弃水
			else
			{
				//来水多 要弃水
				if (sk.ZtoV(SimuR[i].Zup) + SimuR[i].Qin * 3600 > vflat) {
					// 水位控制到23
					SimuR[i + 1].Zup = Zflat;
					SimuR[i].Qout = SimuR[i].Qin - (vflat - sk.ZtoV(SimuR[i].Zup)) / 3600;
					// SimuR[i].Hd = (SimuR[i].Zup + SimuR[i + 1].Zup) / 2 - sk.QtoZ(SimuR[i].Qout);
					SimuR[i].N = 0;
					SimuR[i].Qgen = 0;
				} else {// 来水少 不弃水 全部蓄水
					SimuR[i + 1].Zup = sk.VtoZ(sk.ZtoV(SimuR[i].Zup) + SimuR[i].Qin * 3600);
					SimuR[i].Qout = 0;
					SimuR[i].Qgen = 0;
					SimuR[i].N = 0;
				}
			}
		}
		
		//高峰期 8-12 17-21
		else {
			//高于死水位
			if (flag) {
				double h = 14;
				SimuR[i].Qout = SimuR[i].Qgen = Nmax / A / h;
				SimuR[i + 1].Zup = sk.VtoZ(sk.ZtoV(SimuR[i].Zup) + (SimuR[i].Qin - SimuR[i].Qout) * 3600);
				SimuR[i].Hd = (SimuR[i].Zup + SimuR[i + 1].Zup) / 2 - sk.QtoZ(SimuR[i].Qout);
				int loop = 0;
				while (abs(SimuR[i].Hd - h) > 0.1) {
					h = SimuR[i].Hd;
					SimuR[i].Qout = SimuR[i].Qgen = Nmax / A / h;
					SimuR[i + 1].Zup = sk.VtoZ(sk.ZtoV(SimuR[i].Zup) + (SimuR[i].Qin - SimuR[i].Qout) * 3600);
					SimuR[i].Hd = (SimuR[i].Zup + SimuR[i + 1].Zup) / 2 - sk.QtoZ(SimuR[i].Qout);
					loop++;
					if (loop > 5) {
						break;
					}
				}

				double Q = (sk.ZtoV(SimuR[i].Zup) + SimuR[i].Qin * 3600 - sk.ZtoV(Zflat)) / 3600;
			
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
				
				// 死水位控制
				if (SimuR[i + 1].Zup < Zmin) {
					SimuR[i].Qout = 0;
					SimuR[i + 1].Zup = sk.VtoZ(sk.ZtoV(SimuR[i].Zup) + (SimuR[i].Qin - SimuR[i].Qout) * 3600);
					SimuR[i].N = 0;
				}
			}
			//低于死水位
			else {
				SimuR[i].Qout = 0;
				SimuR[i].Qgen = 0;
				SimuR[i + 1].Zup = sk.VtoZ(sk.ZtoV(SimuR[i].Zup) + (SimuR[i].Qin - SimuR[i].Qout) * 3600);
				SimuR[i].N = 0;
			}
			//验证整个高峰期末不会低于死水位 提前停止发电 保水位
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