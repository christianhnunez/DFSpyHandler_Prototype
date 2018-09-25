//#include "ftkcommon/patternbank_lib.h"

//#include <TRandom3.h>
//#include <TFile.h>
//#include <TTree.h>

//#include <boost/iostreams/filtering_stream.hpp>
//#include <boost/iostreams/filter/gzip.hpp>
//#include <boost/iostreams/filter/bzip2.hpp>
//#include <boost/iostreams/device/file.hpp>

//#include <iomanip>
//#include <iostream>
//#include <sstream>
//using namespace std;


//FTKPatternBank::~FTKPatternBank()
//{
//}

///** This method prints the configuration of a PatternBank object collected so far.
// * The status includes the number of layers, dc bits in the offline banks, dc
// * bits in the chips. The number is different by 0 if a bank is loaded.
// */
//void FTKPatternBank::printConfiguration() const
//{
//    cout << "FTKPatternBank configuration for:" << m_bankname << endl;
//    cout << "Number of layers: " << m_nlayers << endl;
//    cout << "Number of patterns: " << m_npatterns << endl;
//    cout << "Checksum " << hex << m_checksum << dec << endl;
//    cout << "DC bits in offline banks: ";
//    for (unsigned int il=0; il!=m_nlayers; ++il) {
//        cout << m_bankdcbits[il] << " ";
//    }
//    cout << endl;
//    cout << "DC bits in the chips: ";
//    for (unsigned int il=0; il!=m_nlayers; ++il) {
//        cout << m_chipdcbits[il] << " ";
//    }
//    cout << endl;
//    cout << "SS offset: ";
//    for (unsigned int il=0; il!=m_nlayers; ++il) {
//        cout << m_ssoffset[il] << " ";
//    }
//    cout << endl;
//}

///** \bries This method sets the number of layers. because this allocates
// * memory for other elements it should be called as first setup
// * method.
// *
// * This method also sets the default value of all parameters that have
// * a configuration value in each layer: N DC bits in the bank (def 0),
// * N DC bits in the chip (def 2), SS offset for the chip (def 8, 1<<3).
// */
//void FTKPatternBank::setNLayers(unsigned int n) {
//    m_nlayers= n;
//    for (unsigned int il=0; il!=m_nlayers; ++il) {
//        /* the bank from the offline doesn't have a minimum number of DC bits,
//         * the number will probably need to be set during the the init procedure
//         * elsewhere.
//         */
//        m_bankdcbits[il] = 0;
//        /* the AM chip 05 has *at least* 2 ternary cells available for each layer of every pattern,
//         * if this number changes it has to be changed in the setup
//         */
//        m_chipdcbits[il] = 3;

//        //no offset in the incoming hits right now @ CERN - so no offset!
//        m_ssoffset[il] = 0;
//    }
//}


///** Set the DC configuration in all the layer, an array
// * of unsigned int large as the number of layers set has to
// * be passed as argument.
// *
// * It has to be called after the setNLayers() method.
// */
//void FTKPatternBank::setDCConfig(const std::vector<uint32_t> &bankdc) {
//    // set the memory for the DC arrays, if the arrays were already allocate
//    // the memory is released and reallocated. The content will be lost.

//    for (unsigned int il=0; il!=m_nlayers; ++il) {
//        /* the bank from the offline doesn't have a minimum number of DC bits,
//         * the number will probably need to be set during the the init procedure
//         * elsewhere.
//         */
//        m_bankdcbits[il] = bankdc[il];

//    }
//}


///** This method convert an "oldss" with N ternary bits to a new representation
// * with M ternary bits, returned as result of the function. The ternary bits
// * are assumed to be present in the LSB positions and use 2 real bits with the
// * following convention:
// * * 0 --> 01
// * * 1 --> 10
// * * X --> 00 (DC)
// * * Y --> 11 (Veto, not used)
// *
// * If M>N the additional ternary bits are extracted converting M-N afer the LSB
// * in the previous convension. If M<N some ternary bits are reported as normal bits,
// * where the DC (00) is used the always 0 value is reported in the new SS.
// */
//unsigned int FTKPatternBank::convertSSAdjustTernaryBits(unsigned int oldss, unsigned int ndcbits_old, unsigned int ndcbits_new)
//{
//    // don't do anything
//    if (ndcbits_old==ndcbits_new) return oldss;
//    else if (ndcbits_new>ndcbits_old) {
//        // Increase the number of ternary bits

