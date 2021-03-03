L0DpGaussByNseg :
  L0-segmentation using an existing dynamic programming algorithm
  proposed by Bellman.  Running time scales quadratically in 
  sequence length.  This is referred to under ``Bellman" in table 2.
 
L0GaussByNseg :
  This is the proposed segmenation algorithm referred to as 'DpSegPen' in 
  the text.  Appears to scales linearly with the sequence length for
  random sequences.  Worst-case performance is at least O(N^2) where 'N' is
  the sequence length  e.g. an increasing sequence with x_{k+1} - x_k decreasing
  To use this you specify the number of desired segments.

L0Gauss :
  This is the proposed segmenation algorithm referred to as 'DpSegPen' in 
  the text.  You specify a jump-penalty with this version.  If the number of jumps
  is unknown, it is faster to tune this jump penalty than to repeatedly
  call 'L0GaussByNseg' for different numbers of segments.
