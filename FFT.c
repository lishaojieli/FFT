#include <stdio.h>
#include <math.h>
#include <stdlib.h>

/*定义复数结构体*/
typedef struct {
    double real;
    double img;
} complex;

complex x[16];  // 原始数据，上限为16个
complex *W;     // 欧拉转盘（旋转因子）
int size_x = 16; // 原始数据长度，为2的幂次
double PI = 3.141592653589793; // 圆周率

/*原始数据数组，该数据为对f(t)=cos（2*pi*3*t+pi/3）以16Hz频率在1s内采样所得*/
double arr[16] = {0.5000, -0.6088, -0.9659, -0.1305, 0.8660, 0.7934, -0.2588, -0.9914, 
                  -0.5000, 0.6088, 0.9659, 0.1305, -0.8660, -0.7934, 0.2588, 0.9914};

double amplitude[16] = {0};
double phase = 0;

/*
复数相加
*/
void complex_add(complex a, complex b, complex *c) {
    c->real = a.real + b.real;
    c->img = a.img + b.img;
}

/*
复数相减
*/
void complex_sub(complex a, complex b, complex *c) {
    c->real = a.real - b.real;
    c->img = a.img - b.img;
}

/*
复数相乘
*/
void complex_mul(complex a, complex b, complex *c) {
    c->real = a.real * b.real - a.img * b.img;
    c->img = a.real * b.img + a.img * b.real;
}

/*
欧拉转盘，取出一个圆上的size_x个点，作为每个数据对应的角度值
*/
void Euler_wheel(void) {
    W = (complex *)malloc(sizeof(complex) * size_x);
    for (int i = 0; i < size_x; i++) {
        W[i].real = cos(2 * PI * i / size_x);
        W[i].img = -sin(2 * PI * i / size_x);
    }
}

/*
位逆序，将原始数据按奇偶迭代排序
*/
void Even_odd(complex *x, int N) {
    int j, k;
    for (int i = 1, j = (N / 2); i < N - 1; ++i) {
        if (i < j) {
            complex temp = x[j];
            x[j] = x[i];
            x[i] = temp;
        }
        k = N / 2;
        while (k <= j) {
            j = j - k;
            k = k / 2;
        }
        j = j + k;
    }
}

/*
蝶形运算
*/
void FFT(void) {
    int i = 0, j = 0, k = 0, l = 0;
    complex up, down, product;
    for (int i = 0; i < size_x; i++) {
        x[i].real = arr[i];
        x[i].img = 0.0;
    }
    Even_odd(x, size_x);
    for (i = 0; i < log2(size_x); i++) {
        l = 1 << i;
        for (j = 0; j < size_x; j += 2 * l) {
            for (k = 0; k < l; k++) {
                complex_mul(x[j + k + l], W[size_x * k / (2 * l)], &product);
                complex_add(x[j + k], product, &up);
                complex_sub(x[j + k], product, &down);
                x[j + k] = up;
                x[j + k + l] = down;
            }
        }
    }
}

/*
幅值计算，算出每个复数值得幅值
*/
void amplitude_acculate(void)
{
    for(int i=0;i<16;i++)
    {
        amplitude[i] = sqrt(powl(x[i].img ,2) + powl(x[i].real ,2));
        amplitude[i] = amplitude[i] * 2 / size_x ;
        if(amplitude[i] < 0.0001)   //滤除低频噪声
        {
            amplitude[i] = 0.0;
        }
    }
}

/*
相位计算，算出原始信号复数值的相位
*/
void phase_acculate(void)
{
    for(int i=0;i< (size_x - 1) / 2;i++)
    {
        if(amplitude[i] != 0)
        {
            //printf("%f\n",amplitude[i]);
            phase = atan(x[i].img/x[i].real);
        }
    }
}

int main() {
    Euler_wheel();   //开启欧拉转盘
    FFT();           //fft对原始数据处理
    amplitude_acculate();  //计算每个复数幅值
    phase_acculate();      //计算原始数据相位
    
    // 将频谱数据保存到文件
    FILE *file = fopen("amplitude.txt", "w");
    if (file == NULL) {
        printf("无法打开文件\n");
        return 1;
    }
    for (int i = 0; i < size_x; i++) {
        fprintf(file, "%.4f\n",amplitude[i]);
    }

    //将相位数据保存到文件夹
    FILE *file1 = fopen("phase.txt", "w");
    if (file == NULL) {
        printf("无法打开文件\n");
        return 1;
    }
    fprintf(file1, "%.4f\n", phase);

    fclose(file);
    fclose(file1);
    free(W);
    return 0;
}