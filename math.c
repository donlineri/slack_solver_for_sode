#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define COUNT_INPUT_ARGS 6
#define FILE_POS 11

#define WORK_FOLDER "/var/www/filetrash/share/"
#define PATH_TEX_TEMPLATE "formula_template.tex"
#define PATH_RESULT_TEX_FILE "/var/www/filetrash/share/formula.tex"
#define IMG_INSERT_TEXT "<img src='/share/formula.jpg' alt='formula'>\n"
#define PATH_BUILDED_DOCUMENT "/var/www/filetrash/share/formula.dvi"
#define PATH_RESULT_IMG "/var/www/filetrash/share/formula.jpg"

#define HEADER "Content-Type: text/html\n\n"
#define BEGIN_DOCUMENT "<!DOCTYPE html>\n<html>\n<head>\n<title>index</title>\n<meta charset='utf-8'>\n</head>\n<body>\n"
#define HELP_TEXT "Send GET request, example: <a href='?2.0&1.0&0.0&4.0&1.0&0.0'>http://donlineri.space/math.cgi?2.0&1.0&0.0&4.0&1.0&0.0</a>\n"
#define END_DOCUMENT "</body>\n</html>\n"
#define BAD_REQUEST_TEXT "Bad request, <a href='http://donlineri.space/cgi/math.cgi?2.0&1.0&0.0&4.0&1.0&0.0'>Example</a>"

#ifdef CGI

#define LINEAR_HOMOGENEOUS_SYSTEM_TEX "\\[ \\left\\{\\begin{aligned} & \\dot x = %fx+%fy \\\\ & \\dot y = %fx+%fy \\end{aligned}\\right., \\quad \\left\\{\\begin{aligned} & x(0) = %f \\\\ & y(0) = %f \\end{aligned}\\right.\\]"
#define OO_REAL_STR1 "\\[%c(t,C_1,C_2) = C_1e^{%ft} + C_2e^{%ft}\\]\n"
#define OO_REAL_STR2 "\\[%c(t,C_1,C_2) = %fC_1e^{%ft} + %fC_2e^{%ft}\\]\n"
#define OO_REAL_KOSHI_STR "\\[%c(t) = %fe^{%ft} + %fe^{%ft}\\]\n"
#define OO_MULTIPLE_STR1 "\\[%c(t,C_1,C_2) = C_1e^{%ft} + C_2te^{%ft}\\]\n"
#define OO_MULTIPLE_STR2 "\\[%c(t,C_1,C_2) = %fC_1e^{%ft} + (%f+%ft)C_2e^{%ft}\\]\n"
#define OO_MULTIPLE_KOSHI_STR1 "\\[%c(t) = %fe^{%ft} + %fte^{%ft}\\]\n"
#define OO_MULTIPLE_KOSHI_STR2 "\\[%c(t) = %fe^{%ft} + %f(%f+%ft)e^{%ft}\\]\n"
#define OO_COMPLEX_STR1 "\\[%c(t,C_1,C_2) = C_1e^{%ft}cos(%ft) + C_2e^{%ft}sin(%ft)\\]\n"
#define OO_COMPLEX_STR2 "\\[%c(t,C_1,C_2) = (%fcos(%ft)+%fsin(%ft))C_1e^{%ft} +\\] \\[+ (%fsin(%ft)+%fcos(%ft))C_2e^{%ft}\\]\n"
#define OO_COMPLEX_KOSHI_STR1 "\\[%c(t) = %fe^{%ft}cos(%ft) + %fe^{%ft}sin(%ft)\\]\n"
#define OO_COMPLEX_KOSHI_STR2 "\\[%c(t) = %f(%fcos(%ft)+%fsin(%ft))e^{%ft} +\\] \\[+ %f(%fsin(%ft)+%fcos(%ft))e^{%ft}\\]\n"
#define OO_FIRST_ORDER_STR "\\[%c(t,C_%d) = C_%de^{%ft}\\]\n"
#define OO_FIRST_ORDER_KOSHI_STR "\\[%c(t) = %fe^{%ft}\\]\n"
#define OO_ODZ "\\[t \\in R, C_1,C_2 \\in R\\]\n"
#define OO_KOSHI_ODZ "\\[t \\in R\\]\n"

