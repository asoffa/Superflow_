// Superflow.cxx
//

// std
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <random>
#include <fstream>

// Superflow
#include "Superflow/Superflow.h"
#include "Superflow/StringTools.h"
#include "Superflow/PhysicsTools.h"

using namespace std;

namespace sflow {
    LeptonVector scratchLeptonVector = {};

    ////////////////////////////////////////////
    // Constructor
    ////////////////////////////////////////////
    Superflow::Superflow()
    {
        app_name = "Superflow    ";
        m_dbg = true;
        setSelectTaus(true);

        m_runMode = SuperflowRunMode::null;

        /////////////////////////
        // SusyNtuple/Trigger 
        /////////////////////////

        m_CutStore_Name_Exists = false;
        m_CutStoreUntitled     = 1;

        m_countWeights = true;

        m_outputFileName      = "";
        m_outputFileNameSuffix = "";
        m_entry_list_FileName = "entrylist_";
        m_tree_name_auto      = "";
        m_outputFile          = nullptr;
        m_entryListFile       = nullptr;
        m_HFT                 = nullptr;

        m_output_array = nullptr;

        m_HFT_array = nullptr;

        m_entry_list_total       = nullptr;
        m_entry_list_single_tree = nullptr;

        m_period = ATLAS_period::null;
        m_stream = ATLAS_stream::null;

        m_varState = SupervarState::closed;
        m_superVar_hasFunction = false;
        m_superVar_hasNiceName = false;
        m_superVar_hasHFTName  = false;
        m_superVar_Untitled    = 1;

        m_mcWeighter = nullptr;

        m_nullExprFloat  = [](Superlink* sl, var_float*) -> double { return 0.0; };
        m_nullExprFloatArray = [](Superlink* sl, var_float_array*) -> vector<double> {
                vector<double> null;
                for(int i = 0; i < 25; i++) {
                    null.push_back(0.0);
                }
                return null;
        };
        m_nullExprDouble = [](Superlink* sl, var_double*) -> double { return 0.0; };
        m_nullExprInt    = [](Superlink* sl, var_int*) -> int { return 0; };
        m_nullExprBool   = [](Superlink* sl, var_bool*) -> bool { return false; };
        m_nullExprVoid   = [](Superlink* sl, var_void*) {};

        m_varFloat  = nullptr;
        //m_varFloatArray = nullptr;
        m_varDouble = nullptr;
        m_varInt    = nullptr;
        m_varBool   = nullptr;

        m_varFloatArray_array.clear();
        m_varBoolArray_array.clear();
        m_varFloat_array  = nullptr;
        m_varDouble_array = nullptr;
        m_varInt_array    = nullptr;
        m_varBool_array   = nullptr;

        m_sysState = SupersysState::closed;


        m_sysTemplate.reset();
        m_sys_hasNiceName   = false;
        m_sys_hasTreeName   = false;
        m_sys_hasType       = false;
        m_sys_hasSystematic = false;

        m_singleEventSyst = NtSys::NOM;
        m_RunSyst         = new Supersys(SupersysType::central);

        m_tree_leafs_size    = 0;
        m_weight_leaf_offset = 0;

        //m_trigObj = nullptr;

        m_input_chain = nullptr;

        m_data_stream2string[ATLAS_stream::Main] = "physics_Main";

        m_NtSys_to_string[Susy::NtSys::EG_RESOLUTION_ALL_DN]        = Susy::NtSys::SusyNtSysNames[EG_RESOLUTION_ALL_DN];//"EG_RESOLUTION_ALL_DN";
        m_NtSys_to_string[Susy::NtSys::EG_RESOLUTION_ALL_UP]        = Susy::NtSys::SusyNtSysNames[Susy::NtSys::EG_RESOLUTION_ALL_UP];//"EG_RESOLUTION_ALL_UP";
        m_NtSys_to_string[Susy::NtSys::EG_SCALE_ALL_DN]             = Susy::NtSys::SusyNtSysNames[Susy::NtSys::EG_SCALE_ALL_DN];//"EG_SCALE_ALL_DN"; 
        m_NtSys_to_string[Susy::NtSys::EG_SCALE_ALL_UP]             = Susy::NtSys::SusyNtSysNames[Susy::NtSys::EG_SCALE_ALL_UP];//"EG_SCALE_ALL_UP"; 
        m_NtSys_to_string[Susy::NtSys::JET_GroupedNP_1_UP]          = Susy::NtSys::SusyNtSysNames[Susy::NtSys::JET_GroupedNP_1_UP];//"JET_GroupedNP_1_UP"; 
        m_NtSys_to_string[Susy::NtSys::JET_GroupedNP_1_DN]          = Susy::NtSys::SusyNtSysNames[Susy::NtSys::JET_GroupedNP_1_DN];//"JET_GroupedNP_1_DN"; 
        m_NtSys_to_string[Susy::NtSys::JET_GroupedNP_2_UP]          = Susy::NtSys::SusyNtSysNames[Susy::NtSys::JET_GroupedNP_2_UP];//"JET_GroupedNP_2_UP"; 
        m_NtSys_to_string[Susy::NtSys::JET_GroupedNP_2_DN]          = Susy::NtSys::SusyNtSysNames[Susy::NtSys::JET_GroupedNP_2_DN];//"JET_GroupedNP_2_DN"; 
        m_NtSys_to_string[Susy::NtSys::JET_GroupedNP_3_UP]          = Susy::NtSys::SusyNtSysNames[Susy::NtSys::JET_GroupedNP_3_UP];//"JET_GroupedNP_3_UP";
        m_NtSys_to_string[Susy::NtSys::JET_GroupedNP_3_DN]          = Susy::NtSys::SusyNtSysNames[Susy::NtSys::JET_GroupedNP_3_DN];//"JET_GroupedNP_3_DN";
        m_NtSys_to_string[Susy::NtSys::JER]                         = Susy::NtSys::SusyNtSysNames[Susy::NtSys::JER];//"JER"; 
        m_NtSys_to_string[Susy::NtSys::MET_SoftTrk_ResoPara]        = Susy::NtSys::SusyNtSysNames[Susy::NtSys::MET_SoftTrk_ResoPara];//"MET_SoftTrk_ResoPara";
        m_NtSys_to_string[Susy::NtSys::MET_SoftTrk_ResoPerp]        = Susy::NtSys::SusyNtSysNames[Susy::NtSys::MET_SoftTrk_ResoPerp];//"MET_SoftTrk_ResoPerp"; 
        m_NtSys_to_string[Susy::NtSys::MET_SoftTrk_ScaleDown]       = Susy::NtSys::SusyNtSysNames[Susy::NtSys::MET_SoftTrk_ScaleDown];//"MET_SoftTrk_ScaleDown"; 
        m_NtSys_to_string[Susy::NtSys::MET_SoftTrk_ScaleUp]         = Susy::NtSys::SusyNtSysNames[Susy::NtSys::MET_SoftTrk_ScaleUp];//"MET_SoftTrk_ScaleUp"; 
        m_NtSys_to_string[Susy::NtSys::MUON_ID_DN]                  = Susy::NtSys::SusyNtSysNames[Susy::NtSys::MUON_ID_DN];//"MUON_ID_DN"; 
        m_NtSys_to_string[Susy::NtSys::MUON_ID_UP]                  = Susy::NtSys::SusyNtSysNames[Susy::NtSys::MUON_ID_UP];//"MUON_ID_UP"; 
        m_NtSys_to_string[Susy::NtSys::MUON_MS_DN]                  = Susy::NtSys::SusyNtSysNames[Susy::NtSys::MUON_MS_DN];//"MUON_MS_DN";
        m_NtSys_to_string[Susy::NtSys::MUON_MS_UP]                  = Susy::NtSys::SusyNtSysNames[Susy::NtSys::MUON_MS_UP];//"MUON_MS_UP";
        m_NtSys_to_string[Susy::NtSys::MUON_SCALE_DN]               = Susy::NtSys::SusyNtSysNames[Susy::NtSys::MUON_SCALE_DN];//"MUON_SCALE_DN";
        m_NtSys_to_string[Susy::NtSys::MUON_SCALE_UP]               = Susy::NtSys::SusyNtSysNames[Susy::NtSys::MUON_SCALE_UP];//"MUON_SCALE_UP";
/*
        m_NtSys_to_string[Susy::NtSys::EES_Z_UP]     = "EESZUP";
        m_NtSys_to_string[Susy::NtSys::EES_Z_DN]     = "EESZDOWN";
        m_NtSys_to_string[Susy::NtSys::EES_MAT_UP]   = "EESMATUP";
        m_NtSys_to_string[Susy::NtSys::EES_MAT_DN]   = "EESMATDOWN";
        m_NtSys_to_string[Susy::NtSys::EES_PS_UP]    = "EESPSUP";
        m_NtSys_to_string[Susy::NtSys::EES_PS_DN]    = "EESPSDOWN";
        m_NtSys_to_string[Susy::NtSys::EES_LOW_UP]   = "EESLOWUP";
        m_NtSys_to_string[Susy::NtSys::EES_LOW_DN]   = "EESLOWDOWN";
        m_NtSys_to_string[Susy::NtSys::EER_UP]       = "EERUP";
        m_NtSys_to_string[Susy::NtSys::EER_DN]       = "EERDOWN";
        m_NtSys_to_string[Susy::NtSys::MS_UP]        = "MSUP";
        m_NtSys_to_string[Susy::NtSys::MS_DN]        = "MSDOWN";
        m_NtSys_to_string[Susy::NtSys::ID_UP]        = "IDUP";
        m_NtSys_to_string[Susy::NtSys::ID_DN]        = "IDDOWN";
        m_NtSys_to_string[Susy::NtSys::JES_UP]       = "JESUP";
        m_NtSys_to_string[Susy::NtSys::JES_DN]       = "JESDOWN";
        m_NtSys_to_string[Susy::NtSys::JER]          = "JER";
        m_NtSys_to_string[Susy::NtSys::SCALEST_UP]   = "SCALESTUP";
        m_NtSys_to_string[Susy::NtSys::SCALEST_DN]   = "SCALESTDOWN";
        m_NtSys_to_string[Susy::NtSys::RESOST]       = "RESOST";
        m_NtSys_to_string[Susy::NtSys::TRIGSF_EL_UP] = "TRIGSFELUP";
        m_NtSys_to_string[Susy::NtSys::TRIGSF_EL_DN] = "TRIGSFELDN";
        m_NtSys_to_string[Susy::NtSys::TRIGSF_MUON_UP] = "TRIGSFMUUP";
        m_NtSys_to_string[Susy::NtSys::TRIGSF_MUON_DN] = "TRIGSFMUDN";
        m_NtSys_to_string[Susy::NtSys::TES_UP]       = "TESUP";
        m_NtSys_to_string[Susy::NtSys::TES_DN]       = "TESDOWN";
        m_NtSys_to_string[Susy::NtSys::JVF_UP]       = "JVFUP";
        m_NtSys_to_string[Susy::NtSys::JVF_DN]       = "JVFDOWN";
*/
    }

