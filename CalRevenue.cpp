//我自己能写出来的方法。
#include<iostream>
using namespace std;
typedef int Time;
typedef enume {
	Wage = 0;
	Bonus;
}RevenueType;
struct Revenue {
	Time end;
	RevenueType type;
	double sum;
};
struct ProgressiveRate {
	double threshold;
	double rate;
	int quick_extra;
}
static ProgressiveRate wage_tax_1[] = {
	{2000,0.03,100},
	{4500,0.10,150}
}
static ProgressiveRate wage_tax_2[] = {
	{3500,0.03,100},
	{5000,0.10,150}
}
static ProgressiveRate bonus_tax_1[] = {
	{18000,0.03,0},
	{54000,0.10,0}
}
static ProgressiveRate bonus_tax_1[] = {
	{20000,0.03,0},
	{60000,0.10,0}
}
//月薪扣税方式分为两种方式，外层是按年份分类，里层是按征税点分类。
//将数据固定在函数中比较傻，一旦数据变了，还要修改函数，甚至有多个函数引用此数据就要修改多个函数。
//所以最好创建数组，在函数中根据年份和起征点查找数据。
//在下述函数中，我用了if else语句，将所有情况统一到了一个函数中。
double cal_tax_wage(const Revenue *r) {//这里没必要传r了，因为在calc_income函数中已经将计算方式区分了，而且可以看到函数中只用了sum变量，所以这里直接串double类型的变量就好了
	ProgressiveRate tax_data;
	double tax_res;
	if(r.end <= 2011) {
		/***
		if(r.sum <= 2000) {
			tax_data.threshold = 2000;
			tax_data.rate = 0.03;
			tax_data.quick_extra = 100;
			***/
		find_tax_data(wage_tax_1, r->sum);
		}else if(r.sum <= 4500) {
			
		}else {
			...................
		}
	}else if(r.end <= 2018) {
		
	}else {
		.........
	}
	tax_res = (r->sum - tax_data.threshold) * tax_data.rate - tax_data.quick_extra;
	return r->sum - tax_res;
}
double cal_tax_bonus(const Revenue *r) {
	
}
double calc_income(const Revenue *r) {
	if(r->type == Wage) {
		return cal_tax_wage(r);
	}else if(r->type == Bonus) {
		return cal_tax_bonus(r);
	}
}
int main() {
	Revenue r1;
	double after_tax;
	r1.end = Time(2011);
	r1.type = Wage;
	r1.sum = 2000.24;
	after_tax = calc_income(&r1);
}