#else

#define OO_REAL_STR1 "%c(t,C1,C2) = C1*e^{%ft} + C2*e^{%ft}\n"
#define OO_REAL_STR2 "%c(t,C1,C2) = %f*C1*e^{%ft} + %f*C2*e^{%ft}\n"
#define OO_REAL_KOSHI_STR "%c(t) = %f*e^{%ft} + %f*e^{%ft}\n"
#define OO_MULTIPLE_STR1 "%c(t,C1,C2) = C1*e^{%ft} + t*C2*e^{%ft}\n"
#define OO_MULTIPLE_STR2 "%c(t,C1,C2) = %f*C1*e^{%ft} + (%f+%f*t)*C2*e^{%ft}\n"
#define OO_MULTIPLE_KOSHI_STR1 "%c(t) = %f*e^{%ft} + %f*t*e^{%ft}\n"
#define OO_MULTIPLE_KOSHI_STR2 "%c(t) = %f*e^{%ft} + %f*(%f+%f*t)*e^{%ft}\n"
#define OO_COMPLEX_STR1 "%c(t,C1,C2) = C1*e^{%ft}cos(%ft) + C2*e^{%ft}sin(%ft)\n"
#define OO_COMPLEX_STR2 "%c(t,C1,C2) = (%f*cos(%ft)+%f*sin(%ft))C1*e^{%ft} + (%f*sin(%ft)+%f*cos(%ft))C2*e^{%ft}\n"
#define OO_COMPLEX_KOSHI_STR1 "%c(t) = %f*e^{%ft}cos(%ft) + %f*e^{%ft}sin(%ft)\n"
#define OO_COMPLEX_KOSHI_STR2 "%c(t) = %f*(%f*cos(%ft)+%f*sin(%ft))*e^{%ft} + %f*(%f*sin(%ft)+%f*cos(%ft))*e^{%ft}\n"
#define OO_FIRST_ORDER_STR "%c(t,C%d) = C%d*e^{%ft}\n"
#define OO_FIRST_ORDER_KOSHI_STR "%c(t) = %f*e^{%ft}\n"
#define OO_ODZ "t \\in R, C1,C2 \\in R\n"
#define OO_KOSHI_ODZ "t \\in R\n"

#endif

struct complex_num {
	double alfa, beta;
	int is_complex;
};

struct complex_num *solve_square_equation(double a, double b, double c)
{
	struct complex_num *roots = malloc(sizeof(struct complex_num)*2);
	double d = b*b - 4*a*c;
	if(d < 0.0) {
		roots[0].is_complex = 1;
		roots[1].is_complex = 1;
		roots[0].alfa = -b/(2*a);
		roots[0].beta = sqrt(-d)/(2*a);
		roots[1].alfa = -b/(2*a);
		roots[1].beta = -sqrt(-d)/(2*a);
	}
	else {
		roots[0].alfa = (-b+sqrt(d))/(2*a);
		roots[0].beta = 0.0;
		roots[1].alfa = (-b-sqrt(d))/(2*a);
		roots[1].beta = 0.0;
	}
	return roots;
}

void lhs_independent_solve(FILE *output, double a11, double a22, double x0,
													 double y0)
{
	fprintf(output, OO_FIRST_ORDER_STR, 'x', 1, 1, a11);
	fprintf(output, OO_FIRST_ORDER_STR, 'y', 2, 2, a22);
	fprintf(output, OO_ODZ);
	fprintf(output, OO_FIRST_ORDER_KOSHI_STR, 'x', x0, a11);
	fprintf(output, OO_FIRST_ORDER_KOSHI_STR, 'y', y0, a22);
	fprintf(output, OO_KOSHI_ODZ);
}