//        // number of bits to encode
//        unsigned int naddbits(ndcbits_new-ndcbits_old);

//        // extract the bits that will not be changed in the oldss.
//        // Those are bits using standard binary format that will not be converted.
//        // The position is after the already existing ternary bits (count 2) and
//        // the standard bits that will be converted.
//        unsigned int oldSS_H = oldss >> (ndcbits_old*2+naddbits);
//        // extract the intermediate bits that will need to be encoded as ternary bits
//        unsigned int bitsToConvert = (oldss>>(ndcbits_old*2)) & (~(~0<<naddbits));
//        // extract existing ternary bits, they don't need any conversion
//        unsigned int oldSS_Tern = oldss & (~(~0<<(ndcbits_old*2)));

//        // variable with the converted additional bits
//        unsigned int bitsConverted(0);
//        for (unsigned int ibit=0; ibit!=naddbits; ++ibit) { // bit loop
//            unsigned int val = (bitsToConvert&(1<<ibit)) ? 2 : 1; // 0 becomes 01 (1), 1 becomes 10 (2)
//            bitsConverted |= val << (ibit*2);
//        } // end bit loop

//        // compose the output SS from the segments: oldSS_H | bitsConverted | oldSS_Tern
//        return oldSS_H << (ndcbits_new*2) | bitsConverted << (ndcbits_old*2) | oldSS_Tern;
//    }
//    else { // here ndcbits_old>ndcbits_new
//        // decrease the number of ternary bits, with a potential loss of information

//        // calculate the number of bits to be converted
//        unsigned int nconvbits(ndcbits_old-ndcbits_new);

//        // extract the most-significant bits, standard representation
//        unsigned int oldSS_H = oldss>>(ndcbits_old*2);
//        // extract the least-significant bits, ternary encoded, to be left as they are
//        unsigned int oldSS_L = oldss&((1<<(ndcbits_new*2))-1);
//        /* extract the ternary bits that need to be converted as standard bits.
//         * The range is between the new and old number of ternary bits
//         */
//        unsigned int bitsToConvert = (oldss >> (ndcbits_new*2)) & (~(0<<(nconvbits*2)));

//        unsigned int convertedBits(0); // converted bits
//        for (unsigned int ibit=0; ibit!=nconvbits; ++ibit) { // loop over the bits to be converted
//            // extract the ternary bits
//            const unsigned int ternval = (bitsToConvert>>(2*ibit)) & 3;
//            // if the value is 0 or 1 (X or 1) the binary value is 0 (nothing to do)
//            // otherwise 1 is set
//            if (ternval==2) convertedBits |= 1<<ibit;
//        } // end loop over the bits to be converted

//        return (oldSS_H<<(nconvbits+ndcbits_new*2)) | (convertedBits<<(ndcbits_new*2)) | oldSS_L;
//    }
//}

///**
// * Return a word of N bits that represents the DC value of a SS. The SS
// * is assumed to use ternary encoding for the least-significant N bits.
// */
//unsigned int FTKPatternBank::extractDCBits(unsigned int SS, unsigned int ndcbits)
//{
//    // create the return value
//    unsigned int res(0);

//    for (unsigned int ibit=0; ibit!=ndcbits; ++ibit) { // loop over the LS ternary bits
//        // extract the ternary value of interest
//        const int ternval = (SS>>(ibit*2)) & 3;
//        // check the ternary value and eventually set the flasg in the output word
//        if (ternval==0) res |= 1<<ibit;
//    } // end loop over the LS ternary bits

//    return res;
//}

///** This is the main method to load a pattern bank file. The three parameters are the
// * file path, the file type and and desired number of patterns. The method can check the exact
// * format of the pattern bank and decide how to read it.
// *
// * The allowed file types are:
// * * 0 offline cache format, use ROOT. Internally uses loadPatternBankROOTCache().
// * * 1 offline ASCII format, both compressed or not. Internally uses loadPatternBankAscii().
// *
// * The method retruns
// * * 0 if succeeds,
// * * -1 for a generic error
// * * -2 if the number of requested patterns cannot be loaded
// * * -3 if the format cannot be read
// *
// * For specific information please refer to the documentation of the
// */
//int FTKPatternBank::loadPatternBank(const char *filename, unsigned int type, unsigned int npatterns, bool debug)
//{
//    if (m_isOK){
//        //bank is already loaded
//        cout << "Pattern Bank Already Loaded to SBC memory" << endl;
//        return 0;
//    }

