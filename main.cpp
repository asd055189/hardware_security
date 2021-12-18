#include <bits/stdc++.h>

using namespace std;


struct Bn_Node_{
    int Id;
    string name;
    bool Type;//0=primary I/O ; 1=internal node
    int FType;
    vector<Bn_Node_*> FI_Ary;
    vector<Bn_Node_*> FO_Ary;
};
struct Bn_Ntk_{
    vector<Bn_Node_*> Node_Ary;
    vector<Bn_Node_*> PI_Ary;
    vector<Bn_Node_*> PO_Ary;
    vector<Bn_Node_*> Key_Ary;
};

int _ID=0;
map<string,Bn_Node_*>nodemap;
Bn_Ntk_ *network=new Bn_Ntk_;

int get_type_int(string gatename){
    if(gatename.find("nand")!=-1||gatename.find("NAND")!=-1)
        return 0;
    else if(gatename.find("and")!=-1||gatename.find("AND")!=-1)
        return 1;
    else if(gatename.find("xnor")!=-1||gatename.find("XNOR")!=-1)
        return 2;
    else if(gatename.find("nor")!=-1||gatename.find("nor")!=-1)
        return 3;
    else if(gatename.find("xor")!=-1||gatename.find("XOR")!=-1)
        return 4;
    else if(gatename.find("or")!=-1||gatename.find("or")!=-1)
        return 5;
    else if(gatename.find("buf")!=-1||gatename.find("buf")!=-1)
        return 6;
    else if(gatename.find("not")!=-1||gatename.find("NOT")!=-1)
        return 7;
}
int get_cost(int type_id,int N){
    if(type_id==1 || type_id==5 || type_id==3 || type_id==0 ){
        return N-1;
    }
    else if(type_id==2|| type_id==4){
        return 3;
    }
    else{
        return 1;
    }
}
string get_type_name(int type_id){
    if(type_id==0)
        return "nand";
    else if(type_id==1)
        return "and";
    else if(type_id==2)
        return "xnor";
    else if(type_id==3)
        return "nor";
    else if(type_id==4)
        return "xor";
    else if(type_id==5)
        return "or";
    else if(type_id==6)
        return "buf";
    else if(type_id==7)
        return "not";
}
Bn_Node_ * create_node(string name){
    Bn_Node_ *tmp ;
    if (nodemap.find(name) != nodemap.end()){
        tmp=nodemap[name];
        cout <<"duplicate create! node name : "<<name<<endl;
    }
    else{
        tmp=new Bn_Node_;
        tmp->Id=_ID;
        _ID++;
        tmp->name=name;
        nodemap[name]=tmp;
    }
    return tmp;
}

void gate_assignment(Bn_Node_ *tmp,int type_id,vector<string> FI){
    tmp->Type=1;//0=primary I/O ; 1=internal node
    tmp->FType=type_id;
    for(auto i:FI){
        Bn_Node_ *fi=create_node(i);
        tmp->FI_Ary.push_back(fi);
        fi->FO_Ary.push_back(tmp);
    }
}

vector<string> split(string str,char delim){
    vector<string> con;
    while(1){
        if(str.find(delim)==-1){
            string t=str.substr(0,str.size());
            con.push_back(t);
            break;
        }
        string t=str.substr(0,str.find(delim));
        con.push_back(t);
        str=str.substr(str.find(delim)+2,str.size());
    }
    return con;
}

void print_result(){
    for (auto i:network->PI_Ary){
        cout <<"INPUT("<<i->name<<")"<<endl;
    }
    for (auto i:network->PO_Ary){
        cout <<"OUTPUT("<<i->name<<")"<<endl;
    }
    for (auto i:network->Node_Ary){
        cout<<i->name<<" = "<<get_type_name(i->FType)<<"(";
        for (auto j:i->FI_Ary){
            cout <<j->name<<", ";
        }
        cout<<"\b\b)"<<endl;
    }
}
void enc(){
    vector<Bn_Node_ *>XNOR_ARY;
    for(auto i:network->PI_Ary){
        Bn_Node_ *newnode=new Bn_Node_;
        newnode->Id=_ID++;
        newnode->name="XNOR"+to_string(newnode->Id);
        newnode->Type=1;
        newnode->FType=2;
        Bn_Node_ *keynode=new Bn_Node_;
        keynode->Id=_ID++;
        keynode->name="keyinput"+to_string(keynode->Id);
        keynode->Type=0;
        keynode->FType=6;
        keynode->FO_Ary.push_back(newnode);
        newnode->FI_Ary.push_back(i);
        newnode->FI_Ary.push_back(keynode);
        network->Node_Ary.push_back(newnode);
        network->PI_Ary.push_back(keynode);

        XNOR_ARY.push_back(newnode);
    }
    Bn_Node_ *andnode=new Bn_Node_;
    andnode->Id=_ID++;
    andnode->name="AND"+to_string(andnode->Id);
    andnode->Type=1;
    andnode->FType=1;
    andnode->FI_Ary=XNOR_ARY;
    network->Node_Ary.push_back(andnode);

    for(auto i:network->PO_Ary){
        Bn_Node_ *xornode=new Bn_Node_;
        xornode->Id=_ID++;
        xornode->name=i->name;
        Bn_Node_ *outnode=create_node(i->name);
        outnode->name+="enc";
        xornode->Type=1;
        xornode->FType=4;
        xornode->FI_Ary.push_back(outnode);
        xornode->FI_Ary.push_back(andnode);
        network->Node_Ary.push_back(xornode);
        
    }
}
int main(int argc, char *argv[]){
    ifstream infile(argv[1],std::ifstream::in);
    if(!infile.is_open()){
        cout <<"file error!\n";
        return 0;
    }
    int areacost=0;
    string instr;
    while(getline(infile,instr)){
        //out <<instr<<endl;
        if(instr.find("INPUT")!=-1){
            Bn_Node_ *tmp=new Bn_Node_;
            tmp->name=instr.substr(instr.find("(")+1,instr.find(")")-instr.find("(")-1);
            tmp->Type=0;//0=primary I/O ; 1=internal node
            tmp->FType=0;//cost 0=1 ; 1=3 ; 2=N-1
            network->PI_Ary.push_back(tmp);
            areacost+=get_cost(6,1);
        }
        if(instr.find("OUTPUT")!=-1){
            Bn_Node_ *tmp=new Bn_Node_;
            tmp->name=instr.substr(instr.find("(")+1,instr.find(")")-instr.find("(")-1);
            
            tmp->Type=0;//0=primary I/O ; 1=internal node
            tmp->FType=0;//cost 0=1 ; 1=3 ; 2=N-1
            network->PO_Ary.push_back(tmp);
            areacost+=get_cost(6,1);
        }
        if(instr.find("=")!=-1){
            string nodename=instr.substr(0,instr.find(" "));
            string gatestr=instr.substr(instr.find("=")+1,instr.size());
            string gatename;
            string nodestr=gatestr.substr(gatestr.find("(")+1,gatestr.find(")")-gatestr.find("(")-1);
            vector<string> FI=split(nodestr,',');
            int type_id= get_type_int(gatestr);
            Bn_Node_ *tmp=create_node(nodename);
            gate_assignment(tmp,type_id,FI);
            network->Node_Ary.push_back(tmp);
            areacost+=get_cost(type_id,FI.size());
        }
    }
    print_result();
    cout <<"\n====================\n";
    enc();
    print_result();

    cout <<"cost"<<areacost<<endl;
}