void lhs_complex_solve(FILE *output, double alfa, double beta, double s1,
											 double s2, double c1, double c2, char vardep1,
											 char vardep2)
{
	fprintf(output, OO_COMPLEX_STR1, vardep1, alfa, beta, alfa, beta);
	fprintf(output, OO_COMPLEX_STR2, vardep2, s1, beta, -s2, beta, alfa, s1,
					beta, s2, beta, alfa);
	fprintf(output, OO_ODZ);
	fprintf(output, OO_COMPLEX_KOSHI_STR1, vardep1, c1, alfa, beta, c2, alfa,
					beta);
	fprintf(output, OO_COMPLEX_KOSHI_STR2, vardep2, c1, s1, beta, -s2, beta,
					alfa, c2, s1, beta, s2, beta, alfa);
	fprintf(output, OO_KOSHI_ODZ);
}

void lhs_multiple_solve(FILE *output, double root, double s1, double s2,
												double c1, double c2, char vardep1, char vardep2)
{
	fprintf(output, OO_MULTIPLE_STR1, vardep1, root, root);
	fprintf(output, OO_MULTIPLE_STR2, vardep2, s1, root, s2, s1, root);
	fprintf(output, OO_ODZ);
	fprintf(output, OO_MULTIPLE_KOSHI_STR1, vardep1, c1, root, c2, root);
	fprintf(output, OO_MULTIPLE_KOSHI_STR2, vardep2, s1*c1, root, c2, s2, s1, root);
	fprintf(output, OO_KOSHI_ODZ);
}

void lhs_real_solve(FILE *output, double root1, double root2, double s1,
										double s2, double c1, double c2, char vardep1,
										char vardep2)
{
	fprintf(output, OO_REAL_STR1, vardep1, root1, root2);
	fprintf(output, OO_REAL_STR2, vardep2, s1, root1, s2, root2);
	fprintf(output, OO_ODZ);
	fprintf(output, OO_REAL_KOSHI_STR, vardep1, c1, root1, c2, root2);
	fprintf(output, OO_REAL_KOSHI_STR, vardep2, s1*c1, root1, s2*c2, root2);
	fprintf(output, OO_KOSHI_ODZ);
}

void lhs_solve(FILE *output, double a1, double a2, double k1, double k2,
							 double vardep1_0, double vardep2_0, char vardep1, char vardep2)
{
	double c1, c2, s1, s2, beta, epsilon = 0.00000001;
	struct complex_num *roots = solve_square_equation(1, a1, a2);
	if(roots[0].is_complex) {
		beta = fabs(roots[0].beta);
		s1 = k1*(roots[0].alfa) + k2;
		s2 = beta*k1;
		c1 = vardep1_0;
		c2 = (vardep2_0 - s1*vardep1_0)/s2;
		lhs_complex_solve(output, roots[0].alfa, beta, s1, s2, c1, c2, vardep1,
											vardep2);
	}
	else if(fabs(fabs(roots[0].alfa)-fabs(roots[1].alfa)) < epsilon) {
		s1 = roots[0].alfa*k1+k2;
		s2 = k1;
		c1 = vardep1_0;
		c2 = (vardep2_0 - s1*vardep1_0)/s2;
		lhs_multiple_solve(output, roots[0].alfa, s1, s2, c1, c2, vardep1, vardep2);
	}
	else {
		s1 = k1*(roots[0].alfa)+k2;
		s2 = k1*(roots[1].alfa)+k2;
		c1 = (vardep2_0 - s2*vardep1_0)/(s1-s2);
		c2 = vardep1_0-c1;
		lhs_real_solve(output, roots[0].alfa, roots[1].alfa, s1,
									 s2, c1, c2, vardep1, vardep2);
	}
	free(roots);
}

void choice_lhs_type(FILE *output, double a11, double a12, double a21,
										 double a22, double x0, double y0)
{
	if((a12 == 0.0) && (a21 == 0.0)) {
		lhs_independent_solve(output, a11, a22, x0, y0);
	}
	else if(a12 != 0) {
		lhs_solve(output, -a11-a22, a11*a22-a12*a21, 1/a12, -a11/a12, x0, y0, 'x',
							'y');
	}
	else if(a21 != 0) {
		lhs_solve(output, -a11-a22, a11*a22-a12*a21, 1/a21, -a22/a21, y0, x0, 'y',
							'x');
	}
}

