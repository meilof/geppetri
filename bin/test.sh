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


rm -f ../viff/data/*

export LD_LIBRARY_PATH=.
export PYTHONPATH=../viff

shopt -s nullglob

./qapgen --size 8 --inputters 3

for fl in ../viff/$inputpref-*.txt
do
    fl=${fl/\.\.\/viff\/$inputpref-/}
    fl=${fl/\.txt/}
    python input.py $fl ../viff/$inputpref-$fl.txt
    ./qapinput ../viff/data/geppblk.$fl > ../viff/data/geppblk.$fl.comm
done

cd ../viff

if [ "$mpc" = "yes" ]
then
  python $script player-1.ini &
  python $script player-2.ini &
  python $script player-3.ini &
  wait
else
  python $script player-1.ini --local
fi

cd ../bin

if [ "$mpc" = "yes" ]
then
  cp ../viff/data/geppeq1 ../viff/data/geppeq
  cp ../viff/data/geppout1 ../viff/data/geppout
fi

python qapsplit.py ../viff/data/geppeq

# make missing input commitments
for i in `python printinputs.py ../viff/data/geppeq.schedule`
do
    if [ ! -f ../viff/data/$i.comm ]; then
        ./qapinput ../viff/data/geppblk.$i > ../viff/data/geppblk.$i.comm
    fi
done

# make computation proof
for i in `cat ../viff/data/geppeq.qaplist`
do
    ./qapgenf ../viff/data/geppeq.$i
done

if [ "$mpc" = "yes" ]
then
  ./qapprove ../viff/data/geppval1 > ../viff/data/gepproof1
  ./qapprove ../viff/data/geppval2 > ../viff/data/gepproof2
  ./qapprove ../viff/data/geppval3 > ../viff/data/gepproof3
  ./qapcombine ../viff/data/gepproof1 ../viff/data/gepproof2 ../viff/data/gepproof3 > ../viff/data/gepproof
else
  ./qapprove > ../viff/data/gepproof
fi

./qapver