//    // read the file
//    int res(-1);
//    switch (type) {
//        case 0:
//            if(debug) cout << "Using loadPatternBankROOTCache" << endl;
//            res = loadPatternBankROOTCache(filename, npatterns, debug); // custom root-only format (flat tree)
//            break;
//        case 1:
//            if(debug) cout << "Using loadPatternBankAscii" << endl;
//            res = loadPatternBankAscii(filename, npatterns);
//            break;
//        default:
//            break;
//    }

//    // in case res is ok the OK state is enabled
//    if (res==0) {
//        m_isOK = true;
//        // call the checksum set
//        calculateChecksum();
//    }

//    m_bankname = filename;

//    return res;
//}

//int FTKPatternBank::loadPatternBankDCconfig(const char *filename, unsigned int type, bool debug)
//{
//    // read the file
//    int res(-1);
//    switch (type) {
//        case 0:
//            if(debug) cout << "Using loadPatternBankDCconfigROOTCache " << endl;
//            res = loadPatternBankDCconfigROOTCache(filename, debug); // custom root-only format (flat tree)
//            break;
//        case 1:
//            if(debug) cout << "ASCI format not yet supported for DCbit config reading!" << endl;
//            break;
//        default:
//            break;
//    }

//    return res;
//}

///** This method reads N patterns from an ASCII file representing the pattern bank.
// *
// * The ASCII file starts with an header containing 2 values: number of patterns, number of layers.
// * The values in the header are used to verify the compatibility with the user request:
// * if there aren't enough patterns an is returned -2, if the number of layers is not
// * compatible with the object setup -3 is returned.
// *
// * This bank format not usually contains DC bits but the 0 value in each layer
// * is not enforced. This require the user to pay attention to the setup value.
// */
//int FTKPatternBank::loadPatternBankAscii(std::string filename, unsigned int npatterns)
//{
//    cout << "loading ASCII pattern bank" << endl;
//    // prepare the filtering object using the BOOST library
//    boost::iostreams::filtering_istream pattfile;

//    // check if the filename indicates a bzip2 compressed file
//    if (filename.find("bz2")!=string::npos) {
//        pattfile.push( boost::iostreams::bzip2_decompressor() );
//    }
//    // check if the filename suggests a gzip compressed file
//    else if (filename.find("gz")!=string::npos) {
//        pattfile.push( boost::iostreams::gzip_decompressor() );
//    }

//    // create the file source object
//    boost::iostreams::file_source ifile(filename.c_str()); // open the file
//    if (!ifile.is_open()) return false;
//    pattfile.push(ifile);

//    // read the pattern bank header
//    unsigned int cur_npatterns, cur_nlayers;
//    pattfile >> cur_npatterns >> cur_nlayers;

//    cout << "Reading " << npatterns << "/" << cur_npatterns << " for " << cur_nlayers << " layers" << endl;
//    if (cur_npatterns<npatterns) {
//        cerr << "*** Not enough patterns" << endl;
//        return -2;
//    }
//    if (cur_nlayers!=m_nlayers) {
//        cerr << "*** Not matching number of layers " << cur_nlayers << "!=" << m_nlayers << endl;
//        return -3;
//    }

//    // set the number of patterns to be read
//    m_npatterns = npatterns;
//    // allocate the memory required to read the pattern bank
//    m_patterns = std::vector<uint32_t>(m_npatterns*m_nlayers);


//    m_sectorsID = std::vector<uint32_t>(m_npatterns);


//    for (unsigned int ipatt=0;ipatt!=m_npatterns;++ipatt) { // loop over the patterns
//        // temporary variables
//        unsigned int curpattid, curss, cursector, curcoverage;

//        pattfile >> curpattid; // read the pattern ID
//        if (curpattid!=ipatt) return -3;

//        for (unsigned int il=0; il!=m_nlayers; ++il) { // loop over the layers
//            pattfile >> curss; // read a SS from the file
//            m_patterns[_SSPos(ipatt,il)] = curss; // set the SS into the bank
//        } // end loop over the layers
//        // read the sector ID and the coverage, both variables aren't used
//        pattfile >> cursector >> curcoverage;

//        m_sectorsID[ipatt] = cursector; // store the sectors ID
//    } // end loop over the patterns
//    return 0;
//}

