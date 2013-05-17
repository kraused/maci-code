
#include "mex.h"
#include <math.h>
#include <iso646.h>

#define INLINE
#define RESTRICT

/* Input */
#define IPOS    0
#define IRCUT	1
#define IEPS	2
#define ISIGMA	3
#define IWEIGHT 4

INLINE void checkInput(int nrhs, const mxArray* prhs[])
{
	int i;
	
	/* Check if all of them are doubles */
	for(i = 0; i < nrhs; ++i) {
		if(!mxIsDouble(prhs[i])) {
			mexErrMsgTxt("Invalid input.");
		}
	}
	
	if(1 != mxGetNumberOfElements(prhs[IRCUT ]) || 
	   1 != mxGetNumberOfElements(prhs[IEPS  ]) ||
	   1 != mxGetNumberOfElements(prhs[ISIGMA])) {
		   mexErrMsgTxt("Invalid input.");
	}
	
	if(1 != mxGetM(prhs[IPOS])) {
		mexErrMsgTxt("Invalid input.");
	}
	
	if(5 == nrhs) {
		if(mxGetN(prhs[IWEIGHT]) != mxGetN(prhs[IPOS]) ||
		   mxGetM(prhs[IWEIGHT]) != mxGetM(prhs[IPOS])) {
			   mexErrMsgTxt("Sizes do not match.");
		}
	}
}

INLINE void ljNoWeight(double sigma, double eps, double rcut,
		       int N, double* p, double* F)
{
	double r, s2, s6;
	int    i, j;

	for(i = 0; i < N; ++i) {
		/* Assuming that the overall ordering of atoms is still
		 * valid, i.e. i < j implies A(i) < A(j)
		 */
		for(j = i-1; j >= 0; --j) {
			r  = p[i] - p[j];
			
			if(r > rcut) {
				break;
			}
						
			s2    = sigma*sigma/(r*r);
			s6    = s2*s2*s2;
			F[i] -= 24*eps*s6*(1 - 2*s6)/r;
		}
		for(j = i+1; j <  N; ++j) {
			r  = p[j] - p[i];
			
			if(r > rcut) {
				break;
			}
				
			
			s2    = sigma*sigma/(r*r);
			s6    = s2*s2*s2;			
			F[i] += 24*eps*s6*(1 - 2*s6)/r;			
		}
	}
}

INLINE void ljWeight(double sigma, double eps, double rcut,
		     int N, double* p, double* F, double* weight)
{
	double r, s2, s6, w;
	int    i, j;
			
	for(i = 0; i < N; ++i) {
		/* Assuming that the overall ordering of atoms is still
		 * valid, i.e. i < j implies A(i) < A(j)
		 */
		for(j = i-1; j >= 0; --j) {
			r  = p[i] - p[j];
			
			if(r > rcut) {
				break;
			}
			
			w     = 0.5*(weight[i] + weight[j]);
			
			s2    = sigma*sigma/(r*r);
			s6    = s2*s2*s2;
			F[i] -= w*24*eps*s6*(1 - 2*s6)/r;
		}
		for(j = i+1; j <  N; ++j) {
			r  = p[j] - p[i];
			
			if(r > rcut) {
				break;
			}
				
			w     = 0.5*(weight[i] + weight[j]);			
			
			s2    = sigma*sigma/(r*r);
			s6    = s2*s2*s2;
			F[i] += w*24*eps*s6*(1 - 2*s6)/r;			
		}
	}
}

/* ljf
 * Lennard Jones force evaluation
 */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	double  rcut, eps, sigma, r, s2, s6;
	double* pos;
	double* F;
	int     N;
		
	if(nrhs < 4) {
		mexErrMsgTxt("At least four input arguments required.");
	}
	if(nlhs > 1 || nlhs < 1) {
		mexErrMsgTxt("Wrong number of output arguments.");
	}
		
	checkInput(nrhs,prhs);	
	
	N     = mxGetN     (prhs[IPOS  ]);
	pos   = mxGetPr    (prhs[IPOS  ]);
	rcut  = mxGetScalar(prhs[IRCUT ]);
	eps   = mxGetScalar(prhs[IEPS  ]);
	sigma = mxGetScalar(prhs[ISIGMA]);
		
	/* Create force vector. This one is already initialized to 0 */
	plhs[0] = mxCreateDoubleMatrix(1,N,mxREAL);
		
	if(4 == nrhs) {
		ljNoWeight(sigma, eps, rcut, N, pos, mxGetPr(plhs[0]));
	} else if(5 == nrhs) {
		ljWeight(sigma, eps, rcut, N, pos, mxGetPr(plhs[0]),
			 mxGetPr(prhs[IWEIGHT]));
	} else {
		mexErrMsgTxt("Wrong number of input arguments.");
	}
}
