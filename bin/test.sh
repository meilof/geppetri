# Copyright (c) 2016-2017 Koninklijke Philips N.V. All rights reserved. A
# copyright license for redistribution and use in source and binary forms,
# with or without modification, is hereby granted for non-commercial,
# experimental and research purposes, provided that the following conditions
# are met:
# - Redistributions of source code must retain the above copyright notice,
#   this list of conditions and the following disclaimers.
# - Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimers in the
#   documentation and/or other materials provided with the distribution. If
#   you wish to use this software commercially, kindly contact
#   info.licensing@philips.com to obtain a commercial license.
# 
# This license extends only to copyright and does not include or grant any
# patent license or other license whatsoever.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

#!/bin/bash

TOOLS=../../bin/

# set to yes to do mpc
mpc=yes

## simple.py
#script=simple.py
#inputpref=simple

## simple2.py
#script=simple2.py
#inputpref=simple2

# simple3.py
script=simple3.py
inputpref=simple3

# make sure we are actually in the data directory
rm -f ../data/*

export LD_LIBRARY_PATH=$TOOLS

shopt -s nullglob

$TOOLS/qapgen --size 8 --smallsize 128 --inputters 3

for fl in ../$inputpref-*.txt
do
    fl=${fl/\.\.\/$inputpref-/}
    fl=${fl/\.txt/}
    echo "File is $fl"
    PYTHONPATH=.. python $TOOLS/input.py $fl ../$inputpref-$fl.txt
    $TOOLS/qapinput geppblk.$fl > geppblk.$fl.comm
done

cd ..

if [ "$mpc" = "yes" ]
then
  python $script player-1.ini &
  python $script player-2.ini &
  python $script player-3.ini &
  wait
else
  python $script player-1.ini --local
fi

cd data

if [ "$mpc" = "yes" ]
then
  cp geppeq1 geppeq
  cp geppout1 geppout
fi

python $TOOLS/qapsplit.py geppeq

# make missing input commitments
for i in `python $TOOLS/printinputs.py geppeq.schedule`
do
    if [ ! -f $i.comm ]; then
        $TOOLS/qapinput geppblk.$i > geppblk.$i.comm
    fi
done

# make computation proof
for i in `cat geppeq.qaplist`
do
    $TOOLS/qapgenf geppeq.$i
done

if [ "$mpc" = "yes" ]
then
  $TOOLS/qapprove geppval1 > gepproof1
  $TOOLS/qapprove geppval2 > gepproof2
  $TOOLS/qapprove geppval3 > gepproof3
  $TOOLS/qapcombine gepproof1 gepproof2 gepproof3 > gepproof
else
  $TOOLS/qapprove > gepproof
fi

$TOOLS/qapver