///** The method loads a ROOT file containg an offline cache, in a flat TTree.
// * The ROOT file contains a TTree named SSMap, which is accessed to get the DCbit
// * configuration for the pbank object. If the TTree is found the function and the
// * DCbit configuration set, it returns (0). If the TTree is not found, a default
// * DCbit configuration of [3,3,3,3,3,3,3,3] is used, and it returns (-1). If none
// * of the above, it returns (-2).
// */
//int FTKPatternBank::loadPatternBankDCconfigROOTCache(std::string filename, bool debug = false)
//{
//    // open the cache root file
//    TFile *rootcache = TFile::Open(filename.c_str());

//    // try to get the SSMap TTree
//    TTree *ssmap = dynamic_cast<TTree*>(rootcache->Get("SSMap"));
//    if (ssmap) {
//        // If the tree exists the information will be read
//        cout << "Retrieving the DC configuration from the file" << endl;

//        // connect the branches that are useful in this case
//        int ndcx, ndcy;
//        if(debug) cout << "Getting ndcx" << endl;
//        ssmap->SetBranchAddress("ndcx", &ndcx);
//        if(debug) cout << "Getting ndcy" << endl;
//        ssmap->SetBranchAddress("ndcy", &ndcy);

//        for (unsigned int il=0; il!=m_nlayers; ++il) { // loop over the layers
//            // each layer is an entry of the TTree
//            if(debug) cout << "Getting Layer " << il << endl;
//            ssmap->GetEntry(il);

//            // set the number of the DC bits for the layer
//            // to interpret the patterns only the sum is important
//            // at this stage
//            m_bankdcbits[il] = ndcx+ndcy;
//            if(m_chipdcbits[il]!=m_bankdcbits[il]) m_chipdcbits[il]=m_bankdcbits[il];
//        }
//        rootcache->Close();
//        return 0;
//    }
//    else{
//        cout << "No SSMap TTree. List File Contents and set default DCbit configuration [3,3,3,3,3,3,3]" << endl;
//        rootcache->ls();

//        for (unsigned int il=0; il!=m_nlayers; ++il) { // loop over the layers
//            // each layer is an entry of the TTree
//            if(debug) cout << "Getting Layer " << il << endl;
//            // set the number of the DC bits for the layer
//            m_bankdcbits[il] = 3;
//            if(m_chipdcbits[il]!=m_bankdcbits[il]) m_chipdcbits[il]=m_bankdcbits[il];
//        }
//        rootcache->Close();
//        return -2;
//    }

//    rootcache->Close();
//    return -1;

//}

///** The method loads a ROOT file containg an offline cache, in a flat TTree.
// * The ROOT file contains a TTree named TTree. As for the ASCII format if the
// * number of requested patterns is larger than the number of patterns in the
// * bank the method return -2, if the number of layers in the bank doesn't
// * match the object setup -3 is returned.
// *
// * The file is expected to describe the number of DC bits contained in the
// * bank, otherwise a default value is set (current deault: [2,2,2,1,1,1,1).
// */
//int FTKPatternBank::loadPatternBankROOTCache(std::string filename, unsigned int npatterns, bool debug = false)
//{
//    // open the cache root file
//    TFile *rootcache = TFile::Open(filename.c_str());

//    // Create the TTree with 1 branch: the list of the FTK patterns (AM level)
//    TTree *amtree = dynamic_cast<TTree *>(rootcache->Get("Bank"));
//    if (!amtree) {
//        if(debug) cout << "Failed to find Bank TTree" << endl;
//        rootcache->Close();
//        return -1;
//    }

//    // try to get the SSMap TTree
//    TTree *ssmap = dynamic_cast<TTree*>(rootcache->Get("SSMap"));
//    if (ssmap) {
//        // If the tree exists the information will be read
//        cout << "Retrieving the DC configuration from the file" << endl;

//        // connect the branches that are useful in this case
//        int ndcx, ndcy;
//        if(debug) cout << "Getting ndcx" << endl;
//        ssmap->SetBranchAddress("ndcx", &ndcx);
//        if(debug) cout << "Getting ndcy" << endl;
//        ssmap->SetBranchAddress("ndcy", &ndcy);

//        for (unsigned int il=0; il!=m_nlayers; ++il) { // loop over the layers
//            // each layer is an entry of the TTree
//            if(debug) cout << "Getting Layer " << il << endl;
//            ssmap->GetEntry(il);

