##
## Configures Skribe.  We expect skribe, dvipdf, and latex to be in
## the user's path.
##
## Output:
##
## SKRIBE:   Path of the Skribe program.
## PDFLATEX: Path of the pdflatex program.
##
## LATEXOK:  AM_CONDITIONAL set if Latex can be used.
##
AC_DEFUN([AM_SKRIBE],
[

AC_PATH_PROG(SKRIBE,skribe)
if test "x$SKRIBE" = x ; then
  echo "The skribe program was not found.  Documentation will not be built."
  echo "To build the documentation, make sure that skribe is installed and in your path."
  echo "You can find skribe here:  http://www-sop.inria.fr/mimosa/fp/Skribe/"
  DOCDIR=
fi

AM_PDFLATEX

])

AC_DEFUN([AM_PDFLATEX],
[
AC_PATH_PROG(PDFLATEX,pdflatex)

if [[ x$PDFLATEX = x ]] ; then
  echo "PDF documentation will not be build because pdflatex could not be found."
else
  AC_CACHE_CHECK([that latex is good],[adl_cv_prog_pdflatex_works],[

## Make sure that our latex is good- it has to have the most
## recent versixbon of hyperref.
latextest=latex-test.tex
rm -f $latextest
cat >$latextest <<_LATEXEND
\documentclass{book}
\usepackage{color}
\usepackage[[setpagesize=false]]{hyperref}
\usepackage{epsfig}


%% colors
\definecolor{c1033}{rgb}{0.0,0.81176470588235,0.0}
\definecolor{c1034}{rgb}{1.0,0.0,0.0}
\definecolor{c1035}{rgb}{0.67843137254902,0.26274509803922,0.52549019607843}
\definecolor{c1036}{rgb}{0.098039215686275,0.098039215686275,0.68627450980392}
\definecolor{c1037}{rgb}{0.41176470588235,0.34901960784314,0.81176470588235}
\definecolor{c1038}{rgb}{1.0,0.65098039215686,0.0}


\newdimen\oldframetabcolsep
\newdimen\oldcolortabcolsep

\title{The Plasma Language}
\begin{document}
\date{}
\maketitle
\noindent Sample text.\par


%% First-chapter
\chapter{First chapter}
\label{First-chapter}
\noindent Chapter text.\par

\end{document}
_LATEXEND

adl_cv_prog_pdflatex_works=no
if $PDFLATEX -interaction=nonstopmode $latextest > /dev/null ; then
  adl_cv_prog_pdflatex_works=yes
fi
rm -f latex-test.*
])

if [[ $adl_cv_prog_pdflatex_works = no ]]; then
  AC_WARN([You need to get the most recent version of the hyperref package in order for pdflatex to work.])
  PDFLATEX=
fi

fi

AM_CONDITIONAL(LATEXOK, test x$PDFLATEX != x)

])
