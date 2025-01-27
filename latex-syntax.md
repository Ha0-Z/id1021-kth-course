# Syntax for latex

## Where the document begins
\begin{document}

## New section
\section*{NAME OF SECTION}

## Type writer font
{\tt article}\

**\addplot**\
{\tt \textbackslash addplot}


## Code section
```
\begin{minted}{LANGUAGE}
[
  CODE
]
\end{minted}
```

## Math expression
$EXPRESSION$\

**Micro sybole**\
"$\mu s$"

**n log(n)**\
$n \log(n)$

## Table
**l|c|c** stands for left center center\
```
\begin{table}[h]
    \begin{center}
        \begin{tabular}{l|c|c}
            \textbf{ROW1} & \textbf{ROW2} & \textbf{ROW3}\\
            \hline
              ELEMENT1      &  123 &     ABV\\
              ELEMENT2      &  123 &     ASI\\
              ELEMENT3      &  123 &     SI#\\
            \end{tabular}
            \caption{DESCRIPTION TO TABLE}
        \label{tab:table1}
    \end{center}
\end{table}
```

## Italic
{\em TILT THE WORDS}

## Figure
```
\begin{figure}
  \centering
  \includegraphics[scale=0.45]{PATH}
  \label{fig:NAME}
  \caption{DESCRIPTION}
\end{figure}
```

## Double figure
```
\begin{figure}[h]
  \centering
  \begin{subfigure}{.5\textwidth}
    \centering
    \includegraphics[scale=0.45]{fib.png}
    \caption{using raster graphics}
  \end{subfigure}%
  \begin{subfigure}{.5\textwidth}
    \centering
    \includegraphics[scale=0.45]{fib.pdf}
    \caption{using vector graphics.}
  \end{subfigure}
  \caption{Difference in image formats.}
  \label{fig:LABEL NAME}
\end{figure}
```

## Reference a figure
.\ref{fig:LABEL NAME}

## Graph
```
\begin{figure}
  \centering
  \begin{tikzpicture}
    \begin{axis}[
      xmin=12, xmax=28, ymin=0, ymax=3500,
      xlabel=n, ylabel={time in $\mu s$},
      width=8cm, height=6cm]
      \addlegendentry{runtime fib(n)};
      \addplot[] table {fib.dat};
    \end{axis}

  \end{tikzpicture}
  \caption{The same graph using TikZ}
  \label{fig:tikz}
\end{figure}
```

## Mathmatical comparasion
**<**\
\textless\ \

**>**\
\textgreater\