//            // set the number of the DC bits for the layer
//            // to interpret the patterns only the sum is important
//            // at this stage
//            m_bankdcbits[il] = ndcx+ndcy;
//            if(m_chipdcbits[il]!=m_bankdcbits[il]) m_chipdcbits[il]=m_bankdcbits[il];
//        }
//    }
//    else{
//        cout << "WARNING: No SSMap TTree. List File Contents" << endl;
//        rootcache->ls();
//    }

//    unsigned int cur_npatterns = amtree->GetEntries();
//    if(cur_npatterns == 0) cout << "ERROR: No entries in Bank TTree " << filename << endl;
//    cout << "Reading " << npatterns << "/" << cur_npatterns << endl;
//    if (cur_npatterns<npatterns) {
//        cerr << "*** Not enough patterns" << endl;
//        cout << "*** Not enough patterns" << endl;
//        rootcache->Close();
//        return -2;
//    }

//    //int patternID; // pattern identification
//    int nplanes; // number of planes
//    int ssid[8]; //[m_nplanes] SS id on each plane
//    int sectorID; // sector containing this pattern
//    //int coverage; // number of events that created this pattern
//    int dcmask; // DC mask for this pattern
//    int hbmask; // half-bin mask

//    if(debug) cout << "Getting nplanes, ssid, sectorID, dcmask, hbmask" << endl;
//    //amtree->SetBranchAddress("patternID", &patternID);
//    amtree->SetBranchAddress("nplanes", &nplanes);
//    amtree->SetBranchAddress("ssid", &ssid);
//    //amtree->SetBranchAddress("ndc", &ndc);
//    amtree->SetBranchAddress("sectorID", &sectorID);
//    //amtree->SetBranchAddress("coverage", &coverage);
//    amtree->SetBranchAddress("dcmask", &dcmask);
//    amtree->SetBranchAddress("hbmask", &hbmask);

//    // uset the first pattern to setup global variables
//    if(debug) cout << "Getting amtree entry 0" << endl;
//    amtree->GetEntry(0);
//    unsigned int cur_nlayers = nplanes;

//    if (cur_nlayers!=m_nlayers) {
//        cerr << "*** Not matching number of layers " << cur_nlayers << "!=" << m_nlayers << endl;
//        cout << "*** Not matching number of layers " << cur_nlayers << "!=" << m_nlayers << endl;
//        rootcache->Close();
//        return -3;
//    }

//    // set the number of patterns to be read
//    m_npatterns = npatterns;

//    // allocate the memory required to read the pattern bank
//    m_patterns = std::vector<uint32_t>(m_npatterns*m_nlayers);

//    m_sectorsID = std::vector<uint32_t>(m_npatterns);

//    if(debug) cout << "Looping over patterns: 0 to " << m_npatterns <<endl;
//    for (unsigned int ipatt=0;ipatt!=m_npatterns;++ipatt) { // loop over the patterns
//        if(debug) cout << "Reading pattern: " << ipatt << endl;
//        // get the pattern
//        amtree->GetEntry(ipatt);
//        if(ipatt%(m_npatterns/10) == 0) cout << "loading pattern: " << ipatt << "/" << m_npatterns << endl;
//        //pattfile >> curpattid; // read the pattern ID
//        //if (patternID!=ipatt) return -3;

//        int shift(0);
//        if(debug) cout << "Looping over layers: 0 to " << m_nlayers << endl;
//        for (unsigned int il=0; il!=m_nlayers; ++il) { // loop over the layers
//            // The MSB of the SS, related to the low precision, can be read
//            // as they are from the ssid stored in the TTree. To make space
//            // for the LSB bits, as ternary bits, they need to be moved
//            // right in the final word
//            if(debug) cout << "Layer: "<< il <<" Getting curss" << endl;
//            int curss = ssid[il]<<m_bankdcbits[il]*2;

//            // create the bitmask used to extract information from
//            // global words
//            if(debug) cout << "Getting mask" << endl;
//            int mask = (1<<m_bankdcbits[il])-1;

//            // extract the half-bin and DC bits portions for the current
//            // layer from the global words
//            int hb = (hbmask>>shift) & mask;
//            int dc = (dcmask>>shift) & mask;

//            // combine the HB and DC values to
//            if(debug) cout << "Looping over bits: 0 to " << m_bankdcbits[il] << endl;
//            for (unsigned int ibit=0; ibit!=m_bankdcbits[il]; ++ibit) { // bit loop
//                int val(0);
//                if (dc&(1<<ibit)) val = 0; // Enable the DC for this bit, 00
//                else val = (hb&(1<<ibit)) ? 2 : 1; // 0 becomes 01 (1), 1 becomes 10 (2)
//                curss |= val << (ibit*2); // update part of the LSB of the curss word
//            } // end bit loop
//            m_patterns[_SSPos(ipatt,il)] = curss; // set the SS into the bank

