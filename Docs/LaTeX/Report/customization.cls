\LoadClass[a4paper,12pt]{article} % Classe du document

%--------------------- Packages ------------------------

\RequirePackage[english]{babel}
\RequirePackage[utf8]{inputenc}
\RequirePackage[section]{placeins}
\RequirePackage[T1]{fontenc}
\RequirePackage{mathtools} 
\RequirePackage{siunitx} 
\RequirePackage{float}
\RequirePackage{graphicx} 
\RequirePackage[justification=centering]{caption} 
\RequirePackage{subcaption}
\RequirePackage{wallpaper}
\RequirePackage{nomencl}
%\makenomenclature
\RequirePackage{fancyhdr}
%\pagestyle{fancy}
%\fancyheadoffset{1cm}
%\setlength{\headheight}{2cm}
\RequirePackage{url}
\RequirePackage[hidelinks]{hyperref}
\RequirePackage[left=2.5cm,right=2.5cm,top=2cm,bottom=3.5cm]{geometry} 


%--------------------Information----------------------

\newcommand{\UE}[1]{\renewcommand{\UE}{#1}}
\newcommand{\sujet}[1]{\renewcommand{\sujet}{#1}}
\newcommand{\titre}[1]{\renewcommand{\titre}{#1}}
\newcommand{\enseignant}[1]{\renewcommand{\enseignant}{#1}}
\newcommand{\eleves}[1]{\renewcommand{\eleves}{#1}}

\newcommand{\fairemarges}{
\makenomenclature
\pagestyle{fancy}
\fancyheadoffset{1cm}
\setlength{\headheight}{2cm}
\lhead{\includegraphics[scale=0.05]{logos/lhcbwhite.png}} 
\rhead{\nouppercase{\leftmark}}
\rfoot{\thepage}
\cfoot{Summer student project report}
\lfoot{\UE}
}

\newcommand{\fairepagedegarde}{
\begin{titlepage}

\ThisLRCornerWallPaper{1}{logos/lhcb_wat2.png}
%\CenterWallPaper{0.8}{logos/lhcb_wat2.png}
	\centering 
	\includegraphics[width=0.3\textwidth]{logos/cern.png}\par\vspace{1cm} 
	\vspace{1.5cm}
    \rule{\linewidth}{0.2 mm} \\[0.4 cm]
	{\huge\bfseries \titre \par} \
	\rule{\linewidth}{0.2 mm} \\[1.5 cm]
	%\vspace{1cm}
    {\Large Large Hadron Collider beauty experiment \\ Summer student project report \par}
    \vspace{2cm}
	\begin{minipage}{0.5\textwidth}
		\begin{flushleft} \large 
		\emph{\textbf{Author :}}\\ %
        \eleves\\
		\end{flushleft}
	\end{minipage}
	~
	\begin{minipage}{0.4\textwidth}
		\begin{flushright} \large
		\emph{\textbf{Supervisors:}} \\
		 \enseignant \\
		\end{flushright}
	\end{minipage}\\[4cm]
    
	\vfill
	{\large \today\par}

\end{titlepage}
}

\newcommand{\tabledematieres}{
\tableofcontents
\newpage
}

\newcommand{\insererfigure}[4]{
\begin{figure}[ht]
\centering
\includegraphics[height=#2]{#1}
\caption{#3}
\label{fig: #4}
\end{figure}
}



