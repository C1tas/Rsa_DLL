// Rsa_DLL.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"


#include <stdio.h>
#include <stdlib.h>
#include <time.h> 
#include <string.h>

#define MAXLENGTH 100
#define DEBUG 1

char result_buffer[20480];
char thousand_prime_result[20480];
char miller_rabin_result[20480];


//extern "C" _declspec(dllexport) int add(int x, int y);    // ����ΪC���롢���ӷ�ʽ���ⲿ����
//extern "C" _declspec(dllexport) int sub(int x, int y);    // ����ΪC���롢���ӷ�ʽ���ⲿ����


extern "C" _declspec(dllexport) long long GCD(long long a, long long b);
extern "C" _declspec(dllexport) int check_prime_in_thounsand_prime(long long num);
extern "C" _declspec(dllexport) int check_miller_rabin(long long num);
extern "C" _declspec(dllexport) int probale_prime(long long num);
extern "C" _declspec(dllexport) long long random_prime(long long min_border, long long max_border);
extern "C" _declspec(dllexport) long long ext_gcd(long long e, long long modulus);
extern "C" _declspec(dllexport) long long encode(long long m, long long e, long long modulus);
extern "C" _declspec(dllexport) long long decode(long long m, long long e, long long modulus);
extern "C" _declspec(dllexport) int return_string(char tmp_str[20480], char tmp2_str[20480], char tmp3_str[20480]);


int thousand_prime[] = { 2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,103,107,
109,113,127,131,137,139,149,151,157,163,167,173,179,181,191,193,197,199,211,223,
227,229,233,239,241,251,257,263,269,271,277,281,283,293,307,311,313,317,331,337,
347,349,353,359,367,373,379,383,389,397,401,409,419,421,431,433,439,443,449,457,
461,463,467,479,487,491,499,503,509,521,523,541,547,557,563,569,571,577,587,593,
599,601,607,613,617,619,631,641,643,647,653,659,661,673,677,683,691,701,709,719,
727,733,739,743,751,757,761,769,773,787,797,809,811,821,823,827,829,839,853,857,
859,863,877,881,883,887,907,911,919,929,937,941,947,953,967,971,977,983,991,997 };

struct my_binary {
	int binary[MAXLENGTH];
	int length;
};

long long GCD(long long a, long long b) {

	long long temp;
	if (a < b) {
		temp = a;
		a = b;
		b = temp;
	}
	while (b != 0) {
		temp = b;
		b = a % b;
		a = temp;
	}
	return a;
}


struct my_binary trans_to_binary(long long num) {
	int i = 0;
	struct my_binary tmp_struct;

	while (num) {
		tmp_struct.binary[i] = num % 2;
		num /= 2;
		i++;
	}
	tmp_struct.length = i - 1;
	return tmp_struct;
}

int check_prime_in_thounsand_prime(long long num) {
	char tmp_buffer[1024];
	if (DEBUG) {
		sprintf(tmp_buffer, "Doing checking in thounsand prime...\n");
		strcat(result_buffer, tmp_buffer);
		strcat(thousand_prime_result, tmp_buffer);
	}
	int i = 0;

	for (i; i < 168; i++) {
		long long remainder = num % thousand_prime[i];
		if (num == thousand_prime[i]) {
			return 1;
		}
		else if (remainder == 0) {
			if (DEBUG) {
				sprintf(tmp_buffer, "%lld mod %d, and remainder is %lld.\n", num, thousand_prime[i], remainder);
				strcat(result_buffer, tmp_buffer);
				strcat(thousand_prime_result, tmp_buffer);
				sprintf(tmp_buffer, "Thounsand checking faild, the num is a composite.\n\n");
				strcat(result_buffer, tmp_buffer);
				strcat(thousand_prime_result, tmp_buffer);
			}
			return 0;
		}
		else {
			if (DEBUG) {
				sprintf(tmp_buffer, "%lld mod %d, and remainder is %lld.\n", num, thousand_prime[i], remainder);
				strcat(result_buffer, tmp_buffer);
				strcat(thousand_prime_result, tmp_buffer);
			}
			continue;
		}
	}
	return 1;
}