//            // incrememt the shift used to extract the HB and DC information
//            shift += m_bankdcbits[il];
//        } // end loop over the layers

//        m_sectorsID[ipatt] = sectorID; // store the sector ID of the current pattern
//    } // end loop over the patterns

//    rootcache->Close();
//    return 0;
//}


///** Create a random pattern bank of N patterns according a specific mode.
// * The modes are not yet defined
// */
//int FTKPatternBank::createRandomBank(unsigned int npatterns, unsigned int mode)
//{
//    m_npatterns = npatterns;

//    m_patterns =std::vector<uint32_t>(m_npatterns*m_nlayers);

//    m_sectorsID = std::vector<uint32_t>(m_npatterns);

//    if (mode==0) {
//        // random bank
//        for (unsigned int ipatt=0; ipatt!=m_npatterns; ++ipatt) { //loop over the patterns
//            for (unsigned int il=0; il!=m_nlayers; ++il) { // layer loop
//                m_patterns[_SSPos(ipatt,il)] = gRandom->Integer(1<<15); //TODO is not correctly setting the DC bits
//            } // end layer loop
//        } // end loop over the patterns
//    }
//    else {
//        m_npatterns = 0;
//        m_patterns.clear();
//        return -1;
//    }

//    // call the checksum set
//    calculateChecksum();

//    // set the OK flag
//    m_isOK = true;
//    m_bankname = "Random";

//    return 0;
//}

///** Create a bank with equal SS and progressive SS in each patterns.
// *
// * In this bank the patterns have equivalent SS in all layers and the SS
// * increment by 1 unit from pattern N to N+1:
// *   Patt0 0 0 0 0 0...
// *   Patt1 1 1 1 1 1...
// *
// * Because the number of bits is limited a maximum value after which
// * the counter restart has to be set.
// */
//int FTKPatternBank::createSequentialBank(unsigned int npatterns, unsigned int maxSS) {
//    // allocate the memory
//    m_npatterns = npatterns;

//    m_patterns = std::vector<uint32_t>(m_npatterns*m_nlayers);

//    m_sectorsID = std::vector<uint32_t>(m_npatterns);

//    for (unsigned int ipatt=0; ipatt!=m_npatterns; ++ipatt) { //loop over the patterns
//        for (unsigned int il=0; il!=m_nlayers; ++il) { // layer loop
//            m_patterns[_SSPos(ipatt,il)] = ipatt%maxSS;
//        } // end layer loop
//        m_sectorsID[ipatt] = ipatt%maxSS;
//    } // end loop over the patterns


//    // call the checksum set
//    calculateChecksum();

//    // set the OK flag
//    m_isOK = true;
//    // create the bank tag
//    ostringstream otag;
//    otag << "seqbank" << npatterns << "m" << maxSS;
//    m_bankname = otag.str();

//    return 0;

//}


///** This method overwrite the pattern bank content to ensure a special pattern
// * content that can be replicated at regular distance.
// *
// * The pattern bank will probably know that this is a special pattern but
// * the SS value is forced, to be in agreement with online configuration.
// */
//void FTKPatternBank::overrideSpecialPattern(unsigned int ss, unsigned int pattstep, unsigned int firstpatt)
//{
//    for (unsigned int ipatt=firstpatt; ipatt<m_npatterns; ipatt+=pattstep) { //loop over the patterns
//        for (unsigned int i=0;i!=m_nlayers;++i) { // loop over the layers
//            /* The SS value is encoded using the DC bits, converting the number
//             * according the DC setup for the bank, to be compatible with all the
//             * other SS values.
//             */
//            const unsigned int realss = convertSSAdjustTernaryBits(ss,0, m_bankdcbits[i]);
//            m_patterns[_SSPos(ipatt,i)] = realss;
//        } // end loop over the layers
//    } // end loop over the patterns
//}


