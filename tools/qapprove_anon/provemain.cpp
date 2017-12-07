/**
 *
 * Copyright (c) 2016-2017 Koninklijke Philips N.V. All rights reserved. A
 * copyright license for redistribution and use in source and binary forms,
 * with or without modification, is hereby granted for non-commercial,
 * experimental and research purposes, provided that the following conditions
 * are met:
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimers.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimers in the
 *   documentation and/or other materials provided with the distribution. If
 *   you wish to use this software commercially, kindly contact
 *   info.licensing@philips.com to obtain a commercial license.
 *
 * This license extends only to copyright and does not include or grant any
 * patent license or other license whatsoever.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <fstream>
#include <time.h>

#include <libqap/base.h>
#include <libqap/key.h>
#include <libqap/modp.h>
#include <libqap/proof.h>
#include <libqap/prove.h>
#include <libqap/qap.h>

#include <getopt.h>

void print_help() {
    cerr << "qapprove: produce QAP proof" << endl;
}

template<typename X> X readfromfile(string fname) {
    X ret;
    ifstream fl(fname);
    fl >> ret;
    fl.close();
    return ret;
}

int main (int argc, char **argv) {
    libqap_init();

    string keyfile  = "geppmasterkey";
    string wires    = argv[3];
    masterkey mkey = readfromfile<masterkey>(keyfile);
    wirevalt wirevals = readfromfile<wirevalt>(wires);

//    cerr << "Reading for block..." << endl;

    string qapfileb = "geppeq.summ";
    string qapkeyb  = "geppeq.summ.ek";
    qap theqapb = readfromfile<qap>(qapfileb);
    qapek ekb = readfromfile<qapek>(qapkeyb);

    cerr << "Building proof" << endl;
    clock_t clbeg = clock();

    cerr << "Proof block ";

    for (int i = 0; i < atoi(argv[1]); i++) {
        cerr << " " << (i+1);
        //cerr << "Building proof for block " << i << endl;
        stringstream str; str << "summ" << i;
        string pref = str.str();

        qapproof proofb = qapprove(mkey, theqapb, ekb, wirevals, pref, false);

        for (int j = 0; j < atoi(argv[2]); j++) {
            stringstream nm; nm << "input" << j; string nms = nm.str();
            proofb.blocks[nms] = qapblockproof(mkey, theqapb.blocks[nms], ekb.blocks[nms], wirevals, pref);
        }
        proofb.blocks["output"] = qapblockproof(mkey, theqapb.blocks["output"], ekb.blocks["output"], wirevals, pref);
        cout << proofb << endl;
    }
    cerr << endl;


    clock_t clend = clock();
    fprintf (stderr, " ... took %ld clicks (%f seconds)\n",clend-clbeg,((float)(clend-clbeg))/CLOCKS_PER_SEC);


    //qapproof proofm = qapprove(mkey, theqapm, ekm, wirevals, "main", true);
    //proof.blocks["input"] = qapblockproof(mkey, theqap.blocks["input"], ek.blocks["input"], wirevals, "1");



    return 0;
}
