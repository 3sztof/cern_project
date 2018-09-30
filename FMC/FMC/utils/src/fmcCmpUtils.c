/* ######################################################################### */
/*
 * $Log: fmcCmpUtils.c,v $
 * Revision 1.6  2008/10/23 07:41:28  galli
 * minor changes
 *
 * Revision 1.2  2007/08/14 07:49:39  galli
 * working version
 *
 * Revision 1.1  2006/10/05 16:06:34  galli
 * Initial revision
 */
/* ######################################################################### */
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#define BUF_SZ 4096
/* ######################################################################### */
static char rcsid[]="$Id: fmcCmpUtils.c,v 1.6 2008/10/23 07:41:28 galli Exp $";
/* ######################################################################### */
static int minimum(int a,int b,int c);
/* ######################################################################### */
/* return 1 if similar and 0 if different */
int wordCmp(char *s1,char *s2,int diff)
{
  char *p1,*p2;
  int l1,l2;
  int diffCnt;
  /*-------------------------------------------------------------------------*/
  if(!s1 && !s2)return 1;
  if(!s1 || !s2)return 0;
  /*-------------------------------------------------------------------------*/
  for(p1=s1,p2=s2,diffCnt=0;;)
  {
    p1+=strspn(p1," \t");                                     /* skip blanks */
    p2+=strspn(p2," \t");                                     /* skip blanks */
    l1=strcspn(p1," \t");                                     /* word length */
    l2=strcspn(p2," \t");                                     /* word length */
    if(!l1 && !l2)break;                         /* no more words in strings */
    diffCnt+=!!strncmp(p1,p2,l1>l2?l1:l2);                   /* compare word */
    if(diffCnt>diff)break;
    p1+=l1;                                              /* jump to word end */
    p2+=l2;                                              /* jump to word end */
  }
  /*-------------------------------------------------------------------------*/
  return (diffCnt<=diff);
}
/*****************************************************************************/
/* Evaluate the levenshtein distance between s and t                         */
/* derived from from Lorenzo Seidenari (sixmoney@virgilio.it)                */
/* http://www.merriampark.com/ldc.htm                                        */
/*****************************************************************************/
/* The Levenshtein Distance is the basic edit distance function whereby the  */
/* distance is given simply as the minimum edit distance which transforms    */
/* string1 into string2. Edit Operations are listed as follows:              */
/* Copy character from string1 over to string2 (cost 0)                      */
/* Delete a character in string1 (cost 1)                                    */
/* Insert a character in string2 (cost 1)                                    */
/* Substitute one character for another (cost 1)                             */
/*****************************************************************************/
int levDist(char *s,char *t)
{
  int k,i,j,n,m,cost,*d,distance;
  /*-------------------------------------------------------------------------*/
  if(!s||!t)return -2;                       /* one or both strings are NULL */
  /* Step 1 */
  n=strlen(s);
  m=strlen(t);
  if(n==0||m==0)return -1;                  /* one or both strings are empty */
  d=malloc((sizeof(int))*(m+1)*(n+1));
  m++;
  n++;
  /* Step 2 */
  for(k=0;k<n;k++)d[k]=k;
  for(k=0;k<m;k++)d[k*n]=k;
  /* Step 3 and 4 */
  for(i=1;i<n;i++)
  {
    for(j=1;j<m;j++)
    {
      /* Step 5 */
      if(s[i-1]==t[j-1])cost=0;
      else cost=1;
      /* Step 6 */
      d[j*n+i]=minimum(d[(j-1)*n+i]+1,d[j*n+i-1]+1,d[(j-1)*n+i-1]+cost);
    }
  }
  distance=d[n*m-1];
  free(d);
  return distance;
}
/*****************************************************************************/
/* gets the minimum of three values */
static int minimum(int a,int b,int c)
{
  int min=a;
  if(b<min)min=b;
  if(c<min)min=c;
  return min;
}
/*****************************************************************************/
char *getFmcCmpUtilsVersion()
{
  return rcsid;
}
/* ######################################################################### */
