/*****************************************************************************/
/*
 * $Log: severityPatterns.h,v $
 * Revision 1.6  2012/11/29 15:52:27  galli
 * Compatibility with gcc/g++ 4.4.6
 *
 * Revision 1.5  2009/02/10 14:11:52  galli
 * changed default regex pattern for DEBUG severity
 *
 * Revision 1.3  2006/11/07 14:47:54  galli
 * Revision 1.2  2006/11/07 13:00:43  galli
 * Revision 1.1  2006/09/27 10:02:24  galli
 * Initial revision
 */
/*****************************************************************************/
char *dfltVerbRegExpPattern= (char*)"\\bverb(|ose)\\b";
char *dfltDebugRegExpPattern=(char*)"\\bdebug(\\b|[0-f]+\\:\\W+)";
char *dfltInfoRegExpPattern= (char*)"\\binfo\\b";
char *dfltWarnRegExpPattern= (char*)"\\bwarn(|ing)\\b|"
                                    "\\bunknown\\b|"
                                    "\\bincomplete\\b|"
                                    "\\bunusual\\b|"
                                    "\\brepeated\\b|"
                                    "\\btoo\\b|"
                                    "\\binterrupted\\b|"
                                    "\\bbusy\\b|"
                                    "\\bexceeded\\b|"
                                    "\\bexpired\\b|"
                                    "\\bcancelled\\b|"
                                    "\\bdeadlock\\b|"
                                    "\\bhalted\\b|"
                                    "\\breset\\b|"
                                    "\\bcan(\\W|_)*not\\b|"
                                    "\\bout(\\W|_)*of(\\W|_)*"
                                    "(range|domain)\\b|"
                                    "\\bno(\\W|_)*locks\\b|"
                                    "\\bno(\\W|_)*space\\b|"
                                    "\\bno(\\W|_)*such\\b|"
                                    "\\bno(\\W|_)+(\\w+(\\W|_)+){0,2}"
                                    "available\\b|"
                                    "\\btry(\\W|_)*again\\b|"
                                    "\\bnot(\\W|_)*a\\b|"
                                    "\\bnot(\\W|_)*attached\\b|"
                                    "\\bnot(\\W|_)*representable\\b|"
                                    "\\bnot(\\W|_)*implemented\\b|"
                                    "\\bnot(\\W|_)*installed\\b|"
                                    "\\bnot(\\W|_)*supported\\b|"
                                    "\\bnot(\\W|_)*permitted\\b|"
                                    "\\bnot(\\W|_)*synchronized\\b|"
                                    "\\bnot(\\W|_)*available\\b|"
                                    "\\bunavailable\\b|"
                                    "\\binappropriate\\b|"
                                    "\\baddress(\\W|_)*required\\b|"
                                    "\\btimed(\\W|_)*out\\b";
char *dfltErrorRegExpPattern=(char*)"\\berror\\b|"
                                    "\\bden(y|ied)\\b|"
                                    "\\billegal\\b|"
                                    "\\bfail(|ed)\\b|"
                                    "\\brefuse(|d)\\b|"
                                    "\\breject(|ed)\\b|"
                                    "\\binvalid\\b|"
                                    "\\bunauthorized\\b|"
                                    "\\bbad\\b|"
                                    "\\boverflow\\b|"
                                    "\\bhost(\\W|_)+(\\w+(\\W|_)+){0,2}"
                                    "down\\b|"
                                    "\\bnetwork(\\W|_)+(\\w+(\\W|_)+){0,2}"
                                    "down\\b|"
                                    "\\bnetwork(\\W|_)+(\\w+(\\W|_)+){0,2}"
                                    "unreachable\\b|"
                                    "\\bno(\\W|_)*route(\\W|_)*to(\\W|_)*"
                                    "host\\b|"
                                    "\\baddress(\\W|_)*already(\\W|_)*"
                                    "in(\\W|_)*use\\b|"
                                    "\\bbroken\\b|"
                                    "\\bwrong\\b|"
                                    "\\bcorrupt(|ed|ing)\\b|"
                                    "\\bstale\\b";
char *dfltFatalRegExpPattern=(char*)"\\bfatal\\b|"
                                    "\\bpanic\\b|"
                                    "\\bsevere(|d)\\b|"
                                    "\\babort\\b";
/*****************************************************************************/
