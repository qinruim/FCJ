#pragma once

double coef = 100000000;

typedef struct {
	int t;//Сʱ���
	double Zup;//����ˮλ
	double Zdn;//βˮλ
	double Qout;//��������
	double Qgen;//��ˮ
	double Qin;//�������
	double Hd;//ˮͷ
	double N;//����
	double profit;//����
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
double Zmin;//��Сˮλ
double Zmax;//���ˮλ
double Zflat;//������ˮλ
double Nmax;//װ������
int Intval;//����ʱ����

int Nzv;
ZV zv[30];
int Nqz;
QZ qz[30];
Resvr resvr[9000];
Resvr SimuR[9000];
double battery[9000];