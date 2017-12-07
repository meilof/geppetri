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

# set to yes to do mpc
mpc=yes

## setting 1: block size=1
#script=kmstat.py
#file=btrialm
#lpb=1
#nblocks=175
#qapdeg=8
#nsumm=7
#bpers=25

## setting 2: block size=25
#script=kmstat.py
#file=btrialm
#lpb=25
#nblocks=7
#qapdeg=13
#nsumm=7
#bpers=1

# setting 3: block size=175
script=kmstat.py
file=btrialm
lpb=175
nblocks=1
qapdeg=15
nsumm=1
bpers=1

rm -f ../viff/data/*

export LD_LIBRARY_PATH=.
export PYTHONPATH=../viff

./qapgen --size $qapdeg --inputters 3

python input.py input.1 ../viff/$file.1.txt $nblocks
python input.py input.2 ../viff/$file.2.txt $nblocks

./qapinput_surv ../viff/data/geppblk.input.1 $((4*$lpb)) $nblocks 1
./qapinput_surv ../viff/data/geppblk.input.2 $((4*$lpb)) $nblocks 2


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

# make computation proof
for i in `cat ../viff/data/geppeq.qaplist`
do
    ./qapgenf ../viff/data/geppeq.$i
done

if [ "$mpc" = "yes" ]
then
  ./qapprove_stat $nblocks ../viff/data/geppval > ../viff/data/gepproof
  ./qapprove_stat $nblocks ../viff/data/geppval > ../viff/data/gepproof
  ./qapprove_stat $nblocks ../viff/data/geppval > ../viff/data/gepproof
  ./qapcombine_stat ../viff/data/gepproof1 ../viff/data/gepproof2 ../viff/data/gepproof3 $nblocks > ../viff/data/gepproof
else
  ./qapprove_stat $nblocks ../viff/data/geppval > ../viff/data/gepproof
fi

./qapver_stat $nblocks

