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
#include <time.h>

#include <libqap/base.h>
#include <libqap/proof.h>

using namespace std;

int main(int argc, char** argv) {
    libqap_init();

    string schedf = "../viff/data/geppeq.schedule";
    ifstream sched(schedf);

    std::ifstream in1(argv[1]), in2(argv[2]), in3(argv[3]);

    while (!sched.eof()) {
        string tok;

        sched >> tok;

        if (tok=="") continue;

        if (tok == "[function]") {
            string type; sched >> type;
            string name; sched >> name;

            qapproof proof1, proof2, proof3;
            in1 >> proof1; in2 >> proof2; in3 >> proof3;
            cout << 3*proof1 - 3*proof2 + proof3;
        } else if (tok=="[input]") {
            string fun; sched >> fun;
            string blk; sched >> blk;
            string fln; sched >> fln;
        } else if (tok=="[glue]") {
            string fun1; sched >> fun1;
            string blk1; sched >> blk1;
            string fun2; sched >> fun2;
            string blk2; sched >> blk2;

            datablock bblk1, bblk2, bblk3;
            in1 >> bblk1; in2 >> bblk2; in3 >> bblk3;
            cout << 3*bblk1 - 3*bblk2 + bblk3;
        } else if (tok=="[output]") {
            string fun; sched >> fun;
            string blk; sched >> blk;
            string tmp; getline(sched, tmp);
        } else {
            cerr << "*** Unrecognized token: " << tok << endl;
        }
    }

    fprintf (stderr, " ... combining done\n");

    return 0;
}
