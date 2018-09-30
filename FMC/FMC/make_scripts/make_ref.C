using namespace std;
#include <string>
#include <stdio.h>
#include "fmcRef.h"
int main()
{
  string fmcUrls=FMC_URLS;
  string fmcAuthors=FMC_AUTHORS;
  string escFmcUrls;
  string escFmcAuthors;
  string::iterator it;
  for(it=fmcUrls.begin();it<fmcUrls.end();it++)
  {
    if(*it=='\\')escFmcUrls+="\\\\";
    else if(*it=='\n')escFmcUrls+="\\n";
    else if(*it=='&')escFmcUrls+="\\&";
    else escFmcUrls+=*it;
  }
  for(it=fmcAuthors.begin();it<fmcAuthors.end();it++)
  {
    if(*it=='\\')escFmcAuthors+="\\\\";
    else if(*it=='\n')escFmcAuthors+="\\n";
    else if(*it=='&')escFmcAuthors+="\\&";
    else escFmcAuthors+=*it;
  }
  printf("%s%s\n",escFmcUrls.c_str(),escFmcAuthors.c_str());
}