///** The method copies the content of a pattern into the memory allocated
// * in unsigned int* pattern.
// * During the copy the pattern's description is translated to the
// * representation required by the chip configuration, in particular
// * the ternary bit encoding is used for all the number of DC bits
// * set in the patterns, this can be greater of the numebr of DC bits
// * used in the original bank.
// * It returns 0 in case of suceess, -1 otherwise. Cannot currently fail.
// */
//int FTKPatternBank::getPattern(unsigned int pattid,std::vector< uint32_t> &pattern) const
//{
//    for (unsigned int i=0;i!=m_nlayers;++i) {
//        /* When the SS is converted for the chip an offset to the value
//         * can be requested. This offset is assumed to have an effect
//         * only in the normal bits, beyond the DC bits content.
//         * In case DC bits are already encoded in the SS of the bank,
//         * the offset need to shifted right to account for the fact that
//         * each DC bit uses 2 real bits. This avoid to confuse a numeric
//         * shift with the ternary bits.
//         */
//        pattern[i] = convertSSAdjustTernaryBits(m_patterns[_SSPos(pattid,i)]+ (m_ssoffset[i] << m_bankdcbits[i]), m_bankdcbits[i], m_chipdcbits[i]);
//    }

//    return 0;
//}


///**
// * This method return the content as set of SS, full precision, and DC bits
// * of a given pattern.
// */
//int FTKPatternBank::getPatternAndDC(unsigned int pattid, std::vector<uint32_t> &pattern, std::vector<uint32_t> &dcbits) const
//{
//    for (unsigned int i=0;i!=m_nlayers;++i) {
//        // As in the ::getPattern() if requested an offset is added to the SS value
//        pattern[i] = convertSSAdjustTernaryBits(m_patterns[_SSPos(pattid,i)]+(m_ssoffset[i] << m_bankdcbits[i]), m_bankdcbits[i], 0);
//        dcbits[i] = extractDCBits(m_patterns[_SSPos(pattid,i)], m_bankdcbits[i]);
//    }
//    return 0;
//}


///**
// * This method returns the information about a single pattern according the
// * needs of the AUX card.
// *
// * It returns a negative value if an error occours.
// */
//int FTKPatternBank::getPatternAUX(unsigned int pattid, std::vector<uint32_t> &pattern, std::vector<uint32_t> &ssid, std::vector<uint32_t> &dc, unsigned int &sectorID, bool loadAllSectors, unsigned int lamb) const
//{
//    if(loadAllSectors)
//        sectorID = m_sectorsID[pattid]; // for testing purposes, load all sectors to all processors. do not bitshift in this case
//    else{
//        if(lamb < 4){
//            if(m_sectorsID[pattid]%4!=lamb) cout << "ERROR: Pattern has wrong sector ID for this lamb." << endl;
//        }
//        sectorID = m_sectorsID[pattid]>>2; // the sectors are stored cyclically for the four processors.
//    }

//    for (unsigned int i=0; i!=m_nlayers; ++i) { // loop over the layers
//        unsigned int currss = convertSSAdjustTernaryBits(m_patterns[_SSPos(pattid,i)]+(m_ssoffset[i] << m_bankdcbits[i]), m_bankdcbits[i], 0);
//        unsigned int dcbits = extractDCBits(m_patterns[_SSPos(pattid,i)], m_bankdcbits[i]);

//        ssid[i] = currss;//full precission SSID
//        dc[i] = dcbits;  //Bitwise mask. 1 if corresponding LSB of curss is ternary, 0 if regular bit.

//        //The AUX can only store 144 bits per pattern, 16 bits are used to store the sector ID leaving 128 bits for the pattern.
//        //   -> Allowing for a maximum of 3 ternary bits requires 3^3=27 possibilities to be encoded per layer.
//        //   -> Each full precision SSID is 14 bits.
//        //   -> 8*14{SSID bits} + 8*3{ternary bit mask} = 136 > 128!!!
//        //To solve this, the aux uses the three lsb of each SSID and two extra bits per layer for a total of 5 bits to encode the 27 ternary possibilites
//        //   -> 8*(14-3){11 MSB of SSID} + 8*5{Encoded ternary bits} = 128 which saturates the External memory word width. (This was an accident and very lucky...)
//        //The encoding of the ternary bits is done with a ROM on each processor FPGA which is structured as follows:
//        //   0: 000
//        //   1: 001
//        //   2: 00X
//        //   3: 010
//        //   4: 011
//        //   5: 01X
//        //   6: 0X0
//        //   7: 0X1
//        //   etc.  (X means the bit is allowed to be either 1 or 0, effectively ignoring the bit. Hence the term "Don't Care" bit)
//        int ternaryBaseThree0 = (dcbits>>0)&1 ? 2 : (currss>>0)&1;
//        int ternaryBaseThree1 = (dcbits>>1)&1 ? 2 : (currss>>1)&1;
//        int ternaryBaseThree2 = (dcbits>>2)&1 ? 2 : (currss>>2)&1;
//        int encodedTernaryBits = (ternaryBaseThree0 + 3*ternaryBaseThree1 + 9*ternaryBaseThree2) & 0x1f; // 0x1f = 0x11111
//        //int thisLayer = (currss>>3)<<5 | encodedTernaryBits;
//        int thisLayer = (currss>>3)<<5 | encodedTernaryBits;
//        //cout << "dcbits: " << dcbits << endl;
//        //cout << "currss: " << currss << endl;
//        //cout << "ternaryBaseThree: " << ternaryBaseThree2 << ternaryBaseThree1 << ternaryBaseThree0 << endl;
//        //cout << "encoded: " << encodedTernaryBits << endl;
//        //cout << "thisLayer:" << thisLayer << endl;
//        pattern[i] = thisLayer;
//    } // end loop over the layers
//    return 0;
//}