//ģ�ظ�ƽ��
long long repeat_calculation_method_square(long long x, long long n, long long m) {
	long long a = 1, b = x; //i=0��ʱ��b = x^(2^0) = x
	while (n)
	{
		if (n % 2 == 1)
			a = a * b % m;
		b = b * b % m;
		n /= 2;
	}
	return a;
}

//ƽ���˷���
long long multiply_calculation_method_square(long long x, long long n, long long m) {
	struct my_binary tmp_binary;
	long long remainder = 1;
	tmp_binary = trans_to_binary(n);
	int i = tmp_binary.length;
	for (i; i >= 0; i--) {
		if (tmp_binary.binary[i]) {
			remainder = remainder*remainder % m;
			remainder = remainder * x % m;
		}
		else {
			remainder = remainder*remainder % m;
		}
	}
	return remainder;

}


/*
���㷨�����ж�һ������ 3 ������ n �Ƿ����������� k ���ھ��� n �������ĸ��ʡ�
���㷨�ܹ��϶����ж� n �Ǻ���������ֻ��˵ n ������������
�� 01 �У��� n �C 1 �ֽ�Ϊ 2s��d  ����ʽ������ d ��������
�� 02 �У������²���(�� 03 �� 10 ��)ѭ�� k �Ρ�
�� 03 �У����� [2, n - 2] �ķ�Χ�ж����������ѡ��һ�������� a ��
�� 04 �У����������еĵ�һ��ֵ��x �� ad mod n ��
�� 05 �У�����������еĵ�һ������ 1 ���� n - 1����������������n ������������ת���� 03 �н���һ�´�ѭ����
�� 06 �У���ѭ��ִ�е� 07 �� 09 �У�˳�����������ʣ�µ� s �C 1 ��ֵ��
�� 07 �У�����������е���һ��ֵ��x �� x2 mod n ��
�� 08 �У����������ֵ�� 1 ������ǰ�ߵ�ֵ���� n - 1��������������������� n �϶��Ǻ������㷨������
�� 09 �У����������ֵ�� n - 1�������һ��ֵһ���� 1����������������n ������������ת���� 03 �н�����һ��ѭ����
�� 10 �У����ָ����в����� 1 ������������������������� n �϶��Ǻ������㷨������
�� 11 �У��Ѿ��� k �������������ѡ��� a ֵ�����˼��飬����ж� n �ǳ��п������������㷨������
��һ�μ����У����㷨����Ŀ��ܶ������ķ�֮һ��������Ƕ����غ������ѡ�� a �����ظ����飬һ�����㷨���� n �Ǻ�����
���ǾͿ���ȷ�� n �϶�������������������㷨�ظ����� 25 �α��涼����˵ n ������������
�����ǿ���˵ n �������϶�����������
��Ϊ����һ�� 25 �εļ�����̸���������������Ĵ�����Ϣ�ĸ���С�� (1/4)25��
���ֻ���С�� 1015 ��֮һ��
*/
int check_miller_rabin(long long num) {
	char tmp_buffer[1024];
	sprintf(tmp_buffer, "Doing Miller_Rabin checking...\nChecking Num is %lld\n", num);
	strcat(result_buffer, tmp_buffer);
	strcat(miller_rabin_result, tmp_buffer);
	long long d, s, t;
	d = num - 1;
	s = 0;
	t = 5;
	while (d % 2 == 0) {
		d /= 2;
		s += 1;
	}
	int k = 0;

	while (k < t) {
		srand((unsigned)time(NULL));
		sprintf(tmp_buffer, "This the %d time.\n", k);
		strcat(result_buffer, tmp_buffer);
		strcat(miller_rabin_result, tmp_buffer);
		long long a = rand() % (num - 1 - 2 + 1) + 2;
		if (DEBUG) {
			sprintf(tmp_buffer, "The random num a is %lld, the factor d is %lld, the factor s is %lld.\n", a, d, s);
			strcat(result_buffer, tmp_buffer);
			strcat(miller_rabin_result , tmp_buffer);
		}

		k += 1;
		if (GCD(a, num) != 1) {
			continue;
		}
		long long x = multiply_calculation_method_square(a, d, num);
		if (DEBUG) {
			sprintf(tmp_buffer, "The result of a**d mod N is %lld.\n", x);
			strcat(result_buffer, tmp_buffer);
			strcat(miller_rabin_result, tmp_buffer);
		}
		if (x == 1 || x == num - 1) {
			continue;
		}
		else {
			long long r = 0;
			for (r; r <= s - 1; r++) {
				long long n_x = multiply_calculation_method_square(a, 2 ^ r*d, num);
				if (DEBUG) {
					sprintf(tmp_buffer, "The result of %lld in (0, s-1) , (a**(d * 2**r)):%lld**(%lld * 2**%lld) mod N is %lld.\n", r, a, d, r, x);
					strcat(result_buffer, tmp_buffer);
					strcat(miller_rabin_result, tmp_buffer);
				}
				if (n_x == (num - 1)) {
					continue;
				}
				else if (n_x == 1 && x != num - 1) {
					sprintf(tmp_buffer, "Mailler_Rabin checking faild, the num is a composite.\n\n");
					strcat(result_buffer, tmp_buffer);
					strcat(miller_rabin_result, tmp_buffer);
					return 0;
				}
			}
		}
	}
	return 1;
}