    ////////////////////////////////////////////
    // Destructor
    ////////////////////////////////////////////
    Superflow::~Superflow()
    {}


    // Superlink
    // Superlink
    // Superlink

    void Superflow::attach_superlink(Superlink* sl_)
    {
        //sl_->tools = this;
        sl_->tools = &m_nttools; // from SusyNtAna

        sl_->anaType = m_nttools.getAnaType();

        sl_->nt = &nt; // SusyNt
        sl_->weights = m_weights;
        sl_->nt_sys = m_RunSyst->event_syst;

        sl_->preElectrons = &m_preElectrons;
        sl_->preMuons = &m_preMuons;
        sl_->preTaus = &m_preTaus;
        sl_->preJets = &m_preJets;

        sl_->baseLeptons = &m_baseLeptons;
        sl_->baseElectrons = &m_baseElectrons;
        sl_->baseMuons = &m_baseMuons;
        sl_->baseTaus = &m_baseTaus;
        sl_->baseJets = &m_baseJets;

        scratchLeptonVector.clear();
        if (doFakes) {
            if (m_baseLeptons.size() == fakesTightLooseConfig.size()) {
                // populate `leptons` with baseline leptons to be *treated* as signal for fakes
                for (uint i = 0; i < fakesTightLooseConfig.size(); ++i) {
                    char & c = fakesTightLooseConfig[i];
                    if (c == '1') scratchLeptonVector.push_back(m_baseLeptons[i]);
                    else if (c != '0') {
                        cout << "ERROR: Superflow::attach_superlink: illegal `fakesTightLooseConfig`: " << fakesTightLooseConfig << "\n";
                        exit(1);
                    }
                }
            }
            sl_->leptons = &scratchLeptonVector;
        }
        else if (doQflip) sl_->leptons = &m_baseLeptons;  // treat all baseline leptons as signal for qflip
        else              sl_->leptons = &m_signalLeptons;

        sl_->signalLeptons = &m_signalLeptons;
        sl_->electrons = &m_signalElectrons;
        sl_->muons = &m_signalMuons;
        sl_->taus = &m_signalTaus;
        sl_->jets = &m_signalJets;

        sl_->met = m_met;
        sl_->trackMet = m_trackMet;

        if (nt.evt()->isMC) {
            sl_->isMC = true;
        }
        else {
            sl_->isData = true;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    // TSelector Begin
    //  > Called before looping on entries
    ///////////////////////////////////////////////////////////////////////////////
    void Superflow::Begin(TTree* /*tree*/)
    {
        cout << app_name << "Superflow::Begin" << endl;
        SusyNtAna::Begin(0);

        cout << app_name << "Superflow::Begin    Input sample: "<< endl;
        cout << app_name << "Superflow::Begin    " << m_sample << endl;

        if (m_runMode == SuperflowRunMode::null) {
            cout << app_name << "Superflow::Begin ERROR (Fatal)    Missing call to Superflow::setRunMode()." << endl;
            exit(1);
        }

        if (m_varState != SupervarState::closed || m_sysState != SupersysState::closed) {
            cout << app_name << "Superflow::Begin ERROR (Fatal)    Close the Var using SaveVar()." << endl;;
            exit(1);
        }

        if (m_runMode == SuperflowRunMode::single_event_syst && m_singleEventSyst == Susy::NtSys::NOM) {
            cout << app_name << "Superflow::Begin ERROR (Fatal)    SuperflowRunMode::single_event_syst: Call setSingleEventSyst(SusyNtSys nt_syst_)." << endl;
            exit(1);
        }

        // determine number of weight systematics
        if (m_runMode == SuperflowRunMode::nominal_and_weight_syst || m_runMode == SuperflowRunMode::all_syst) {
            for (int i = 0; i < m_sysStore.size(); i++) {
                if (m_sysStore[i].type == SupersysType::weight) {
                    index_weight_sys.push_back(i);
                }
            }
        }

        // determine number of event systematics
        if (m_runMode == SuperflowRunMode::all_syst) {
            for (int i = 0; i < m_sysStore.size(); i++) {
                if (m_sysStore[i].type == SupersysType::event) {
                    index_event_sys.push_back(i);
                }
            }
        }
        
        // if run mode single_event_syst, set which syst to run on
        if (m_runMode == SuperflowRunMode::single_event_syst) {
            for (int i = 0; i < m_sysStore.size(); i++) {
                if (m_sysStore[i].type == SupersysType::event && m_sysStore[i].event_syst == m_singleEventSyst) {
                    // initially set to nominal
                    delete m_RunSyst;

                    m_RunSyst = &m_sysStore[i]; // don't delete!!
                }
            }
        } // end if run mode single_event_syst


        //string period = "Moriond";
        //bool useReweightUtils = false;
    } // end Superflow::Begin()

    ///////////////////////////////////////////////////////////////////////////////
    // TSelector Init
    //  > called when the TChain is attached
    //  > initialize all output files and ntuples
    ///////////////////////////////////////////////////////////////////////////////
    void Superflow::Init(TTree* tree)
    {
        cout << app_name << "Superflow::Init" << endl;
        SusyNtAna::Init(tree);
        
        TString input_sample = m_input_chain->GetFile()->Get("inputContainerName")->GetTitle();
        TString output_sample = m_input_chain->GetFile()->Get("outputContainerName")->GetTitle();
        TString nt_tag = m_input_chain->GetFile()->Get("productionTag")->GetTitle();
        TString prod_command = m_input_chain->GetFile()->Get("productionCommand")->GetTitle();
        cout << endl;
        cout << app_name << "Superflow::Init    ================ Sample Information ==============" << endl;
        cout << app_name << "Superflow::Init     Input container name  : " << input_sample  << endl;
        cout << app_name << "Superflow::Init     Output container name : " << output_sample << endl;
        cout << app_name << "Superflow::Init     SusyNt production tag : " << nt_tag << endl;
        cout << app_name << "Superflow::Init     SusyNt production cmd : " << prod_command << endl; 
        cout << app_name << "Superflow::Init    ==================================================" << endl;

        if (!nt.evt()->isMC) {
            cout << app_name << "Superflow::Init    Switching run mode to SuperflowRunMode::data" << endl;
            m_runMode = SuperflowRunMode::data; // Changing the run mode!!
        }
        else if (nt.evt()->isMC) {
            initMcWeighter(tree);
        }

        // output file name
        stringstream sfile_name_;

        // determine output file name
        if (m_runMode == SuperflowRunMode::data) {
            m_countWeights = true;

            // output file name
            stringstream sfile_name_;
            sfile_name_ << "CENTRAL_";

            if(input_sample.Contains("data15_13TeV") || input_sample.Contains("data16_13TeV")){
                TString data_run; data_run.Form("%d",nt.evt()->run);
                if(input_sample.Contains("physics_Main")) {
                    cout << app_name << "Superflow::Init    ====== Determined input data specifications ====== " << endl;
                    cout << app_name << "Superflow::Init     stream: Main " << endl;
                    cout << app_name << "Superflow::Init     run   : " << data_run << endl;
                    cout << app_name << "Superflow::Init    ==================================================" << endl;
                    cout << endl;
                    m_stream = ATLAS_stream::Main;
                } // if main
                else {
                    cout << app_name << "Superflow::Init ERROR    Could not determine data stream for sample from the input container: " << endl;
                    cout << app_name << "Superflow::Init ERROR    \t" << input_sample << endl;
                    cout << app_name << "Superflow::Init ERROR    The only supported stream is 'Main' ('physics_Main')." << endl;
                    cout << app_name << "Superflow::Init ERROR    >>> Exiting." << endl;
                    exit(1);
                } 

                stringstream suffix;
                if(m_outputFileNameSuffix!="")
                    suffix << "_" << m_outputFileNameSuffix;
                else suffix << "";

                sfile_name_ << m_data_stream2string[m_stream] << "_" << data_run << suffix.str();
                cout << app_name << "Superflow::Init    Setting output file name to: " << sfile_name_.str() << endl;

                if (!doQflip && !doFakes) sfile_name_ << "_data";
                if (doQflip) sfile_name_ << "_qflip";
                if (doFakes) sfile_name_ << "_fakes_" << fakesTightLooseConfig;
                sfile_name_ << ".root";
                
                m_outputFileName = sfile_name_.str();
                m_entry_list_FileName += m_data_stream2string[m_stream] + "_" + data_run + suffix.str() + ".root";

            }
            else {
                cout << app_name << "Superflow::Init ERROR    The input container name does not appear to be a data sample." << endl;
                cout << app_name << "Superflow::Init ERROR    It does not contain either the 'data15_13TeV' or 'data16_13TeV' grouping and the run mode is" << endl;
                cout << app_name << "Superflow::Init ERROR    set for data (SuperflowRunMode::data). " << endl;
                cout << app_name << "Superflow::Init ERROR    >>> Exiting." << endl;
                exit(1);
            } 

        }
        else if (m_runMode == SuperflowRunMode::single_event_syst) {
            if (m_NtSys_to_string.count(m_singleEventSyst) != 0) {

                stringstream suffix;
                if(m_outputFileNameSuffix!="")
                    suffix << "_" << m_outputFileNameSuffix;
                else suffix << "";

                stringstream sfile_name_; // output file name
                sfile_name_ << m_NtSys_to_string[m_singleEventSyst] << "_" << nt.evt()->mcChannel
                            << suffix.str() << ".root";
                cout << app_name << "Superflow::Init    Run mode: SuperflowRunMode::single_event_syst" << endl;
                cout << app_name << "Superflow::Init    Setting output file name to: " << sfile_name_.str() << endl;
                m_outputFileName = sfile_name_.str();
                m_entry_list_FileName += sfile_name_.str() + suffix.str() + ".root";
            }
            else {
                cout << app_name << "Superflow::Init ERROR (Fatal)    Unknown event systematic! Code: " << static_cast<int>(m_singleEventSyst) << endl;
                exit(1);
            }
        }
        else if (nt.evt()->isMC) {
            stringstream suffix;
            if(m_outputFileNameSuffix!="")
                suffix << "_" << m_outputFileNameSuffix;
            else suffix << "";

            stringstream sfile_name_; // output file name
            sfile_name_ << "CENTRAL_" << nt.evt()->mcChannel << suffix.str() << ".root";
            if (m_runMode == SuperflowRunMode::nominal_and_weight_syst) {
                cout << app_name << "Superflow::Init    Run mode: SuperflowRunMode::nominal_and_weight_syst" << endl;
            }
            else {
                cout << app_name << "Superflow::Init    Run mode: SuperflowRunMode::nominal (weighted)" << endl;
            }
            cout << app_name << "Superflow::Init    Setting output file name to: " << sfile_name_.str() << endl;
            m_outputFileName = sfile_name_.str();
            m_entry_list_FileName += to_string(nt.evt()->mcChannel) + suffix.str() + ".root";
        }
        else {
            cout << app_name << "Superflow::Init ERROR (Fatal)    Inconsistent setup." << endl;
            exit(1);
        }

        m_outputFile = new TFile(m_outputFileName.data(), "RECREATE");

        /////////////////////////////////////////////
        // Set the output tree name
        /////////////////////////////////////////////
    //    stringstream tree_name;
    //    if (m_runMode == SuperflowRunMode::data) {
    //        tree_name << "id_" << m_data_stream2string[m_stream];
    //    }
    //    else {
    //        tree_name << "id_" << nt.evt()->mcChannel;
    //    }
        stringstream tree_name;
        tree_name << "superNt";
        cout << app_name << "Superflow::Init    Setting output tree name to: " << tree_name.str() << endl; 

        m_tree_name_auto = tree_name.str();

        // initialize total entry list (also see Notify();)
        m_entryListFile = new TFile(m_entry_list_FileName.data(), "RECREATE");
        m_entry_list_total = new TEntryList(m_tree_name_auto.data(), m_tree_name_auto.data());
        m_entry_list_total->SetDirectory(m_entryListFile);

        // initialize output tree
        m_HFT = new TTree(tree_name.str().data(), tree_name.str().data());
        m_HFT->SetDirectory(m_outputFile);
        m_HFT->SetAutoFlush(-16777216L);

        // define number of trees
        m_tree_leafs_size = m_varType.size() + 2 * index_weight_sys.size();// 2nd term may be zero
        m_weight_leaf_offset = m_varType.size();
       

        m_varFloat = new Float_t[m_tree_leafs_size]; // this one is larger to hold the syst_WEIGHT
        //m_varFloatArray = new Double_t*[m_varType.size()];
        m_varDouble = new Double_t[m_varType.size()];
        m_varInt = new Int_t[m_varType.size()];
        m_varBool = new Bool_t[m_varType.size()];

        // initialize HFT
        for (int i = 0; i < m_tree_leafs_size; i++) m_varFloat[i] = 1.0;
        for (int i = 0; i < m_varType.size(); i++) m_varDouble[i] = 1.0;
        for (int i = 0; i < m_varType.size(); i++) m_varInt[i] = 0;
        for (int i = 0; i < m_varType.size(); i++) m_varBool[i] = false;
        for (int i = 0; i < m_varType.size(); i++) { //m_varFloatArray[i] = 1.0; }
            vector<double> null(25, -999);
            vector<bool> f(25, false);
            m_varFloatArray.push_back(null);
            m_varBoolArray.push_back(f);
        }

        for (int i = 0; i < m_varType.size(); i++) {
            switch (m_varType[i]) {
                case SupervarType::sv_void: break;
                case SupervarType::sv_float: {
                    string leaflist_ = m_varHFTName[i] + "/F";
                    m_HFT->Branch(m_varHFTName[i].data(), m_varFloat + i, leaflist_.data(), 65536);
                    break;
                }
                case SupervarType::sv_double: {
                    string leaflist_ = m_varHFTName[i] + "/D";
                    m_HFT->Branch(m_varHFTName[i].data(), m_varDouble + i, leaflist_.data(), 65536);
                    break;
                }
                case SupervarType::sv_int: {
                    string leaflist_ = m_varHFTName[i] + "/I";
                    m_HFT->Branch(m_varHFTName[i].data(), m_varInt + i, leaflist_.data(), 65536);
                    break;
                }
                case SupervarType::sv_bool: {
                    string leaflist_ = m_varHFTName[i] + "/O";
                    m_HFT->Branch(m_varHFTName[i].data(), m_varBool + i, leaflist_.data(), 65536);
                    break;
                }
                case SupervarType::sv_float_array: {
                    //string leaflist_ = m_varHFTName[i] + "[25]/D";
                    //m_HFT->Branch(m_varHFTName[i].data(), m_varFloatArray.at(i), leaflist_.data(), 65536);
                    m_HFT->Branch(m_varHFTName[i].data(), &m_varFloatArray[i]);
                    break;
                }
                case SupervarType::sv_bool_array: {
                    m_HFT->Branch(m_varHFTName[i].data(), &m_varBoolArray[i]);
                    break;
                }
            }
        }

        for (int i = 0; i < index_weight_sys.size(); i++) {
            string syst_var_name_up = weight_prefix + m_sysStore[index_weight_sys[i]].tree_name + weight_suffix_up;
            string syst_var_name_down = weight_prefix + m_sysStore[index_weight_sys[i]].tree_name + weight_suffix_down;

            string leaflist_up = syst_var_name_up + "/F";
            string leaflist_down = syst_var_name_down + "/F";

            cout << app_name << "Weight var trees: " << syst_var_name_up << ", " << syst_var_name_down << endl;

            m_HFT->Branch(syst_var_name_up.data(), m_varFloat + m_weight_leaf_offset + 2 * i, leaflist_up.data(), 65536);
            m_HFT->Branch(syst_var_name_down.data(), m_varFloat + m_weight_leaf_offset + 2 * i + 1, leaflist_down.data(), 65536);
        }

        // Make an output file for each event systematic.
        if (m_runMode == SuperflowRunMode::all_syst) {

            m_output_array = new TFile*[index_event_sys.size()];
            for (int i = 0; i < index_event_sys.size(); i++) {

                stringstream suffix;
                if(m_outputFileNameSuffix!="")
                    suffix << "_" << m_outputFileNameSuffix;
                else suffix << "";

                stringstream sfile_name_; // output file name
                sfile_name_ << m_NtSys_to_string[m_sysStore[index_event_sys[i]].event_syst] << "_" << nt.evt()->mcChannel << suffix.str() << ".root";
                m_output_array[i] = new TFile(sfile_name_.str().data(), "RECREATE");
            }

            m_HFT_array = new TTree*[index_event_sys.size()];
            for (int i = 0; i < index_event_sys.size(); i++) {
                m_HFT_array[i] = new TTree(tree_name.str().data(), tree_name.str().data());
                m_HFT_array[i]->SetDirectory(m_output_array[i]);
                m_HFT_array[i]->SetAutoFlush(-16777216L);
            }

            m_varFloat_array = new Float_t*[index_event_sys.size()];
            m_varDouble_array = new Double_t*[index_event_sys.size()];
            m_varInt_array = new Int_t*[index_event_sys.size()];
            m_varBool_array = new Bool_t*[index_event_sys.size()];

            m_varFloatArray_array.clear();
            m_varBoolArray_array.clear();
            vector<vector<vector<double>>> f(index_event_sys.size(), vector<vector<double>>(m_varType.size(),
                            vector<double>(25,1.0)));
            m_varFloatArray_array = f;

            vector<vector<vector<bool>>> fb(index_event_sys.size(), vector<vector<bool>>(m_varType.size(),
                            vector<bool>(25,false)));
            m_varBoolArray_array = fb;


            for (int i = 0; i < index_event_sys.size(); i++) {
                m_varFloat_array[i] = new Float_t[m_varType.size()];
                m_varDouble_array[i] = new Double_t[m_varType.size()];
                m_varInt_array[i] = new Int_t[m_varType.size()];
                m_varBool_array[i] = new Bool_t[m_varType.size()];

                for (int j = 0; j < m_varType.size(); j++) m_varFloat_array[i][j] = 1.0;
                for (int j = 0; j < m_varType.size(); j++) m_varDouble_array[i][j] = 1.0;
                for (int j = 0; j < m_varType.size(); j++) m_varInt_array[i][j] = 0;
                for (int j = 0; j < m_varType.size(); j++) m_varBool_array[i][j] = false;
            }

            for (int i = 0; i < index_event_sys.size(); i++) {
                for (int j = 0; j < m_varType.size(); j++) {
                    switch (m_varType[j]) {
                        case SupervarType::sv_void: break;
                        case SupervarType::sv_float: {
                            string leaflist_ = m_varHFTName[j] + "/F";
                            m_HFT_array[i]->Branch(m_varHFTName[j].data(), m_varFloat_array[i] + j, leaflist_.data(), 65536);
                            break;
                        }
                        case SupervarType::sv_float_array: { 
                            m_HFT_array[i]->Branch(m_varHFTName[j].data(), &m_varFloatArray_array[i][j]); 
                            break;
                        }
                        case SupervarType::sv_bool_array: {
                            m_HFT_array[i]->Branch(m_varHFTName[j].data(), &m_varBoolArray_array[i][j]);
                            break;
                        }
                        case SupervarType::sv_double: {
                            string leaflist_ = m_varHFTName[j] + "/D";
                            m_HFT_array[i]->Branch(m_varHFTName[j].data(), m_varDouble_array[i] + j, leaflist_.data(), 65536);
                            break;
                        }
                        case SupervarType::sv_int: {
                            string leaflist_ = m_varHFTName[j] + "/I";
                            m_HFT_array[i]->Branch(m_varHFTName[j].data(), m_varInt_array[i] + j, leaflist_.data(), 65536);
                            break;
                        }
                        case SupervarType::sv_bool: {
                            string leaflist_ = m_varHFTName[j] + "/O";
                            m_HFT_array[i]->Branch(m_varHFTName[j].data(), m_varBool_array[i] + j, leaflist_.data(), 65536);
                            break;
                        }
                    }
                }
            }
        }
    } // end Superflow::Init()

    ///////////////////////////////////////////////////////////////////////////////
    // TSelector Notify
    //  > Called at each event
    ///////////////////////////////////////////////////////////////////////////////
    Bool_t Superflow::Notify()
    {
        cout << app_name << "Superflow::Notify" << endl;
        static int tree_counter;

        if (m_entry_list_single_tree != nullptr) m_entry_list_total->Add(m_entry_list_single_tree);
        delete m_entry_list_single_tree;

        string new_list_name = m_tree_name_auto + "_" + to_string(tree_counter);
        tree_counter++;

        m_entry_list_single_tree = new TEntryList();
        m_entry_list_single_tree->SetTree(m_input_chain->GetTree());

        return kTRUE;
    } // end Superflow::Notify()

    ///////////////////////////////////////////////////////////////////////////////
    // TSelector Process
    //  > Called at each event
    ///////////////////////////////////////////////////////////////////////////////
    Bool_t Superflow::Process(Long64_t entry)
    {
        GetEntry(entry);
        ///////////////////////////////////////////////////////////////////////
        // since we may be looping over for many systematics, etc... we only
        // want to save to the EntryLists each entry once. "save_entry_to_list"
        // acts as a catch to prevent from duplicate storing.
        bool save_entry_to_list = true;
        ///////////////////////////////////////////////////////////////////////

        m_chainEntry++; // SusyNtAna counter

        //////////////////////////////////////////////
        // process counter
        //////////////////////////////////////////////
        if (m_chainEntry % 50000 == 0) {
            cout << app_name << "Superflow::Process    **** Processing entry " << setw(6) << m_chainEntry
                << " run "   << setw(6) << nt.evt()->run
                << " event " << setw(7) << nt.evt()->eventNumber << " ****" << endl;
        }

        // these are flags
        var_float* vf_          = nullptr;
        var_double* vd_         = nullptr;
        var_float_array* vfa_   = nullptr;
        var_bool_array* vba_    = nullptr;
        var_int* vi_            = nullptr;
        var_bool* vb_           = nullptr;
        var_void* vv_           = nullptr;

        /////////////////////////////////////////////////////////////
        // Now select the objects, etc... depending on which 
        // run mode you are in
        /////////////////////////////////////////////////////////////
        
        switch (m_runMode) {
            /////////////////////////
            // run mode data
            /////////////////////////
            case SuperflowRunMode::data: {
                SusyNtAna::clearObjects();
                SusyNtAna::selectObjects(m_RunSyst->event_syst, TauId::Medium); // always select with nominal? (to compute event flags)

                m_weights = new Superweight();
                Superlink* sl_ = new Superlink;
                attach_superlink(sl_);

                bool pass_cuts = true; // loop over and appply the cuts in m_CutStore.
                if (m_CutStore.size() > 0) {
                    for (int i = 0; i < m_CutStore.size(); i++) {
                        pass_cuts = m_CutStore[i](sl_); // run the cut function

                        if (pass_cuts) {
                            m_RawCounter[i]++;
                        }
                        else {
                            break;
                        }
                    }
                }

                if (pass_cuts) { // data passed cuts, so fill HFTs.
                    if (save_entry_to_list) {
                        m_entry_list_single_tree->Enter(entry);
                        save_entry_to_list = false;
                    }
                    for (int v_ = 0; v_ < m_varType.size(); v_++) {
                        switch (m_varType[v_]) {
                            case SupervarType::sv_float: {
                                m_varFloat[v_] = m_varExprFloat[v_](sl_, vf_); break;
                            }
                            case SupervarType::sv_double: {
                                m_varDouble[v_] = m_varExprDouble[v_](sl_, vd_); break;
                            }
                            case SupervarType::sv_float_array: { 
                                m_varFloatArray[v_] = m_varExprFloatArray[v_](sl_,vfa_);
                                break;
                            }
                            case SupervarType::sv_bool_array: {
                                m_varBoolArray[v_] = m_varExprBoolArray[v_](sl_,vba_);
                                break;
                            }
                            case SupervarType::sv_int: {
                                m_varInt[v_] = m_varExprInt[v_](sl_, vi_); break;
                            }
                            case SupervarType::sv_bool: {
                                m_varBool[v_] = m_varExprBool[v_](sl_, vb_); break;
                            }
                            case SupervarType::sv_void: {
                                m_varExprVoid[v_](sl_, vv_); break;
                            }
                        }
                    }
                    m_HFT->Fill();
                }
                delete sl_;
                delete m_weights;

            } break;
            
            /////////////////////////
            // run mode nominal
            // run mode single_event_syst
            /////////////////////////
            case SuperflowRunMode::nominal:
            case SuperflowRunMode::single_event_syst: {

                SusyNtAna::clearObjects();
                SusyNtAna::selectObjects(m_RunSyst->event_syst, TauId::Medium); // always select with nominal? (to compute event flags)

                m_weights = new Superweight();
                Superlink* sl_ = new Superlink;
                attach_superlink(sl_);

                bool pass_cuts = true;

                if (m_countWeights) {
                    computeWeights(nt, *m_mcWeighter, m_signalLeptons, m_baseJets, m_RunSyst, m_weights);
                }

                if (m_CutStore.size() > 0) {
                    for (int i = 0; i < m_CutStore.size(); i++) {
                        pass_cuts = m_CutStore[i](sl_); // run the cut function
                        if (pass_cuts) {
                            m_RawCounter[i]++;
                            if (m_countWeights) m_WeightCounter[i] += m_weights->product();
                        }
                        else {
                            break;
                        }
                    }
                }

                if (pass_cuts) {
                    if (save_entry_to_list) {
                        m_entry_list_single_tree->Enter(entry);
                        save_entry_to_list = false;
                    }
                    if (!m_countWeights) {
                        computeWeights(nt, *m_mcWeighter, m_signalLeptons, m_baseJets, m_RunSyst, m_weights);
                        m_WeightCounter[m_CutStore.size() - 1] += m_weights->product();
                    }

                    // FILL_HFTs
                    for (int v_ = 0; v_ < m_varType.size(); v_++) {
                        switch (m_varType[v_]) {
                            case SupervarType::sv_float: {
                                m_varFloat[v_] = m_varExprFloat[v_](sl_, vf_); break;
                            }
                            case SupervarType::sv_float_array: { 
                                m_varFloatArray[v_] = m_varExprFloatArray[v_](sl_,vfa_);
                                break;
                            }
                            case SupervarType::sv_bool_array: {
                                m_varBoolArray[v_] = m_varExprBoolArray[v_](sl_,vba_);
                                break;
                            }
                            case SupervarType::sv_double: {
                                m_varDouble[v_] = m_varExprDouble[v_](sl_, vd_); break;
                            }
                            case SupervarType::sv_int: {
                                m_varInt[v_] = m_varExprInt[v_](sl_, vi_); break;
                            }
                            case SupervarType::sv_bool: {
                                m_varBool[v_] = m_varExprBool[v_](sl_, vb_); break;
                            }
                            case SupervarType::sv_void: {
                                m_varExprVoid[v_](sl_, vv_); break;
                            }
                        }
                    }
                    m_HFT->Fill();
                }
                delete sl_;
                delete m_weights;

            } break;
            case SuperflowRunMode::all_syst: {
                for (int i = 0; i < index_event_sys.size(); i++) { // loop over event systematics
                    SusyNtAna::clearObjects();
                    delete m_RunSyst;

                    m_RunSyst = &m_sysStore[index_event_sys[i]]; // don't delete!!
                    SusyNtAna::selectObjects(m_RunSyst->event_syst, TauId::Medium); // always select with nominal? (to compute event flags)

                    m_weights = new Superweight();
                    Superlink* sl_ = new Superlink;
                    attach_superlink(sl_);

                    bool pass_cuts = true;

                    if (m_CutStore.size() > 0) {
                        for (int k = 0; k < m_CutStore.size(); k++) {
                            pass_cuts = m_CutStore[k](sl_); // run the cut function
                            if (!pass_cuts) break;
                        }
                    }

                    if (pass_cuts) {
                        if (save_entry_to_list) {
                            m_entry_list_single_tree->Enter(entry);
                            save_entry_to_list = false;
                        }
                        computeWeights(nt, *m_mcWeighter, m_signalLeptons, m_baseJets, m_RunSyst, m_weights);
                        // FILL_HFTs
                        for (int v_ = 0; v_ < m_varType.size(); v_++) {
                            switch (m_varType[v_]) {
                                case SupervarType::sv_float: {
                                    m_varFloat_array[i][v_] = m_varExprFloat[v_](sl_, vf_); break;
                                }
                                case SupervarType::sv_double: {
                                    m_varDouble_array[i][v_] = m_varExprDouble[v_](sl_, vd_); break;
                                }
                                case SupervarType::sv_float_array: { 
                                    m_varFloatArray_array[i][v_] = m_varExprFloatArray[v_](sl_,vfa_);
                                    break;
                                }
                                case SupervarType::sv_bool_array: {
                                    m_varBoolArray_array[i][v_] = m_varExprBoolArray[v_](sl_,vba_);
                                    break;
                                }
                                case SupervarType::sv_int: {
                                    m_varInt_array[i][v_] = m_varExprInt[v_](sl_, vi_); break;
                                }
                                case SupervarType::sv_bool: {
                                    m_varBool_array[i][v_] = m_varExprBool[v_](sl_, vb_); break;
                                }
                                case SupervarType::sv_void: {
                                    m_varExprVoid[v_](sl_, vv_); break;
                                }
                            }
                        }
                        m_HFT_array[i]->Fill();
                    }
                    delete sl_;
                    delete m_weights;

                    m_RunSyst = nullptr;
                }
            } // we didn't break!!
            case SuperflowRunMode::nominal_and_weight_syst: {
                delete m_RunSyst;
                m_RunSyst = new Supersys(SupersysType::central);

                SusyNtAna::clearObjects();
                SusyNtAna::selectObjects(m_RunSyst->event_syst, TauId::Medium);

                m_weights = new Superweight();
                Superlink* sl_ = new Superlink;
                attach_superlink(sl_);

                if (m_countWeights) {
                    computeWeights(nt, *m_mcWeighter, m_signalLeptons, m_baseJets, m_RunSyst, m_weights);
                }

                bool pass_cuts = true;

                if (m_CutStore.size() > 0) {
                    for (int i = 0; i < m_CutStore.size(); i++) {
                        pass_cuts = m_CutStore[i](sl_); // run the cut function

                        if (pass_cuts) {
                            m_RawCounter[i]++;
                            if (m_countWeights) m_WeightCounter[i] += m_weights->product();
                        }
                        else {
                            break;
                        }
                    }
                }

                if (pass_cuts) {
                    if (save_entry_to_list) {
                        m_entry_list_single_tree->Enter(entry);
                        save_entry_to_list = false;
                    }
                    computeWeights(nt, *m_mcWeighter, m_signalLeptons, m_baseJets, m_RunSyst, m_weights);

                    double nom_eventweight = m_weights->product();
                    if(!m_countWeights)
                        m_WeightCounter[m_CutStore.size() - 1] += m_weights->product();

                    // FILL HFTs
                    for (int v_ = 0; v_ < m_varType.size(); v_++) {
                        switch (m_varType[v_]) {
                            case SupervarType::sv_float: {
                                m_varFloat[v_] = m_varExprFloat[v_](sl_, vf_); break;
                            }
                            case SupervarType::sv_double: {
                                m_varDouble[v_] = m_varExprDouble[v_](sl_, vd_); break;
                            }
                            case SupervarType::sv_float_array: { 
                                m_varFloatArray[v_] = m_varExprFloatArray[v_](sl_,vfa_);
                                break;
                            }
                            case SupervarType::sv_bool_array: {
                                m_varBoolArray[v_] = m_varExprBoolArray[v_](sl_,vba_);
                                break;
                            }
                            case SupervarType::sv_int: {
                                m_varInt[v_] = m_varExprInt[v_](sl_, vi_); break;
                            }
                            case SupervarType::sv_bool: {
                                m_varBool[v_] = m_varExprBool[v_](sl_, vb_); break;
                            }
                            case SupervarType::sv_void: {
                                m_varExprVoid[v_](sl_, vv_); break;
                            }
                        }
                    }

                    // FILL more HFTs
                    for (int w_ = 0; w_ < index_weight_sys.size(); w_++) {
                        Superweight* weightComponents_copy = new Superweight(*m_weights);

                        // Up variation
                        m_RunSyst->weight_syst = m_sysStore[index_weight_sys[w_]].weight_syst_up; // do up variation
                        computeWeights(nt, *m_mcWeighter, m_signalLeptons, m_baseJets, m_RunSyst, weightComponents_copy);
                        double up_weight = weightComponents_copy->product();
                        delete weightComponents_copy;

                        weightComponents_copy = new Superweight(*m_weights);
                        // Down variation
                        m_RunSyst->weight_syst = m_sysStore[index_weight_sys[w_]].weight_syst_down; // do down variation
                        computeWeights(nt, *m_mcWeighter, m_signalLeptons, m_baseJets, m_RunSyst, weightComponents_copy);
                        double down_weight = weightComponents_copy->product();

                        delete weightComponents_copy;
                        weightComponents_copy = nullptr;

                        if (up_weight < nom_eventweight && down_weight > nom_eventweight) {
                            double temp = up_weight; // reason to swap these
                            up_weight = down_weight;
                            down_weight = temp;
                        }
                        if (nom_eventweight > m_epsilon) {
                            up_weight = up_weight / nom_eventweight;
                            down_weight = down_weight / nom_eventweight;
                        }
                        else {
                            up_weight = 1.0;
                            down_weight = 1.0;
                        }

                        *(m_varFloat + m_weight_leaf_offset + 2 * w_) = up_weight; // put in TBranch
                        *(m_varFloat + m_weight_leaf_offset + 2 * w_ + 1) = down_weight;
                    }
                    m_RunSyst->weight_syst = SupersysWeight::null; // must reset this value!
                    m_HFT->Fill();
                }
                delete sl_;
                delete m_weights;
            } break;
            default: break;
        }
        return kTRUE;
    } // end Superflow::Process()

    ///////////////////////////////////////////////////////////////////////////////
    // TSelector Terminate
    //  > Called after looping over events is finished
    ///////////////////////////////////////////////////////////////////////////////
    void Superflow::Terminate()
    {
        cout << app_name << "Superflow::Terminate" << endl;

        cout << app_name << "------------------------------ ------------------------------" << endl;
        cout << app_name << " Raw cutflow " << endl;
        cout << app_name << "----------------------------- -------------------------------" << endl;
        cout << std::fixed;
        cout << std::setprecision(0);
        for (int i = 0; i < m_CutStore.size(); i++) {
            cout << app_name << "Cut " << pad_width(to_string(i), 2) << ": " << pad_width(m_CutStoreNames[i], 32) << ": " << m_RawCounter[i] << endl;
        }
        cout << app_name << endl << app_name << endl;

        cout << app_name << "------------------------------ ------------------------------" << endl;
        cout << app_name << " Weighted cutflow " << endl;
        cout << app_name << "------------------------------- -----------------------------" << endl;
        cout << std::resetiosflags(std::ios::floatfield);
        cout << std::resetiosflags(std::ios::adjustfield);
        cout << std::setprecision(6);
        for (int i = 0; i < m_CutStore.size(); i++) {
            cout << app_name << "Cut " << pad_width(to_string(i), 2) << ": " << pad_width(m_CutStoreNames[i], 32) << ": " << m_WeightCounter[i] << endl;
        }
        cout << app_name << endl << app_name << endl;

        m_outputFile->Write();
        for (int i = 0; i < index_event_sys.size(); i++) m_output_array[i]->Write();

        m_outputFile->Close();
        for (int i = 0; i < index_event_sys.size(); i++) m_output_array[i]->Close();

        cout << app_name << "Files OK." << endl;

        if (m_entry_list_single_tree != nullptr) {
            m_entry_list_total->Add(m_entry_list_single_tree); // last tree
        }

        m_entryListFile->Write();
        m_entryListFile->Close();

        delete m_outputFile;
        delete m_entryListFile;
        for (int i = 0; i < index_event_sys.size(); i++) delete m_output_array[i];
        delete[] m_output_array;

        if (m_entry_list_single_tree != nullptr) delete m_entry_list_single_tree;

        // Trees and entry-lists in the files are deleted with the file.

        SusyNtAna::Terminate();

        //dantrim -- use MCWeighter from SusyNtAna
        //if (m_mcWeighter) delete m_mcWeighter;
        //if (m_trigObj) delete m_trigObj;

        delete[] m_varFloat;
        delete[] m_varDouble;
        delete[] m_varInt;
        delete[] m_varBool;

        if (m_runMode == SuperflowRunMode::all_syst) {
            for (int i = 0; i < index_event_sys.size(); i++) delete[] m_varFloat_array[i];
            delete m_varFloat_array;
            for (int i = 0; i < index_event_sys.size(); i++) delete[] m_varDouble_array[i];
            delete m_varDouble_array;
            for (int i = 0; i < index_event_sys.size(); i++) delete[] m_varInt_array[i];
            delete m_varInt_array;
            for (int i = 0; i < index_event_sys.size(); i++) delete[] m_varBool_array[i];
            delete m_varBool_array;
        }

        if (m_runMode != SuperflowRunMode::single_event_syst) delete m_RunSyst;

        cout << app_name << "Done." << endl;

    } // end Superflow::Terminate()



    bool Superflow::initMcWeighter(TTree *tree)
    {
        cout << app_name << "Superflow::initMcWeighter    Initializing MCWeighter" << endl;
        bool success = false;
        if (tree) {
            string xsecDir = gSystem->ExpandPathName("$ROOTCOREBIN/data/SUSYTools/mc15_13TeV/");
            cout << "MCWeighter address: " << &m_mcWeighter << endl;
            m_mcWeighter = &mcWeighter(); // use MCWeighter instance from SusyNtAna
        
            //m_mcWeighter = new MCWeighter();
            ////m_mcWeighter = new MCWeighter(tree, xsecDir);
            //if(m_useSumwFile) {
            //    cout << "Superflow::initMcWeighter    Setting sumw file for MCWeighter: " << m_sumw_file << endl;
            //    m_mcWeighter->setSumwFromFILE(m_sumw_file);
            //}
            // m_mcWeighter->buildSumwMap(tree);
            m_mcWeighter->printSumwMap();
            if (m_dbg) {
                cout << app_name << "Superflow::initMcWeighter    MCWeighter has been initialized." << endl;
                cout << app_name << "Superflow::initMcWeighter    MCWeighter using cross-section directory: " << xsecDir << endl;
            }
        }
        else {
            cout << app_name << "Superflow::initMcWeighter ERROR    Invalid input tree, cannot initialize MCWeighter." << endl;
            cout << app_name << "Superflow::initMcWeighter ERROR    >>> Exiting." << endl;
            exit(1);
        }
        return success;
    }

    // -------------------------------- WEIGHTS COMPUTATION ----------------------------- //

    bool Superflow::computeWeights(
        Susy::SusyNtObject &ntobj,
        MCWeighter &weighter,
        const LeptonVector& leptons,
        const JetVector& jets,
        Supersys* super_sys,
        Superweight* weights_
        )
    {

        // MCWeighter's susynt-weight calculation
        if (ntobj.evt()->isMC) {
            //MCWeighter::WeightSys wSys = MCWeighter::Sys_NOM;
            NtSys::SusyNtSys wSys = NtSys::NOM;


            bool do_susynt_w = false;

            switch (super_sys->weight_syst) {
                case SupersysWeight::PILEUP_UP : {
                    //wSys = MCWeighter::Sys_PILEUP_UP;
                    wSys = NtSys::PILEUP_UP;
                    do_susynt_w = true;
                    break;
                }
                case SupersysWeight::PILEUP_DN : {
                    //wSys = MCWeighter::Sys_PILEUP_DN;
                    wSys = NtSys::PILEUP_DN;
                    do_susynt_w = true;
                    break;
                }
                case SupersysWeight::null : {
                    do_susynt_w = true;
                }
                default: break;
            } // switch
            if (do_susynt_w) {
                weights_->susynt = weighter.getMCWeight(ntobj.evt(), m_luminosity, wSys, false);
            }

            // JVT eff
            bool do_jvt = false;
            switch (super_sys->weight_syst) {
                case SupersysWeight::JVT_EFF_UP :
                case SupersysWeight::JVT_EFF_DN : {
                    do_jvt = true;
                    break;
                } default: break;
            } // switch
            if(do_jvt && jets.size()>0) {
                double outsf = 1.0;
                for(int i = 0; i < (int)jets.size(); i++) {
                    double sf = jets[i]->jvtEff;
                    double delta = 0.0;
                    if(super_sys->weight_syst == SupersysWeight::JVT_EFF_UP) {
                        delta = jets[i]->jvtEff_up;
                    }
                    else if(super_sys->weight_syst == SupersysWeight::JVT_EFF_DN) {
                        delta = jets[i]->jvtEff_dn;
                    }
                    outsf *= (sf + delta);
                } // i
                weights_->jvt = outsf;
            } // do jvt 

            if(leptons.size()>=1) {
                bool do_lepSf_ = false;
                switch(super_sys->weight_syst) {
                    case SupersysWeight::EL_EFF_ID_UP :
                    case SupersysWeight::EL_EFF_ID_DN :
                    case SupersysWeight::EL_EFF_ISO_UP :
                    case SupersysWeight::EL_EFF_ISO_DN : 
                    case SupersysWeight::EL_EFF_RECO_UP :
                    case SupersysWeight::EL_EFF_RECO_DN :
                    case SupersysWeight::EL_EFF_TRIG_UP :
                    case SupersysWeight::EL_EFF_TRIG_DN :
                    case SupersysWeight::MUON_EFF_STAT_UP :
                    case SupersysWeight::MUON_EFF_STAT_DN :
                    case SupersysWeight::MUON_EFF_STAT_LOWPT_UP :
                    case SupersysWeight::MUON_EFF_STAT_LOWPT_DN :
                    case SupersysWeight::MUON_EFF_SYS_UP :
                    case SupersysWeight::MUON_EFF_SYS_DN :
                    case SupersysWeight::MUON_EFF_SYS_LOWPT_UP :
                    case SupersysWeight::MUON_EFF_SYS_LOWPT_DN :
                    case SupersysWeight::MUON_EFF_ISO_STAT_UP :
                    case SupersysWeight::MUON_EFF_ISO_STAT_DN :
                    case SupersysWeight::MUON_EFF_ISO_SYS_UP :
                    case SupersysWeight::MUON_EFF_ISO_SYS_DN :
                    case SupersysWeight::null : {
                        do_lepSf_ = true;
                    }; break;
                    default : break;
                } // switch
                if(do_lepSf_) {
                    float outSF = 1.;
                    for(unsigned int iL = 0; iL < leptons.size(); iL++) {
                        const Lepton &lep = *(leptons[iL]);
                        outSF *= computeLeptonEfficiencySf(lep, super_sys->weight_syst);
                    }
                    weights_->lepSf = outSF;
                } // do_lepSf
            } // lepts >= 1

            //flavor tagging systematics

            bool do_btag_sys = false;
            bool do_btag_nom = false;

            switch (super_sys->weight_syst) {
                case SupersysWeight::FT_EFF_B_UP :
                case SupersysWeight::FT_EFF_B_DN :
                case SupersysWeight::FT_EFF_C_UP :
                case SupersysWeight::FT_EFF_C_DN :
                case SupersysWeight::FT_EFF_LT_UP :
                case SupersysWeight::FT_EFF_LT_DN :
                case SupersysWeight::FT_EFF_EXTRAP_UP :
                case SupersysWeight::FT_EFF_EXTRAP_DN :
                case SupersysWeight::FT_EFF_EXTRAPC_UP :
                case SupersysWeight::FT_EFF_EXTRAPC_DN : {
                    do_btag_sys = true;
                    do_btag_nom = false;
                    break;
                };
                case SupersysWeight::null : {
                    do_btag_sys = false;
                    do_btag_nom = true;
                    break;
                };
                default: break;
            } //switch
            if (do_btag_nom) {
                double btagSF = 1.0;
                if(jets.size()>0) { btagSF = m_nttools.bTagSF(jets); } 
                weights_->btag = btagSF;
            }
            if (do_btag_sys) {
                double btagSF = 1.0;
                NtSys::SusyNtSys sys = NtSys::SusyNtSys::NOM;
                {
                    switch(super_sys->weight_syst) {
                        case SupersysWeight::FT_EFF_B_UP :
                            sys = NtSys::SusyNtSys::FT_EFF_B_systematics_UP;
                            break;
                        case SupersysWeight::FT_EFF_B_DN : 
                            sys = NtSys::SusyNtSys::FT_EFF_B_systematics_DN;
                            break;
                        case SupersysWeight::FT_EFF_C_UP :
                            sys = NtSys::SusyNtSys::FT_EFF_C_systematics_UP;
                            break;
                        case SupersysWeight::FT_EFF_C_DN :
                            sys = NtSys::SusyNtSys::FT_EFF_C_systematics_DN;
                            break;
                        case SupersysWeight::FT_EFF_LT_UP :
                            sys = NtSys::SusyNtSys::FT_EFF_Light_systematics_UP;
                            break;
                        case SupersysWeight::FT_EFF_LT_DN :
                            sys = NtSys::SusyNtSys::FT_EFF_Light_systematics_DN;
                            break;
                        case SupersysWeight::FT_EFF_EXTRAP_UP :
                            sys = NtSys::SusyNtSys::FT_EFF_extrapolation_UP;
                            break; 
                        case SupersysWeight::FT_EFF_EXTRAP_DN :
                            sys = NtSys::SusyNtSys::FT_EFF_extrapolation_DN;
                            break; 
                        case SupersysWeight::FT_EFF_EXTRAPC_UP :
                            sys = NtSys::SusyNtSys::FT_EFF_extrapolation_charm_UP;
                            break;
                        case SupersysWeight::FT_EFF_EXTRAPC_DN :
                            sys = NtSys::SusyNtSys::FT_EFF_extrapolation_charm_DN;
                            break;
                        default: break;
                    } // switch
                }
                if(jets.size()>0) { btagSF = m_nttools.bTagSFError(jets, sys); }
                weights_->btag = btagSF;
            }

        } //isMC
        return true;
    }

    double Superflow::computeLeptonEfficiencySf(const Susy::Lepton &lep, const SupersysWeight sys)
    {
        double out_SF = 1.0;
        double sf = 1.0;
        double delta = 0.0;

        AnalysisType currentAnaType = nttools().getAnaType();

        if (lep.isEle()) {
            const Electron* el = dynamic_cast<const Electron*> (&lep);

            // get the ElectronId for the analysis type to select the correct errSF
            ElectronId id;
            if(currentAnaType==AnalysisType::Ana_Stop2L) { id = ElectronId::MediumLLH; }
            else { id = ElectronId::TightLLH; }

            // select the nominal SF
            sf = el->eleEffSF[id];

            if (sys == SupersysWeight::EL_EFF_ID_UP) {
                delta = el->errEffSF_id_up[id];    // we store the signed errEffSF
            }
            else if (sys == SupersysWeight::EL_EFF_ID_DN) {
                delta = el->errEffSF_id_dn[id];    // we store the signed errEffSF
            }
            else if (sys == SupersysWeight::EL_EFF_ISO_UP) {
                delta = el->errEffSF_iso_up[id];
            }
            else if (sys == SupersysWeight::EL_EFF_ISO_DN) {
                delta = el->errEffSF_iso_dn[id];
            }
            else if (sys == SupersysWeight::EL_EFF_RECO_UP) {
                delta = el->errEffSF_reco_up[id];  // we store the signed errEffSF
            }
            else if (sys == SupersysWeight::EL_EFF_RECO_DN) {
                delta = el->errEffSF_reco_dn[id];  // we store the signed errEffSF
            }
            else if (sys == SupersysWeight::EL_EFF_TRIG_UP) {
                delta = el->errEffSF_trig_up[id];
            }
            else if (sys == SupersysWeight::EL_EFF_TRIG_DN) {
                delta = el->errEffSF_trig_dn[id];
            }
        } // isEle
        else if (lep.isMu()) {
            const Muon* mu = dynamic_cast<const Muon*> (&lep);

            // get the MuonId for the analysis type to select the correct errSF
            MuonId id;
            if(currentAnaType==AnalysisType::Ana_Stop2L) { id = MuonId::Medium; }
            else { id = MuonId::Medium; }

            // select the nominal SF
            sf = mu->muoEffSF[id];

            if ( sys == SupersysWeight::MUON_EFF_STAT_UP ) {
                delta = mu->errEffSF_stat_up[id];
            }
            else if (sys == SupersysWeight::MUON_EFF_STAT_DN ) {
                delta = mu->errEffSF_stat_dn[id];
            }
            else if (sys == SupersysWeight::MUON_EFF_STAT_LOWPT_UP ) {
                delta = mu->errEffSF_stat_lowpt_up[id];
            }
            else if (sys == SupersysWeight::MUON_EFF_STAT_LOWPT_DN ) {
                delta = mu->errEffSF_stat_lowpt_dn[id];
            }
            else if (sys == SupersysWeight::MUON_EFF_SYS_UP ) {
                delta = mu->errEffSF_syst_up[id];
            }
            else if (sys == SupersysWeight::MUON_EFF_SYS_DN ) {
                delta = mu->errEffSF_syst_dn[id];
            }
            else if (sys == SupersysWeight::MUON_EFF_SYS_LOWPT_UP ) {
                delta = mu->errEffSF_syst_lowpt_up[id];
            }
            else if (sys == SupersysWeight::MUON_EFF_SYS_LOWPT_DN ) {
                delta = mu->errEffSF_syst_lowpt_dn[id];
            }
            else if (sys == SupersysWeight::MUON_EFF_ISO_STAT_UP ) {
                delta = mu->errIso_stat_up[id];
            }
            else if (sys == SupersysWeight::MUON_EFF_ISO_STAT_DN ) {
                delta = mu->errIso_stat_dn[id];
            }
            else if (sys == SupersysWeight::MUON_EFF_ISO_SYS_UP ) {
                delta = mu->errIso_syst_up[id];
            }
            else if (sys == SupersysWeight::MUON_EFF_ISO_SYS_DN ) {
                delta = mu->errIso_syst_dn[id];
            }
        } // isMu

        out_SF = (sf + delta);
        return out_SF;
    }


    void Superflow::setLumi(const float lumi) {
        m_luminosity = lumi;
        cout << app_name << "------------------------------ ------------------------------" << endl;
        cout << app_name << " Setting MC normalization (luminosity) to " << m_luminosity << " pb^-1." << endl;
        cout << app_name << "----------------------------- -------------------------------" << endl;
    }

    void Superflow::setCountWeights(bool value) ///> public function, if set true it prints the weighted cutflow
    {
        m_countWeights = value;
    }

    void Superflow::setRunMode(SuperflowRunMode run_mode_) ///> public function
    {
        m_runMode = run_mode_;
    }

    void Superflow::setSingleEventSyst(SusyNtSys nt_syst_)
    {
        m_singleEventSyst = nt_syst_;
    }

    void Superflow::setChain(TChain* input_chain_)
    {
        m_input_chain = input_chain_;
    }

    void Superflow::setFileSuffix(string suffix)
    {
        m_outputFileNameSuffix = suffix;
    }

    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////

    // --------------------------------------------------------------- //
    //  Superflow operator implementation
    //  Superflow operator implementation
    //  Superflow operator implementation
    // --------------------------------------------------------------- //

    ////////////////////////////////////////////
    // Cut operators
    ////////////////////////////////////////////

    /* ---------------------------
        CutName operator

        Feeds in a human-readable/descriptive name for a cut to be placed.
        --> Checks that the SupersysState and SupervarState are closed, meaning
            that any previous definition of a systematic or variable is finished
            and succesfully saved/stored.

        usage:
            <SuperflowObject> << CutName("lead lepton pT > 20 GeV");
       ---------------------------
    */
    Superflow& Superflow::operator<<(CutName cut_)
    {
        if (m_sysState == SupersysState::closed && m_varState == SupervarState::closed) {
            m_CutStore_Name_Exists = true;
            m_CutStoreNames.push_back(cut_.name);

            cout << app_name << "New cut: " << cut_.name << endl;
        }
        else {
            cout << app_name << "ERROR (Fatal): Cutflow operations are incorrectly ordered.";
            exit(1);
        }
        return *this;
    }
    
    /* ----------------------------
        Cut function operator

        Feeds in the (lambda) function expression to be evaluated for the cut.
        --> Checks that the SupersysState and SupervarState are closed, meaning
            that any previous definition of a systematic or variable is finished
            and succesfully saved/stored.
        --> Checks m_CutStore_Name_Exists variable to be sure that we have processed
            the CutName operator prior this call (i.e. so that this cut will have 
            a descriptive name). If this is not the case, a default name is given.
        --> Adds a place in the raw and weighted counters (m_RawCounter and m_WeightCounter).

         usage:
            <SuperflowObject> << CutName("at least 2 base leptons") << [](Superlink* sl) -> bool {
                return (sl->baseLeptons.size() >= 2);
            };
        --------------------------
    */
    Superflow& Superflow::operator<<(std::function<bool(Superlink*)> cut_)
    {
        if (m_sysState == SupersysState::closed && m_varState == SupervarState::closed) {
            m_CutStore.push_back(cut_);

            if (m_CutStore_Name_Exists) {
                m_CutStore_Name_Exists = false;
            }
            else {
                m_CutStoreNames.push_back("Untitled-" + to_string(m_CutStoreUntitled));
                cout << app_name << "New cut: " << "Untitled-" << m_CutStoreUntitled << endl;
                m_CutStoreUntitled++;
            }

            m_RawCounter.push_back(0.0);
            m_WeightCounter.push_back(0.0);
        }
        else {
            cout << app_name << "ERROR (Fatal): Cutflow operations are incorrectly ordered.";
            exit(1);
        }
        return *this;
    }

    ////////////////////////////////////////////
    // HFT operators
    //   These set what will be stored in the
    //   output ntuples
    ////////////////////////////////////////////

    /* ----------------------------
        NewVar operator
            
        Feeds in a human-readable/descriptive name for a variable to store in the output trees.
        --> This is NOT the name that will show up as the leaf name (i.e. when you do TBrowser/TTree::Draw())
        --> Checks that the SupersysState and SupervarState are closed, meaning
            that any previous definition of a systematic or variable is finished
            and succesfully saved/stored.
        --> Name is stored in vector m_varNiceName
        --> m_superVar_hasNiceName is set to true to ensure that the ordering of HFT var creation is correct.
            This is set to false once the full variable is saved/stored.
        --> Variable definition is to take place after the NewVar is fed in, so we set the 
            m_varState to SupervarState::open. This will be set to SupervarState::closed once
            the variable is saved/stored.

        usage:
            <SuperflowObject> << NewVar("stransverse mass");
 
       ----------------------------
    */
    Superflow& Superflow::operator<<(NewVar new_var_name) // this is the NiceName
    {
        if (m_varState == SupervarState::closed && m_sysState == SupersysState::closed) {
            m_varNiceName.push_back(new_var_name.name);
            m_superVar_hasNiceName = true;
            m_varState = SupervarState::open;
        }
        else {
            cout << app_name << "ERROR (Fatal): Close the Var using SaveVar().";
            exit(1);
        }
        return *this;
    }

    /* ----------------------------
        HFTname operator
        
        Feed in the name of the variable that will appear in the output ntuples (i.e. when you 
        do TTree::Draw() or look in the TBrowser).
        --> We must have already provided a "nice name" using the NewVar operator which means that
            m_varState == SupervarState::open.
        --> m_superVar_hasHFTName must be false, this operator provides the HFTname and sets this to true.
        --> The leaf name is stored in the vector m_varHFTName 

        usage:
            <SuperflowObject> << NewVar("stransverse mass") << HFTname("mT2");

       ----------------------------
    */
    Superflow& Superflow::operator<<(HFTname hft_name)
    {
        if (m_varState == SupervarState::open && m_sysState == SupersysState::closed && !m_superVar_hasHFTName) {
            m_varHFTName.push_back(hft_name.name);
            m_superVar_hasHFTName = true;
        }
        else {
            cout << app_name << "ERROR (Fatal): Open a new Var using NewVar().";
            exit(1);
        }
        return *this;
    }

    /* ----------------------------
        Variable function operators
        
        Provide a (lambda) function expression using the Superlink object to 
        calculate/provide the values for the variables whose names we have
        already provided using the NewVar and HFTname operators.
        --> There are function expressions for each of the conceivable variable types:
                - float
                - double
                - int
                - bool
                - void
            and the expression is fed into the appropriate vector of functions. 
            E.G. m_varExprFloat is type vector<std::function<double(Superlink*, var_float*)>> 
        --> var_float, var_double, var_int, var_void, and var_bool are dummy classes that are
            used only for type identification in the signatures of these different operators
            (see Superflow/Supervar.h).
        --> A null expression is provided to all other function vectors that are not of
            the type of the current function expression being fed in. In this way all of the
            function vectors will be of the same length and the indices of the functions will
            unambiguously line up with the vectors for the NewVar and HFTname vectors.
        --> m_varState must == SupervarState::open, indicating that we are currently in the process of
            storing a variable.
        --> m_superVar_hasFunction must be false since we have, at this point, only just opened the
            the m_varState and have only provided names for this variable.
        --> m_varType holds values of the enum SupervarType -- one for each function expression 
            fed in and is essentially used as a way to determine how many variables we have
            to store. 

        --> NOTE: for var_void type function expressions the NewVar and HFTname operators do not need
                to be called as they are set automatically within void function. The checks for the m_varState, etc...
                are also not checked. The main purpose of the var_void type function expression operator
                is to, for example, clear a global object that is defined in the executable outside of 
                the function expressions but used across many of them.
                E.G.
                    JetVector central_light_jets;
                    <VAR OPERATOR> { central_light_jets.push_back(foo) }
                        ...
                    <VAR OPERATOR> { central_light_jets.at(0)->Pt() }
                        ...
                    <SuperflowObject> << [&](Superlink* sl, var_void*) { central_light_jets.clear(); };

        usage:
            <SuperflowObject> << NewVar("leading lepton transverse momenta");
            <SuperflowObject> << HFTname("lept1Pt");
            <SuperflowObject> << [](Superlink* sl, var_float*) -> double { return sl->leptons->at(0)->Pt(); }

       ----------------------------
    */

    // float function
    Superflow& Superflow::operator<<(std::function<double(Superlink*, var_float*)> var_)
    {
        if (m_varState == SupervarState::open && m_sysState == SupersysState::closed && !m_superVar_hasFunction) {
            m_varExprFloat.push_back(var_); // fill
            m_varExprFloatArray.push_back(m_nullExprFloatArray);
            m_varExprBoolArray.push_back(m_nullExprBoolArray);
            m_varExprDouble.push_back(m_nullExprDouble);
            m_varExprInt.push_back(m_nullExprInt);
            m_varExprBool.push_back(m_nullExprBool);
            m_varExprVoid.push_back(m_nullExprVoid);

            m_varType.push_back(SupervarType::sv_float);
            m_superVar_hasFunction = true;
        }
        else {
            cout << app_name << "ERROR (Fatal): First open a new Var using NewVar().";
            exit(1);
        }
        return *this;
    }
    // vector<double> function
    Superflow& Superflow::operator<<(std::function<vector<double>(Superlink*, var_float_array*)> var_)
    {
        if (m_varState == SupervarState::open && m_sysState == SupersysState::closed && !m_superVar_hasFunction) {
            m_varExprFloat.push_back(m_nullExprFloat);
            m_varExprDouble.push_back(m_nullExprDouble);
            m_varExprBoolArray.push_back(m_nullExprBoolArray);
            m_varExprFloatArray.push_back(var_);
            m_varExprInt.push_back(m_nullExprInt);
            m_varExprBool.push_back(m_nullExprBool);
            m_varExprVoid.push_back(m_nullExprVoid);

            m_varType.push_back(SupervarType::sv_float_array);
            m_superVar_hasFunction = true;
        }
        else {
            cout << app_name << "ERROR (Fatal): First open a new Var using NewVar().";
            exit(1);
        }
        return *this;
    }
    // vector<bool> function
    Superflow& Superflow::operator<<(std::function<vector<bool>(Superlink*, var_bool_array*)> var_)
    {
        if (m_varState == SupervarState::open && m_sysState == SupersysState::closed && !m_superVar_hasFunction) {
            m_varExprFloat.push_back(m_nullExprFloat);
            m_varExprDouble.push_back(m_nullExprDouble);
            m_varExprBoolArray.push_back(var_);
            m_varExprFloatArray.push_back(m_nullExprFloatArray);
            m_varExprInt.push_back(m_nullExprInt);
            m_varExprBool.push_back(m_nullExprBool);
            m_varExprVoid.push_back(m_nullExprVoid);

            m_varType.push_back(SupervarType::sv_bool_array);
            m_superVar_hasFunction = true;
        }
        else {
            cout << app_name << "ERROR (Fatal): First open a new Var using NewVar().";
            exit(1);
        }
        return *this;
    }

        
    // double function
    Superflow& Superflow::operator<<(std::function<double(Superlink*, var_double*)> var_)
    {
        if (m_varState == SupervarState::open && m_sysState == SupersysState::closed && !m_superVar_hasFunction) {
            m_varExprFloat.push_back(m_nullExprFloat);
            m_varExprFloatArray.push_back(m_nullExprFloatArray);
            m_varExprBoolArray.push_back(m_nullExprBoolArray);
            m_varExprDouble.push_back(var_); // fill
            m_varExprInt.push_back(m_nullExprInt);
            m_varExprBool.push_back(m_nullExprBool);
            m_varExprVoid.push_back(m_nullExprVoid);

            m_varType.push_back(SupervarType::sv_double);
            m_superVar_hasFunction = true;
        }
        else {
            cout << app_name << "ERROR (Fatal): First open a new Var using NewVar().";
            exit(1);
        }
        return *this;
    }
    // int function
    Superflow& Superflow::operator<<(std::function<int(Superlink*, var_int*)> var_)
    {
        if (m_varState == SupervarState::open && m_sysState == SupersysState::closed && !m_superVar_hasFunction) {
            m_varExprFloat.push_back(m_nullExprFloat);
            m_varExprDouble.push_back(m_nullExprDouble);
            m_varExprFloatArray.push_back(m_nullExprFloatArray);
            m_varExprBoolArray.push_back(m_nullExprBoolArray);
            m_varExprInt.push_back(var_); // fill
            m_varExprBool.push_back(m_nullExprBool);
            m_varExprVoid.push_back(m_nullExprVoid);

            m_varType.push_back(SupervarType::sv_int);
            m_superVar_hasFunction = true;
        }
        else {
            cout << app_name << "ERROR (Fatal): First open a new Var using NewVar().";
            exit(1);
        }
        return *this;
    }
    // bool function
    Superflow& Superflow::operator<<(std::function<bool(Superlink*, var_bool*)> var_)
    {
        if (m_varState == SupervarState::open && m_sysState == SupersysState::closed && !m_superVar_hasFunction) {
            m_varExprFloat.push_back(m_nullExprFloat);
            m_varExprDouble.push_back(m_nullExprDouble);
            m_varExprFloatArray.push_back(m_nullExprFloatArray);
            m_varExprBoolArray.push_back(m_nullExprBoolArray);
            m_varExprInt.push_back(m_nullExprInt);
            m_varExprBool.push_back(var_); // fill
            m_varExprVoid.push_back(m_nullExprVoid);

            m_varType.push_back(SupervarType::sv_bool);
            m_superVar_hasFunction = true;
        }
        else {
            cout << app_name << "ERROR (Fatal): First open a new Var using NewVar().";
            exit(1);
        }
        return *this;
    }
    // void function
    Superflow& Superflow::operator<<(std::function<void(Superlink*, var_void*)> var_)
    {
        m_varExprFloat.push_back(m_nullExprFloat);
        m_varExprDouble.push_back(m_nullExprDouble);
        m_varExprFloatArray.push_back(m_nullExprFloatArray);
        m_varExprBoolArray.push_back(m_nullExprBoolArray);
        m_varExprInt.push_back(m_nullExprInt);
        m_varExprBool.push_back(m_nullExprBool);
        m_varExprVoid.push_back(var_);// fill

        m_varNiceName.push_back("void");
        m_varHFTName.push_back("void");
        m_varType.push_back(SupervarType::sv_void);
        return *this;
    }

    /* ----------------------------
        SaveVar operator
        
        Check that we have all of the required pieces for storing a variable in the 
        output ntuples.
        
        --> If the var state is still open and we have defined a function (with a name) then we
            have all of the pieces. Set these flags back to false so that we can be ready to
            start defining the next variable to be stored.
        --> If there is no name, provide the default one.

        usage:
            <SuperflowObject> << NewVar("sub-leading jet eta");
            <SuperflowObject> << HFTname("jet2Eta");
            <SuperflowObject> << [](Superlink* sl, var_float*) -> double {
                return sl->jets->size() >= 2 ? sl->jets->at(1)->Eta() : 0.0;
            };
            <SuperflowObject> << SaveVar();
            
       ----------------------------
    */
    Superflow& Superflow::operator<<(SaveVar save_var)
    {
        if (m_varState == SupervarState::open && m_superVar_hasFunction && m_superVar_hasNiceName) {
            if (!m_superVar_hasHFTName) {
                m_varHFTName.push_back("Untitled_" + to_string(m_superVar_Untitled));
                m_superVar_Untitled++;
                m_superVar_hasHFTName = true;
            }

            m_varState = SupervarState::closed;
            m_superVar_hasFunction = false;
            m_superVar_hasNiceName = false;
            m_superVar_hasHFTName = false;

            cout << app_name << "New var: " << m_varNiceName.back() << endl;
            cout << app_name << "    HFT: " << m_varHFTName.back() << endl;
        }
        else {
            if (m_varState != SupervarState::open) {
                cout << app_name << "ERROR (Fatal): First open a new Var using NewVar().";
            }
            else {
                cout << app_name << "ERROR (Fatal): A lambda-expression is required.";
            }
            exit(1);
        }
        return *this;
    }

    ////////////////////////////////////////////
    // Systematic operators
    ////////////////////////////////////////////

    Superflow& Superflow::operator<<(NewSystematic new_sys) // this is the NiceName
    {
        if (m_sysState == SupersysState::closed && m_varState == SupervarState::closed) {
            m_sysTemplate.name = new_sys.name;
            m_sys_hasNiceName = true;
            m_sysState = SupersysState::open;
        }
        else {
            cout << app_name << "ERROR (Fatal): Close using SaveSystematic().";
            exit(1);
        }
        return *this;
    }

    Superflow& Superflow::operator<<(TreeName tree_name)
    {
        if (m_sysState == SupersysState::open && m_varState == SupervarState::closed && !m_sys_hasTreeName) {
            m_sysTemplate.tree_name = tree_name.name;
            m_sys_hasTreeName = true;
        }
        else {
            cout << app_name << "ERROR (Fatal): Open a NewSystematic().";
            exit(1);
        }
        return *this;
    }

    Superflow& Superflow::operator<<(EventSystematic obj_)
    {
        if (m_sysState == SupersysState::open && m_varState == SupervarState::closed && !m_sys_hasSystematic) {
            m_sysTemplate.event_syst = obj_.event_syst_;
            m_sysTemplate.weight_syst = SupersysWeight::null;
            m_sys_hasSystematic = true;

            m_sysTemplate.type = SupersysType::event;
            m_sys_hasType = true;
        }
        else {
            cout << app_name << "ERROR (Fatal): Open a NewSystematic().";
            exit(1);
        }
        return *this;
    }

    Superflow& Superflow::operator<<(WeightSystematic obj_)
    {
        if (m_sysState == SupersysState::open && m_varState == SupervarState::closed && !m_sys_hasSystematic) {
            m_sysTemplate.event_syst = Susy::NtSys::NOM;
            m_sysTemplate.weight_syst_up = obj_.weight_syst_up;
            m_sysTemplate.weight_syst_down = obj_.weight_syst_down;
            m_sys_hasSystematic = true;

            m_sysTemplate.type = SupersysType::weight;
            m_sys_hasType = true;
        }
        else {
            cout << app_name << "ERROR (Fatal): Open a NewSystematic().";
            exit(1);
        }
        return *this;
    }

    Superflow& Superflow::operator<<(SaveSystematic save_var)
    {
        if (m_sysState == SupersysState::open
            && m_varState == SupervarState::closed
            && m_sys_hasNiceName
            && m_sys_hasTreeName
            && m_sys_hasType
            && m_sys_hasSystematic
            ) {
            if (m_sysTemplate.name == "") m_sysTemplate.name = m_sysTemplate.tree_name;

            m_sysState = SupersysState::closed;
            m_sys_hasNiceName = false;
            m_sys_hasTreeName = false;
            m_sys_hasSystematic = false;
            m_sys_hasType = false;

            m_sysStore.push_back(m_sysTemplate);
            m_sysTemplate.reset();

            cout << app_name << "New systematic: " << m_sysStore.back().name << endl;
            if (m_sysStore.back().type == SupersysType::event) {
                cout << app_name << "    event systematic: " << m_sysStore.back().tree_name << endl;
            }
            else if (m_sysStore.back().type == SupersysType::weight) {
                cout << app_name << "    weight systematics: " << m_sysStore.back().tree_name << weight_suffix_up
                    << "/" << m_sysStore.back().tree_name << weight_suffix_down << endl;
            }
            else {
                cout << app_name << "ERROR (Fatal): Impossible SupersysType.";
                exit(1);
            }

        }
        else {
            if (m_sysState != SupersysState::open) {
                cout << app_name << "ERROR (Fatal): First open a NewSystematic().";
            }
            else {
                cout << app_name << "ERROR (Fatal): Can't save incomplete systematic object.";
            }
            exit(1);
        }
        return *this;
    }

} // namespace sflow
