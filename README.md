# geppetri

git submodule init
git submodule update
cd ate-pairing
make SUPPORT_SNARK=1
open qaps.workplace with CodeBlocks and rebuild all


\documentclass[english]{article}
\usepackage[T1]{fontenc}
\usepackage[latin9]{inputenc}
\usepackage{amsmath}
\usepackage{amsthm}
\usepackage{amssymb}
\usepackage{hyperref}
\usepackage{times}
\usepackage{stmaryrd}
\usepackage{xstring}
\usepackage{babel}
\usepackage{MnSymbol}


\begin{document}

\title{Implementation of Trinocchio}
\author{Meilof Veeningen}


\maketitle

\begin{abstract}
This document describes Trinocchio implementation as discussed in \cite{SVdV15} and documents how to reproduce the reported tests.
\end{abstract}


\section{Installation Instructions}

(Tested on Linux, mingw-w64.)

Install TUeVIFF-local (\texttt{http://meilof.home.fmf.nl/}).

Install \texttt{ate-pairing} by following the instructions from \url{https://github.com/herumi/ate-pairing}.
Use the \texttt{SUPPORT\_SNARK=1} to use SNARK-friendly pariings.
Then, edit the file locations in the first lines of the \texttt{Makefile}; and use \texttt{make} to compile.



\section{QAP file format}

The first line starts by giving the number of circuit values of the QAP.
Then, the number of values in each ``block'' is given: the first block consists of public inputs and outputs; each party delivering a private input or obtaining a private output should have a block; and there should be one block for the internal circuit wires of the circuit.
The line ends with a dot.
For instance,
\begin{verbatim}
 25649 0 1640 41 41 23927 .
\end{verbatim}
represents a QAP with 25649 circuit values: 0 public and 25649 private, divided into blocks of 1640, 41, 41, and 23927 values. (In this case, there will be 1640 inputs, 41 outputs for one party, 41 outputs for another party, and 23927 internal wire values.)

The second line specifies inputs to the QAP, starting from the first circuit value, and ending with a dot, e.g., ``\texttt{1 2 3 .}''.
This line is only used for demo purposes by some provers; if this is not needed, then a line with a single dot suffices.

The remainder of the lines give the QAP equations $v\cdot w=y$, with $v$, $w$, and $y$ linear functions $\alpha_1 x_1+\cdots+\alpha_n x_n+\alpha_0$. A term $\alpha_k x_k$ is given as ``\texttt{$\alpha_k$ k}''; the constant term as ``\texttt{$\alpha_0$ 0}''. Plus signs are ommitted, and ``\texttt{*}'' and ``\texttt{=}'' are used to separate the $v$, $w$, and $y$ parts. E.g., ``\texttt{1 1763 * 1 0 -1 1763 = .}'' represents the equation $1\cdot x_{1763}\cdot(1+ -1\cdot x_{1763})=0$.


\section{Tools}

\subsection{\texttt{genkey}}

Usage: \texttt{genkey <qapfile>}

~

\noindent Generate evaluation and verification keys from a given QAP; output both to standard output.

\subsection{\texttt{lpqap}}

Usage: \texttt{lpqap <n> <m> <l>}

~

\noindent Generates a QAP to prove optimality of the solution to a $n$-by-$m$ linear program, using bitlength $l$.

Given a LP $(A,b,c)$ and solution $x$, $p$, $q$ to the LP and its dual, the proof consists of, in order: the $(n+1)$-by-$(m+1)$ tableau, consisting of $A$, $b$, and $c$ \cite{SVdV15}; the values of $q$, $x$, and $p$; a bit decomposition of $q$; partial sums of $cx-pb$; partial sums of $qb-Ax$; bit decompositions of $qb-Ax$; bit decompositions of $x$; partial sums of $qc-pA$; bit decompositions of $qc-pA$; and bit decompositions of $p$.


\subsection{\texttt{combine}}

Usage: \texttt{combine}

~

\noindent Read proofs \texttt{proof1}, \texttt{proof2}, and \texttt{proof3}, and combine them into one overall proof. This combines all blocks from the parts given in the respective proof files.

\subsection{\texttt{eval}}

Usage: \texttt{eval <qap> <evalkey> <wires>}

~

\noindent Produces a ZK-QAP proof based on the given wire values and randomness.
\texttt{<qap>} is a QAP file, and \texttt{<evalkey>} an evaluation key produced by \texttt{genkey}.
\texttt{<wires>} contains the values for all wires of the QAP, followed by 19 randomness values:
shares of $\delta_{v,i}$ for $i=1,2,3$; shares of $\delta_{w,i}$ for $i=1,2,3$; shares of $\delta_{y,i}$ for $i=1,2,3$; and shares of $\delta_{v,4}$, $\delta_{w,4}$ and $\delta_{y,4}$; and finally, seven shares of zero used to randomise the circuit wires.
Prints the proof to standard output.
% TODO: the \delta shares are Shamir and the zero shares are additive, right? how does this work?

\subsection{\texttt{ver}}

Usage: \texttt{ver <qap> <evalkey> <proof>}

~

\noindent Verify a QAP proof. Currently takes as input an evaluation key because \texttt{genkey} does not separately output the verification key, but this could be easily changed.

\subsection{\texttt{Makefile}}\label{sec:makefile}

The \texttt{Makefile} provides an easy way of automatically running the above tools. Edit the \texttt{QAP} variable to point it to the QAP of choice. Then use one of the following targets (\texttt{make <target>}):

\begin{verbatim}
qapkey          # generate evaluation key
viff1:          # (fake target) run party 1 VIFF
viff2:          # (fake target) run party 2 VIFF
viff3:          # (fake target) run party 3 VIFF
proof1          # produce proof by first party
proof2          # produce proof by second party
proof3          # produce proof by third party
proof           # combine proofs into oune
verify          # (fake target) verify proof
\end{verbatim}

\section{Running Experiments}

(Only tested on Linux)

\subsection{Multivariate Polynomial Evaluation}

The distribution contains QAPs \texttt{qap-2-2}, \texttt{qap-5-8}, \texttt{qap-5-10} for the QAPs of various sizes.
To reproduce the experiments from \cite{SVdV15}, run the three scripts 
\begin{verbatim}sh timings-qap-2-2.sh
sh timings-qap-5-8.sh
sh timings-qap-5-10.sh\end{verbatim}
These scripts generate log files in the \texttt{logs} subdirectory from which timing information can be extracted.

\subsection{Verification by Validation}

Install the original universally verifiable linear programming implementation from location \texttt{http://meilof.home.fmf.nl/} using the instructions provided there, but replace \texttt{LPver2.py} by the version provided here that enables computation over the prime field used by Trinocchio. Edit the Makefile to configure which LP to use. Run the LP solver from the LP source direcory using \texttt{make viff1}, \texttt{make viff2}, \texttt{make viff3} for the respective parties.

Next, from the QAP source directory, call \texttt{make lpwires} to make symbolic links to the wire values output by the LP solver (this assumes that the LP implementation is located in \texttt{../../lpqap}; otherwise modify the \texttt{Makefile}). Edit the \texttt{QAP} variable in the Makefile to the QAP corresponding to the linear problem (\texttt{qap-lp5}, \texttt{qap-lp20}, \texttt{qap-sc50a}, \texttt{qap-sc50b}, \texttt{qap-sc105}, \texttt{qap-big}).
Next, use the targets discussed in Section~\ref{sec:makefile} to create and verify the proofs of correctness.



\begin{thebibliography}{SVdV15}

\bibitem[SVdV15]{SVdV15}
B.~Schoenmakers, M.~Veeningen, and N.~de~Vreede.
\newblock Trinocchio: Privacy-friendly outsourcing by distributed verifiable
  computation.
\newblock Cryptology eprint 2015/480, 2015.

\end{thebibliography}


\end{document}