void bad_request()
{
	printf(BAD_REQUEST_TEXT);
	printf(END_DOCUMENT);
	exit(0);
}

void read_get_request(char *req, double *input)
{
	double n = 0;
	int i, j = -1, is_integral = 1, k = 0, pos_token = -1, is_neg = 0;
	for(i = 0; i < strlen(req); i++) {
		if(req[i] == '&') {
			pos_token = i;
			if(is_neg)
				n = -n;
			input[k] = n;
			k++;
			if(k ==	COUNT_INPUT_ARGS)
				bad_request();
			is_integral = 1;
			is_neg = 0;
			n = 0;
			j = -1;
			continue;
		}
		if((req[i] == '-') && (i-1 == pos_token)) {
			is_neg = 1;
			continue;
		}
		if(req[i] == '.') {
			is_integral = 0;
			continue;
		}
		if((req[i] < '0') || (req[i] > '9')) {
			bad_request();
		}
		if(is_integral)
			n = n*10 + (req[i]-'0');
		else {
			n += (req[i]-'0')*pow(10, j);
			j--;
		}
	}
	if(k != 5)
		bad_request();
	if(is_neg)
		n = -n;
	input[k] = n;
}

void do_img()
{
	int pid;
	char *cmdline1[] = { "latex", "-output-directory", WORK_FOLDER, PATH_RESULT_TEX_FILE, NULL };
	char *cmdline2[] = { "convert", "-density", "150", "-trim", PATH_BUILDED_DOCUMENT, "-quality", "100", "-flatten", "-sharpen", "0x1.0", "-crop", "1300x400+0+100", PATH_RESULT_IMG, NULL };
	pid = fork();
	if(!pid) {
		close(1);
		close(2);
		open("/dev/null", O_WRONLY);
		open("/dev/null", O_WRONLY);
		execvp("latex", cmdline1);
		perror("latex");
		_exit(1);
	}
	wait(NULL);
	pid = fork();
	if(!pid) {
		execvp("convert", cmdline2);
		perror("convert");
		_exit(1);
	}
	wait(NULL);
	printf(IMG_INSERT_TEXT);
}

int main()
{
	/* double a11 = 2.0, a12 = 1.0, a21 = 0.0, a22 = 4.0; */
	double a11, a12, a21, a22, x0, y0;
#ifndef CGI
	scanf("%lf;%lf;%lf;%lf;%lf;%lf", &a11, &a12, &a21, &a22, &x0, &y0);
	printf("a11=%f;a12=%f;a21=%f;a22=%f;x0=%f;y0=%f\n", a11, a12, a21, a22, x0, y0);
	choice_lhs_type(stdout, a11, a12, a21, a22, x0, y0);
#else
	FILE *f_template, *f_result;
	char buf[128];
	int i;
	printf(HEADER);
	printf(BEGIN_DOCUMENT);
	char *query = getenv("QUERY_STRING");
	if(!strlen(query)) {
		printf(HELP_TEXT);
		printf(END_DOCUMENT);
		return 0;
	}
	double *input = malloc(sizeof(double)*COUNT_INPUT_ARGS);
	read_get_request(query, input);
	a11 = input[0];
	a12 = input[1];
	a21 = input[2];
	a22 = input[3];
	x0 = input[4];
	y0 = input[5];
	f_template = fopen(PATH_TEX_TEMPLATE, "r");
	if(!f_template) {
		printf("1:Something's wrong");
		return 0;
	}
	f_result = fopen(PATH_RESULT_TEX_FILE, "w");
	if(!f_result) {
		printf("2:Something's wrong");
		return 0;
	}
	for(i = 0; i < FILE_POS; i++) {
		fgets(buf, 128, f_template);
		fputs(buf, f_result);
	}
	fprintf(f_result, LINEAR_HOMOGENEOUS_SYSTEM_TEX, a11, a12, a21, a22, x0, y0);
	choice_lhs_type(f_result, a11, a12, a21, a22, x0, y0);
	while(fgets(buf, 128, f_template))
		fputs(buf, f_result);
	fclose(f_template);
	fclose(f_result);
	do_img();
	printf(END_DOCUMENT);
#endif
	return 0;
}
