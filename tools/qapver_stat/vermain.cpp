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
#include <map>
#include <set>
#include <time.h>

#include <libqap/base.h>
#include <libqap/key.h>
#include <libqap/prove.h> // TODO: eliminate this include
#include <libqap/proof.h>
#include <libqap/verify.h>

using namespace std;

void print_help() {
    cerr << "qapver: verify QAP proof" << endl;
}

template<typename X> X readfromfile(string fname) {
    X ret;
    ifstream fl(fname);
    fl >> ret;
    fl.close();
    return ret;
}

//bool qapblockver(const masterkey& mk, const datablock& db, const qapvk& qvk, const blockvk& bvk, const blockproof& block);
//bool qapver(const qapvk& qvk, const qapproof& proof);

int main (int argc, char **argv) {
    libqap_init();

    string keyfile = "../viff/data/geppmasterkey";
    masterkey mkey = readfromfile<masterkey>(keyfile);

    string wires = "../viff/data/geppout";
    wirevalt wirevals = readfromfile<wirevalt>(wires);

    string qapfileb = "../viff/data/geppeq.block";
    string qapkeyb  = "../viff/data/geppeq.block.vk";
    qap theqapb = readfromfile<qap>(qapfileb);
    qapvk vkb = readfromfile<qapvk>(qapkeyb);

    string qapfilef = "../viff/data/geppeq.fin";
    string qapkeyf  = "../viff/data/geppeq.fin.vk";
    qap theqapf = readfromfile<qap>(qapfilef);
    qapvk vkf; ifstream fl2(qapkeyf); fl2 >> vkf; fl2.close();

    string prooffile = "../viff/data/gepproof";

    datablock dfinin;

    clock_t clbeg = clock();


    ifstream fl3(prooffile);
    for (int i = 0; i < atoi(argv[1]); i++) {
        qapproof proofb;
        datablock datab;
        fl3 >> proofb >> datab;

        stringstream nm1; nm1 << "../viff/data/geppblk.input.1.comm." << (i+1);
        stringstream nm2; nm2 << "../viff/data/geppblk.input.2.comm." << (i+1);
        datablock din1 = readfromfile<datablock>(nm1.str());
        datablock din2 = readfromfile<datablock>(nm2.str());

        cout << "ver proof " << (i+1) << "? proof " << qapver(vkb, proofb);
        cout << " in1 " << qapblockvalid(mkey, din1, 0);
        cout << " in2 " << qapblockvalid(mkey, din2, 1);
        cout << " in " << qapblockver(mkey, din1+din2, vkb.blocks["input"], proofb.blocks["input"]);
        cout << " out " << qapblockvalid(mkey, datab, 2) << " " << qapblockver(mkey, datab, vkb.blocks["output"], proofb.blocks["output"]) << endl;

        dfinin = dfinin + datab;
    }

    qapproof prooff;
    fl3 >> prooff;

    cout << "ver fin? proof " << qapver(vkf, prooff);
    cout << " in? " << qapblockvalid(mkey, dfinin, 2) << " " << qapblockver(mkey, dfinin, vkf.blocks["input"], prooff.blocks["input"]);
    datablock outbl = buildblock(mkey, 2, theqapf.blocks["output"], wirevals, "fin");
    cout << " out? " << qapblockvalid(mkey, outbl, 2) << " " << qapblockver(mkey, outbl, vkf.blocks["output"], prooff.blocks["output"]) << endl;


    clock_t clend = clock();
    fprintf (stderr, " ... took %ld clicks (%f seconds)\n",clend-clbeg,((float)(clend-clbeg))/CLOCKS_PER_SEC);

    return 0;
}


