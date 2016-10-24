struct A {
	int a;
	float b;
};

// struct A print(struct A alpha){
// 	struct A beta;
// 	beta.a = 2;
// 	beta.b = 5;
// 	return beta;
// }

int factorial(int x){
	if(x==0){
		return 1;
	}
	else{
		return x*factorial(x-1);
	}
}


int f(int z,float x){
	return 49;
}

int main(){
	struct A alpha,beta,gamma;
	int c;
	float a,b;
	struct A *ptr;
	int *d;
	alpha.a=69;
	alpha.b=8.9;
	a=b=c=1.8;
	printf(c=f(1.4,0.0));
	printf(a,b,c,1);
	printf(factorial(5));
	1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;1;
	factorial(5);
	printf(factorial(10));
	beta=gamma=alpha;
	printf(beta.a, beta.b, gamma.a, gamma.b,ptr->a);
}
