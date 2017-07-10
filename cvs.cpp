#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <stdlib.h>
#include <unistd.h>
using namespace std;


struct state
{
    string tag;
    int status; // 1 = active / -1 = deleted
    int branch; // 0 = master / 1..n = branch
    string user;
    string date;
    fstream *fp;
    state(){status=1;};
    struct state *next;
    struct path *list;
};


struct path
{
    struct state *arrival_state;
    struct path *next;
};


typedef struct state *Tstate;
typedef struct path *Tpath;
 

struct cvs
{    
    Tstate root = NULL;    


    void add_state(string tag_, int branch_, string user_, string date_, fstream &file_)
    {
        Tstate t, new_ = new struct state;

        new_->tag = tag_;
        new_->branch = branch_;
        new_->user = user_;
        new_->date = date_;
        new_->fp = &file_;
        new_->next = NULL;
        new_->list = NULL;
        
        if(root == NULL)
            root = new_;
        else
        {
            t = root;
            if (t->status!=-1 and new_->status!=-1)
            {
                while(t->next != NULL)
                {
                    t = t->next;
                }
                t->next = new_;
            }
        }
    }

     
    void add_path(Tstate &aux, Tstate &aux2, Tpath &new_)
    {
        if (aux->status!=-1 and aux2->status!=-1)
        {
            Tpath temp;
      
            if(aux->list == NULL )
            {   
                aux->list = new_;
                new_->arrival_state = aux2;
            }
            else
            {   
                temp = aux->list;
                
                while(temp->next != NULL)
                    temp = temp->next;
                
                new_->arrival_state = aux2;
                temp->next = new_;
            }
        }
    }


    void create_path(string start, string end)
    {   
        Tpath new_ = new struct path;
        Tstate aux, aux2;
     
        if(root == NULL)
            return;
        
        new_->next = NULL;
        aux = root;
        aux2 = root;
     
        if (aux->status!=-1 and aux2->status!=-1)
        {
            while(aux2 != NULL)
            {
                if(aux2->tag == end)
                {
                    break;
                }
                aux2 = aux2->next;
            }

            while(aux != NULL)
            {
                if(aux->tag == start)
                {
                    add_path(aux, aux2, new_);
                    return;
                }
                aux = aux->next;
            }
        }
    }
     

    void print()
    {   
        Tstate ptr;
        Tpath path_;
        ptr = root;
         
        while(ptr!=NULL)
        {   
            cout<<"   "<<ptr->tag<<"|";

            if(ptr->list != NULL)
            {
                path_ = ptr->list;

                while(path_ != NULL)
                {   
                    cout<<" "<<path_->arrival_state->tag<<"("<<path_->arrival_state->status<<")";
                    path_ = path_->next;
                }
     
            }
            ptr = ptr->next;
            cout<<endl;
        }
    }


    void find_path(string x)
    {
        Tstate aux;
        Tpath path_;

        aux = root;

        while(aux != NULL)
        {
            if(aux->tag == x)
            {
                if(aux->list == NULL)
                    return;
                else
                {
                    cout<<" "<<aux->tag<<" ---> ";
                    path_ = aux->list;
     
                    while(path_ != NULL)
                    {
                        cout<<path_->arrival_state->tag<<" ";
                        path_ = path_->next;
                    }
                    cout<<endl;
                    return;
                }
            }
            else
            aux = aux->next;
        }
    }


    string print_lista()
    {
        Tstate ptr;
        Tpath path_;
        ptr = root;

        string dot="";
         
        while(ptr!=NULL)
        {         
            if(ptr->list != NULL)
            {
                path_ = ptr->list;

                while(path_ != NULL)
                {   
                    dot += "\t"+ptr->tag + " -> " + path_->arrival_state->tag + "\n";
                    path_ = path_->next;
                }
     
            }
            ptr = ptr->next;
        }

        return dot;
    }


    string print_estado()
    {
        Tstate ptr;
        Tpath path_;
        ptr = root;

        string dott="";
         
        while(ptr!=NULL)
        {  
            if (ptr->branch != 0 )
                dott += "\n\t" + ptr->tag + "[label=\" " + ptr->tag +"  status: "+ to_string(ptr->status) + " branch: "+to_string(ptr->branch)+ "\";];";
            else
                dott += "\n\t" + ptr->tag + "[label=\" " + ptr->tag +"  status: "+ to_string(ptr->status) + " master"+ "\";];";   
         
            ptr = ptr->next;  
        }

        return dott;
    }


    void delete_state(string x)
    {
        Tstate aux;
        Tpath path_;

        aux = root;

        while(aux != NULL)
        {
            if(aux->tag == x)
            {
                if(aux->list == NULL){
                    aux->status = -1;
                }
                else
                {
                    aux->status = -1;
                    path_ = aux->list;
                    
                    while(path_ != NULL)
                    {
                        path_->arrival_state->status= -1;
                        delete_state(path_->arrival_state->tag);
                        path_ = path_->next;
                    }
                }
            }
            aux = aux->next;
        }
    }


    string date_time() 
    {
        time_t     now = time(0);
        struct tm  tstruct;
        char       buf[80];
        tstruct = *localtime(&now);
        strftime(buf, sizeof(buf), "%Y/%m/%d %X", &tstruct);
        return buf;
    }


