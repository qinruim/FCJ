#pragma once

double coef = 100000000;

typedef struct {
	int t;//小时编号
	double Zup;//坝上水位
	double Zdn;//尾水位
	double Qout;//出库流量
	double Qgen;//弃水
	double Qin;//入库流量
	double Hd;//水头
	double N;//出力
	double profit;//收益
}Resvr;

typedef struct {
	double Z;
	double V;
}ZV;

typedef struct {
	double Q;
	double Z;
}QZ;

double A;//N=AQH
double Zmin;//最小水位
double Zmax;//最大水位
double Zflat;//正常蓄水位
double Nmax;//装机容量
int Intval;//年总时段数

int Nzv;
ZV zv[30];
int Nqz;
QZ qz[30];
Resvr resvr[9000];
Resvr SimuR[9000];
double battery[9000];