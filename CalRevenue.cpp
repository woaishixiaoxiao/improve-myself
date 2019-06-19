//我自己能写出来的方法。
#include<iostream>
#include<unordered_map>
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
};
static ProgressiveRate wage_tax_1[] = {
	{2000,0.03,100},
	{4500,0.10,150}
};
static ProgressiveRate wage_tax_2[] = {
	{3500,0.03,100},
	{5000,0.10,150}
};
static ProgressiveRate bonus_tax_1[] = {
	{18000,0.03,0},
	{54000,0.10,0}
};
static ProgressiveRate bonus_tax_1[] = {
	{20000,0.03,0},
	{60000,0.10,0}
};
//月薪扣税方式分为两种方式，外层是按年份分类，里层是按征税点分类。
//将数据固定在函数中比较傻，一旦数据变了，还要修改函数，甚至有多个函数引用此数据就要修改多个函数。
//所以最好创建数组，在函数中根据年份和起征点查找数据。
//在下述函数中，我用了if else语句，将所有情况统一到了一个函数中。
double cal_tax_wage(const Revenue *r) {
	ProgressiveRate tax_data;
	double tax_res;
	if(r->end <= 2011) {
		/***
		if(r.sum <= 2000) {
			tax_data.threshold = 2000;
			tax_data.rate = 0.03;
			tax_data.quick_extra = 100;
		}else if(r.sum <= 4500) {
			
		}else {
			...................
		}
		***/
		find_tax_data(wage_tax_1, r->sum, &tax_data);//这里wage_tax_1和2011是绑定起来的，这里不叫做耦合。
	}else if(r->end <= 2018) {
		
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

//通过函数指针的方式：将cal_tax_wage函数进行拆分。
//根据年份创建多个函数。
typedef double (*Calc)(double);
double calc_tax_wage1(double total) {//这里没必要传r了，因为已经将计算方式和年份分开了，所以这里直接用double类型的变量就好了
	ProgressiveRate tax_data;
	double tax_res;
	find_tax_data(wage_tax_1, total, &tax_data);//这里每个数组还是和具体的函数绑定
	tax_res = (total - tax_data.threshold) * tax_data.rate - tax_data.quick_extra;
	return tax_res;
}
double cal_tax2_wage2(double total){
}
double cal_tax_wage(double total, Calc fun) {
	return total - fun(total);
}

double cal_tax_bonus1(double total){
}
double cal_tax_bonus2(double total) {
}
double cal_tax_bonus(double total, Calc fun) {
	return total - fun(total);
}

double calc_income(const Revenue *r) {
	double after_tax_income;
	if(r->type == Wage) {
		if(r->end <= 2011) {
			after_tax_income = cal_tax_wage(r->sum, cal_tax_wage1);//这里如果到时候2011的计算税的方式变了，可以直接传递新的函数即可。
		}else if(r->end <= 2018) {
			after_tax_income = cal_tax_wage(r->sum, cal_tax_wage2);
		}else {
			.............
		}
	}else if(r->type == Bonus) {
		if(r->end <= 2011) {
			after_tax_income = cal_tax_bonus(r->sum, cal_tax_wage1);
		}else if(r->end <= 2018) {
			after_tax_income = cal_tax_bonus(r->sum, cal_tax_wage2);
		}else {
			.............
		}
	}
	return after_tax_income;
}

//上面的缺点是如果年份太多，那么就要编写很多形式相同的简单的函数。回调函数经常都有一个void *类型的扩展参数。用这个参数区分不同的年份，而不是直接编写函数。
struct Calc {
	double (*calc_tax)(double total, void *param);
	void *param;
};
double cal_tax_wage_common(double total, const void *param) {
	const ProgressiveRate *tax_data = (const ProgressiveRate *)param;
	double tax_res;
	tax_res = (total - tax_data->threshold) * tax_data->rate - tax_data->quick_extra;
	return tax_res;
}
double cal_tax_wage(double total, Calc *calc) {
	return total - calc->calc_tax(total, calc->param);
}

/*制程阶段*/
void process_wage(const Revenue *r, Calc *calc) {
	calc->calc_tax = cal_tax_wage_common;
	double total = r->sum;
	const ProgressiveRate *tax_data;
	if(r->end <= 2011) {
		find_tax_data(wage_tax_1, total, tax_data);
		calc->param = (const void *)tax_data;
	}else if(r->end <= 2018) {
		
	}else {
		.............
	}
}
Calc *process_bonus() {
	
}
/*生产阶段*/
double calc_income(const Revenue *r) {
	double after_tax_income;
	Calc *calc = (Calc *)malloc(sizeof(Calc));
	if(r->type == Wage) {
		process_wage(r, calc);
		after_tax_income = calc->calc_tax(r->sum, calc->param);
	}else if(r->type == Bonus) {
		process_bonus(r, calc);
		after_tax_income = calc->calc_tax(r->sum, calc->param);
	}
	free(calc);
	return after_tax_income;
}
int main() {
	Revenue r1;
	double after_tax;
	r1.end = Time(2011);
	r1.type = Wage;
	r1.sum = 2000.24;
	after_tax = calc_income(&r1);
}

//上面的方式区分bonus以及wage的方式是在代码中根据Revenue中的type类型使用if else进行判断。
//更灵活的是，使用注册机制，然后自动去查找。
unordered_map<Time,unordered_map<RevenueType,calc *> >calc_map;
int register_calc(Time end, RevenueType type, Calc *calc) {
	..........
}
double cal_tax_wage(double total, const void *param) {
	const ProgressiveRate *tax_data_array = (const ProgressiveRate *)param;
	const ProgressiveRate *match_tax = find_match_tax(tax_data_array, total);
	double tax_res;
	tax_res = (total - match_tax->threshold) * match_tax->rate - match_tax->quick_extra;//wage的计算方式
	return tax_res;
}
double cal_tax_bonus(double total, const void *param) {
	const ProgressiveRate *tax_data_array = (const ProgressiveRate *)param;
	const ProgressiveRate *match_tax = find_match_tax(tax_data_array, total);
	double tax_res;
	tax_res = (total - match_tax->threshold) * match_tax->rate - match_tax->quick_extra;//bonus的计算方式，这里两者相同。因为是举例子所以没有合到一起去。
	return tax_res;
}
const Calc* find_calc(Time end, RevenueType type) {
	return calc_map[end][type];
}
struct Register {
	int (*p_register_calc)(Time end, RevenueType type, Calc *calc);
	const Calc * (*p_find_calc)(Time end, RevenueType type);
};
void init(struct Register *p_reg) {
	Calc *calc1 = (Calc*)malloc(sizeof(Calc));
	calc->calc_tax = cal_tax_bonus;
	calc->param = (void *)wage_tax_1;
	p_reg->p_register_calc(2011, Wage, calc);
	.........................
}
double calc_income(const RevenueType *r1, struct Register *p_reg) {
	const Calc *calc = p_reg->find_calc(r1->end, r1->type);
	double tax = calc->cal_tax(r1->sum, calc->param);
	return r1->sum - tax;
}
int main() {
	Register reg(register_calc, find_calc);
	init(&reg);
	double after_tax;
	Revenue r1(2011, Wage, 2000.24);
	after_tax = calc_income(r1, &reg);
}
/*基类*/
class Calc {
	public :
		virtual double calc_tax(double total, const void *param) = 0;//param中存放静态数组然后在函数中。
};
class Wage: public Calc {
	public :
		double calc_tax(double total, const void *param); //param中存放静态数组然后在函数中。
		
};
class Bonus: public Calc {
	public :
		double calc_tax(double total, const void *param);
};
double Wage::cal_tax(double total, const void *param) {
	const ProgressiveRate *tax_data_array = (const ProgressiveRate *)param;
	const ProgressiveRate *match_tax = find_match_tax(tax_data_array, total);
	double tax_res;
	tax_res = (total - match_tax->threshold) * match_tax->rate - match_tax->quick_extra;//wage的计算方式
	return tax_res;
}
double cal_tax(double total, void *param) {
	const ProgressiveRate *tax_data_array = (const ProgressiveRate *)param;
	const ProgressiveRate *match_tax = find_match_tax(tax_data_array, total);
	double tax_res;
	tax_res = (total - match_tax->threshold) * match_tax->rate - match_tax->quick_extra;//bonus的计算方式，这里两者相同。因为是举例子所以没有合到一起去。
	return tax_res;
}	