    int i = 0;
    string fi = "";
    void merge_files(Tstate &aux, Tstate &aux2)
    {
        set<string> A_set;
        string word;

        while (*aux->fp >> word)
            A_set.insert(word);
        
        while (*aux2->fp >> word)
            A_set.insert(word);

        fi = "merge" + to_string(i) + ".txt";
        ofstream merged(fi);
     
        set<string>::iterator it;
        for (it = A_set.begin(); it != A_set.end(); ++it)
            merged << *it << endl;

        fstream mer(fi);
        if(aux->status != -1 and aux2->status != -1)
        {
            Tstate temp = new struct state;
            temp->tag += "m"+ to_string(i);
            temp->user = "merge_user";
            add_state(temp->tag, aux->branch, temp->user, date_time(), mer);
            create_path(aux->tag, temp->tag);
            create_path(aux2->tag, temp->tag);
            i++;
        }
    }


    void merge(string start, string end)
    {
        Tpath new_ = new struct path;
        Tstate aux, aux2;
     
        if(root == NULL)
            return;
        
        new_->next = NULL;
        aux = root;
        aux2 = root;
     
        while(aux2 != NULL)
        {
            if(aux2->tag == end)
            {
                break;
            }
            aux2 = aux2->next;
        }

        while(aux != NULL)
        {
            if(aux->tag == start)
            {
                merge_files(aux, aux2);
            }
            aux = aux->next;
        }
    }


    void write_graph()
    {
        ofstream fs("grafo.dot");
        fs << "digraph {\n" << print_estado() <<"\n\n"<< print_lista() << "\n}";
        fs.close();

        system("dot -Tpng grafo.dot -o grafo.png");
        system("eog grafo.png");
    }


    void restore(string tiempo)
    {
        Tstate ptr;
        Tpath path_;
        ptr = root;
         
        while(ptr!=NULL)
        {  
            if(ptr->date > tiempo)
            {
                delete_state(ptr->tag);
            }
            ptr = ptr->next;       
        }
    }
};


int main(void)
{   
    struct cvs *cvs = new struct cvs;

    fstream fi;
    fi.open("test.txt");
    fstream fi1;
    fi1.open("test1.txt");
    fstream fi2;
    fi2.open("test2.txt");
    fstream fi3;
    fi3.open("test3.txt");
    fstream fi4;
    fi4.open("test4.txt");
    fstream fi5;
    fi5.open("test5.txt");
    fstream fi6;
    fi6.open("test6.txt");
    
    cvs->add_state("0",0,"ale0",cvs->date_time(),fi);
    cvs->add_state("1",1,"ale1",cvs->date_time(),fi1);
    cvs->add_state("2",0,"ale2",cvs->date_time(),fi2);
    cvs->add_state("3",2,"ale3",cvs->date_time(),fi3);
    cvs->add_state("4",0,"ale4",cvs->date_time(),fi4);
    cvs->add_state("5",2,"ale5",cvs->date_time(),fi5);
    cvs->add_state("6",0,"ale6",cvs->date_time(),fi6);
    cvs->add_state("7",0,"ale7",cvs->date_time(),fi2);
    cvs->add_state("8",1,"ale7",cvs->date_time(),fi6);
    cvs->add_state("9",3,"ale7",cvs->date_time(),fi6);
    cvs->add_state("10",3,"ale7",cvs->date_time(),fi3);
    
    cvs->create_path("0","2");
    cvs->create_path("0","3");
    cvs->create_path("3","5");     
    cvs->create_path("0","1");
    cvs->create_path("2","4");
    cvs->create_path("9","10");
    cvs->create_path("4","6");    
    cvs->create_path("6","7");    
    cvs->create_path("1","8");  
    cvs->create_path("4","9");    
        
    cvs->delete_state("3");

    cvs->merge("7","10");
    cvs->merge("8","6");

    sleep(5);
    
    cvs->add_state("00",3,"ale0",cvs->date_time(),fi);
    cvs->add_state("11",4,"ale1",cvs->date_time(),fi1);
    cvs->add_state("22",3,"ale2",cvs->date_time(),fi);
    cvs->add_state("33",5,"ale3",cvs->date_time(),fi1);
    cvs->add_state("44",3,"ale4",cvs->date_time(),fi4);
    cvs->add_state("55",4,"ale5",cvs->date_time(),fi5);
    cvs->add_state("66",3,"ale6",cvs->date_time(),fi6);
    cvs->add_state("77",3,"ale7",cvs->date_time(),fi2);
    cvs->add_state("88",4,"ale7",cvs->date_time(),fi6);
    cvs->add_state("99",6,"ale7",cvs->date_time(),fi6);
    cvs->add_state("100",6,"ale7",cvs->date_time(),fi3);

    cvs->create_path("00","22");
    cvs->create_path("22","33");
    cvs->create_path("10","00");     
    cvs->create_path("00","11");
    cvs->create_path("22","44");
    cvs->create_path("99","100");
    cvs->create_path("44","66");    
    cvs->create_path("66","77");    
    cvs->create_path("11","88");  
    cvs->create_path("44","99");  
    cvs->create_path("88","55");

    cvs->delete_state("44");

    cvs->merge("22","33");

    string a="2017/07/09 18:39:05";
    cvs->restore(a);

    cvs->add_state("111",7,"ale7",cvs->date_time(),fi6);
    cvs->add_state("112",7,"ale7",cvs->date_time(),fi6);
    cvs->add_state("113",7,"ale7",cvs->date_time(),fi6);
    cvs->add_state("114",7,"ale7",cvs->date_time(),fi6);
    cvs->add_state("115",7,"ale7",cvs->date_time(),fi6);
    
    cvs->create_path("10","111");    
    cvs->create_path("111","112"); 
    cvs->create_path("112","113"); 
    cvs->create_path("113","114"); 
    cvs->create_path("114","115"); 

    cvs->delete_state("112"); 

    cvs->write_graph(); 

    
    return 0;
}