int probale_prime(long long num) {
	if (num == 2)
		return 1;
	else if (num % 2 == 0 || num == 1)
		return 0;
	if (!check_prime_in_thounsand_prime(num))
		return 0;
	if (!check_miller_rabin(num))
		return 0;

	return 1;
}

long long random_prime(long long min_border, long long max_border) {
	long long new_range = (max_border - min_border + 1);

	long long prime = rand() % new_range + min_border;
	new_range += new_range % 2;
	prime += 1 - prime % 2;
	while (1) {
		if (probale_prime(prime)) {
			return prime;
		}
		prime += 2;
		if (prime > max_border) {
			prime = prime % max_border;
			prime += min_border;
		}

		// prime = (prime + 2) % new_range + min_border;
	}

}

//���e^-1 mod modulus method

long long ext_gcd(long long e, long long modulus) {
	long long a = e % modulus, b = modulus;
	long long x = 0, y = 1, x0 = 1, y0 = 0, q, temp;
	if (GCD(a, b) != 1) {
		return -1;
	}
	else {
		while (a != 0) {
			q = b / a;
			temp = b % a;
			b = a;
			a = temp;
			temp = x;
			x = x0 - q * x;
			x0 = temp;
			temp = y;
			y = y0 - q * y;
			y0 = temp;
		}
		if (y0 < 0)
			y0 += modulus;

		return y0;
	}
}

long long encode(long long m, long long e, long long modulus) {
	return repeat_calculation_method_square(m, e, modulus);
}

long long decode(long long c, long long d, long long modulus) {
	return multiply_calculation_method_square(c, d, modulus);
}

int return_string(char tmp_str[20480], char tmp2_str[20480], char tmp3_str[20480]) {
	
	memcpy(tmp_str, result_buffer, strlen(result_buffer));
	memcpy(tmp2_str, thousand_prime_result, strlen(result_buffer));
	memcpy(tmp3_str, miller_rabin_result, strlen(result_buffer));
	memset(result_buffer, 0, sizeof(result_buffer) / sizeof(char));
	memset(thousand_prime_result, 0, sizeof(result_buffer) / sizeof(char));
	memset(miller_rabin_result, 0, sizeof(result_buffer) / sizeof(char));
 	 
	return 1;
}