///** Clear the memory allocated to represent the patterns. All the other parameters
// * (checksum, DC configuration, ...) are not removed.
// */
//void FTKPatternBank::clearBank() {
//    m_patterns.clear();
//    vector<uint32_t>().swap(m_patterns);
//}

//void FTKPatternBank::clearAll() {
//    m_patterns.clear();
//    vector<uint32_t>().swap(m_patterns);

//    m_bankdcbits.clear();
//    vector<uint32_t>().swap(m_bankdcbits);

//    m_chipdcbits.clear();
//    vector<uint32_t>().swap(m_chipdcbits);

//    m_sectorsID.clear();
//    vector<uint32_t>().swap(m_sectorsID);

//    m_ssoffset.clear();
//    vector<uint32_t>().swap(m_ssoffset);
//}

///** Save the bank in a commonly used format storing pattern in a ASCII table, with a exadecimal
// * counters for the pattern and the SS.
// */
//void FTKPatternBank::dumpPatternBank(const char *path) {
//    // open the file name
//    ofstream outfile(path);

//    // check if the status allow to be stored
//    if (!m_isOK) {
//        cerr << "*** The bank is not in a good condition and cannot be saved" << endl;
//        return;
//    }


//    // to retrieve a pattern memory has to be allocated
//    std::vector<uint32_t> curpatt = std::vector<uint32_t>(m_nlayers);

//    for (unsigned int ipatt=0; ipatt!=m_npatterns; ++ipatt) { // loop over the patterns
//        // retrieve the patterns
//        getPattern(ipatt,curpatt);

//        outfile << hex << setw(5) << ipatt ;
//        for (unsigned int ilayer=0; ilayer!=m_nlayers; ++ilayer) { // loop over the layers
//            outfile << "\t" << setw(5) << curpatt[ilayer];
//        } // end loop over the layers
//        outfile << dec << endl;

//    } // end loop over the patterns

//    // release the memory for the pattern

//    // close the file
//    outfile.close();
//}

///** The method calculates the Adler checksum on the bank content.
//*/
//void FTKPatternBank::calculateChecksum() {
//    if (!m_isOK) {
//        m_checksum = 0;
//        return;
//    }

//    m_checksum = 1; // initialize the checksum to 1, default initialization

//    const unsigned int ndata = m_npatterns*m_nlayers;

//    updateChecksum(ndata,m_patterns); // update the checksum based on the SS content
//}

///** The method updates the Adler checksum based on external data. */
//void FTKPatternBank::updateChecksum(const unsigned int ndata, const std::vector<uint32_t> &data) {
//    //Calculate ADLER checksum, check on wikipedia

//    const unsigned int ADLER_NUM = 65521;

//    unsigned int sum1 = m_checksum & 0xffff; // this represents the LSB
//    unsigned int sum2 = m_checksum>>16; // represents the MSB

//    for (unsigned int i=0; i!=ndata; i++) {
//        const unsigned int &word = data[i];

//        sum1 = ((sum1 + word) % ADLER_NUM);
//        sum2 = ((sum2 + sum1) & ADLER_NUM);
//    }

//    m_checksum = (sum2 << 16) | sum1;

//}

///** Helper function that converts a pattern number into a road ID */
//unsigned int FTKPatternBank::getHWRoadID(unsigned int id, unsigned int nPatternsPerChip, unsigned int offset) {
//    return ((id/nPatternsPerChip)<<17) | ((id%nPatternsPerChip)+offset);
//}
