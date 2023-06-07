#pragma once
#include"Define.h"
class seek
{
public:
	seek();
	~seek();
	double ZtoV(double);
	double VtoZ(double);
	double QtoZ(double);
private:

};

seek::seek()
{
}

seek::~seek()
{
}


double seek::ZtoV(double Z) {
	if (Z <= zv[0].Z) {
		return zv[0].V;
	}
	else if (Z >= zv[Nzv - 1].Z) {
		return zv[Nzv - 1].V;
	}
	else {
		int indx = 0;
		for (int i = 0; i < Nzv - 1; i++) {
			if (Z >= zv[i].Z && Z < zv[i + 1].Z) {
				indx = i;
				break;
			}
		}
		return zv[indx].V + (zv[indx + 1].V - zv[indx].V) * (Z - zv[indx].Z) / (zv[indx + 1].Z - zv[indx].Z);
	}
}

double seek::VtoZ(double V) {
	if (V <= zv[0].V) {
		return zv[0].Z;
	}
	else if (V >= zv[Nzv - 1].V) {
		return zv[Nzv - 1].Z;
	}
	else {
		int indx = 0;
		for (int i = 0; i < Nzv - 1; i++) {
			if (V >= zv[i].V && V < zv[i + 1].V) {
				indx = i;
				break;
			}
		}
		return zv[indx].Z + (zv[indx + 1].Z - zv[indx].Z) * (V - zv[indx].V) / (zv[indx + 1].V - zv[indx].V);
	}
}

double seek::QtoZ(double Q) {
	if (Q <= qz[0].Q) {
		return qz[0].Z;
	}
	else if (Q >= qz[Nqz - 1].Q) {
		return qz[Nqz - 1].Z;
	}
	else {
		int indx = 0;
		for (int i = 0; i < Nqz - 1; i++) {
			if (Q >= qz[i].Q && Q < qz[i + 1].Z) {
				indx = i;
				break;
			}
		}
		return qz[indx].Z + (qz[indx + 1].Z - qz[indx].Z) * (Q - qz[indx].Q) / (qz[indx + 1].Q - qz[indx].Q);
	}
}