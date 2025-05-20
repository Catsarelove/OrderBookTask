using namespace std;
#include <string>
#include <map>
#include <unordered_map>
#include <iostream>
#include <iomanip>
#include <list>
#include <iterator>
#include "instrument.h"
#include <fstream>
#include <vector>
#include <algorithm>
#include <optional>

MarketSUB Subscriptor;

optional<reference_wrapper<instrument>> find_A(unsigned int oid, unordered_map<unsigned int, instrument &> &hueta)
{
    auto a = hueta.find(oid);
    if (a == hueta.end()) return nullopt;
    return a->second;
}

void exec_line(const string &command, const vector<string> &args, map<string, instrument> &Insts, unordered_map<unsigned int, instrument &> &hueta)
{

    if (command == "ORDER ADD")
    {
        string inam = args[1];
        unsigned int oid = stoul(args[0]);
        double pr = stod(args[3]);
        side_ side;
        unsigned int vol = stoul(args[2]);

        if (args[4] == "sell")
            side = sell;
        else
            side = buy;

        instrument & ptr = Insts[inam];

        if (ptr.iname == "_dummy_") ptr.iname = inam;

        hueta.try_emplace(oid, ptr);

        ptr.add_ord(oid, {vol, pr, side});

        Subscriptor.update(ptr);

    }

    if (command == "ORDER MODIFY")
    {
        unsigned int oid = stoul(args[0]);
        double pr = stod(args[2]);
        unsigned int vol = stoul(args[1]);

        auto ptr = find_A(oid, hueta);

        if (!ptr.has_value())
        {

            cout << "modify: no such order \n";
            return;
        }

        instrument & A = ptr.value();

        A.mod_ord(oid, vol, pr);

        Subscriptor.update(A);
    }

    if (command == "ORDER CANCEL")
    {;
        unsigned int oid = stoul(args[0]);

        auto ptr = find_A(oid, hueta);

        if (!ptr.has_value())
        {

            cout << "cancel: no such order \n";
            return;
        }

        instrument & A = ptr.value();

        A.remove_ord(oid);

        Subscriptor.update(A);
    }

    if (command == "SUBSCRIBE BBO")
    {

        string inam = args[0];

        Subscriptor.add_sub_bbo(inam);
    }
    if (command == "UNSUBSCRIBE BBO")
    {
        string inam = args[0];

        Subscriptor.uns_sub_bbo(inam);
    }
    if (command == "SUBSCRIBE VWAP")
    {

        string inam = args[0];
        unsigned int n = stoul(args[1]);

        Subscriptor.add_sub_vwap(inam, n);
    }
    if (command == "UNSUBSCRIBE VWAP")
    {

        string inam = args[0];
        unsigned int n = stoul(args[1]);

        Subscriptor.add_sub_vwap(inam, n);
    }
    if (command == "PRINT")
    {
        cout << "| ORDER BOOK |\n";
        if (args.empty()){
            auto it = Insts.find(Subscriptor.filter);
            if(it == Insts.end()){
                for(auto it = Insts.begin(); it != Insts.end(); it++ )
                    it->second.print_full();
            }
            else it->second.print_ob();
        }

        else{

            string inam = args[0];

            if (Subscriptor.filter != "" && inam != Subscriptor.filter) return;

            auto it = Insts.find(inam);

            if(it == Insts.end()){

                cout << "print: no such instrument\n";
                return;

            }

            it->second.print_ob();
        }

    }
    if (command == "PRINT_FULL")
    {
        cout << "| ORDERS |\n";
        if (args.empty()){
            for(auto it = Insts.begin(); it != Insts.end(); it++ )
                it->second.print_full();
        }
        else{

            string inam = args[0];

            if (Subscriptor.filter != "" && inam != Subscriptor.filter) return;

            auto it = Insts.find(inam);
        
            if(it == Insts.end()){

                cout << "print: no such instrument\n";
                return;


            }



            it->second.print_full();
        }
    }
}

class Market
{

public:
    map<string, instrument> Insts;
    string sym;
    string filename;
    unordered_map<unsigned int, instrument &>  hueta;

    Market(string filename_, string sym_)
    {

        filename = filename_;
        sym = sym_;
        Subscriptor.filter = sym_;
    }
    void parse()
    {

        string line;
        ifstream in(filename);
        while (getline(in, line))
        {

            string command;
            vector<string> args;
            istringstream s(line);

            getline(s, command, ',');

            for (string arg; getline(s, arg, ',');)
                args.push_back(arg);

            exec_line(command, args, Insts, hueta);
        }
    }
};

int main(int argc, char **argv)
{

    if (argc < 2)
    {

        cerr << "please enter imput file\n";
        return -1;
    }

    string sym;

    if (argc > 2)
        sym = argv[2];

    Market M(argv[1], sym);

    M.parse();

    return 0;
}
