#include <iostream>
#include<vector>
#include <iterator>
#include<fstream>
#include <ctime>

#include <openssl/evp.h>
#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <sstream>

#include <boost/dynamic_bitset.hpp>
#include <limits.h>

using namespace std;

void print_char_vector(vector<char> path)
{
    cout<<endl;
    std::copy(path.begin(), path.end(), std::ostream_iterator<unsigned int>(std::cout, ""));
    cout<<endl;

}

std::string convert_vector_string_ints(vector<char> path)
{
    stringstream ss;
    std::copy(path.begin(), path.end(), std::ostream_iterator<unsigned int>(ss,""));
    return ss.str();

}


int write_data(vector<char> data, char* filename)
{
    std::vector<char>::const_iterator i;
    ofstream myfile (filename, ios::out | ios::binary);
    //vector<char> data;
    //std::copy(data1.begin(), data1.end(), std::back_inserter(data));
    std::copy(data.begin(), data.end(), std::ostream_iterator<char>(myfile, ""));
    return ((data.end()-data.begin())*8);
}

vector<char> read_data(char* filename)
{
    // open the file:
    std::ifstream file(filename, std::ios::binary);
    // read the data:
    vector<char> read_key =  std::vector<char>((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());
    //std::copy(read_key.begin(), read_key.end(), std::ostream_iterator<char>(cout, ""));
    return read_key;
}


vector<char> generate_random_string(int length)
{
    //convert length into bytes from bits
    length = length/8;

    //By default SHA-1 engine is used to generate the random bytes using system entropy by openssl. We can change that by modifying the engine.

    //int  RAND_set_rand_engine(ENGINE *engine);

    //fill a buffer with l/8 random bytes
    char rand_string[length];
    RAND_bytes(rand_string,length);

    //convert to vector
    vector<char> message;
    message.assign(rand_string,rand_string+length );
    return message;

}

vector<char> compute_hash(char* text,int text_len)
{
    EVP_MD_CTX *mdctx;
    mdctx = EVP_MD_CTX_create();
    EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL);
    EVP_DigestUpdate(mdctx, text, text_len);
    char md_value[32];

    int md_len;
    EVP_DigestFinal_ex(mdctx, md_value, &md_len);
    EVP_MD_CTX_destroy(mdctx);
    vector<char> message_digest;
    message_digest.assign(md_value,md_value+32 );
//    std::copy(message_digest.begin(), message_digest.end(), std::ostream_iterator<char>(cout, ""));
    return message_digest;
}

vector<char> access_element_by_index(vector<char> R, int l, int index_position)
{

    //cout<<"String length in bits : "<<l<<endl;
    l = l/8;
    int m = R.size()/l;
    //cout<<"Number of messages : "<<m<<endl;

    index_position = index_position%(m);
    vector<char> message;
    message.assign((R.begin()+(l*index_position)),(R.begin()+(l*(index_position+1))) );
    //cout<<"Message Length : "<<message.size()<<endl;
    return message;
}

unsigned int generate_identifier(vector<char> I, int permutation_length, vector<char> Hk, int permutation_id)
{
    vector<char> it = access_element_by_index(I, permutation_length, permutation_id);
    //print_char_vector(it);
    vector<bool> hash1;
    hash1.assign(Hk.begin(),Hk.end());

    boost::dynamic_bitset<unsigned char> hash(Hk.begin(), Hk.end());
    boost::dynamic_bitset<unsigned char> identifier(it.size());

    for(int i=it.size()-1; i>=0; i--)
    {
        int temp = ((unsigned int)((char)it[i]));
        //cout<< temp % hash.size()<<" : "<<hash[(temp % hash.size())]<<endl;
        identifier[i] = hash[( temp % hash.size())];
    }
    //cout<<identifier;
    //convert it to vector of chaar
    vector< char> buffer(identifier.num_blocks());
    boost::to_block_range(identifier, buffer.begin());
    unsigned int i  = *(unsigned int *) &buffer[0];
    //value.assign(identifier[0]);
    return i;

}

void setup(int l, int j, int m)
{
    int identifier_length = 8;
    int permutation_length = l*j*identifier_length;
    vector<char> salt = generate_random_string(l);
    vector<char> R = generate_random_string(l*m);
    int t = j*l;
    vector<char> I = generate_random_string(permutation_length*t);
    string salt_file = "salt.dp";
    string R_file = "R.dp";
    string I_file = "I.dp";
    write_data(salt, &salt_file[0]);
    write_data(R, &R_file[0]);
    write_data(I, &I_file[0]);
    //cout<<"Successfully written all the values. Please remember the j,l,m values as config values."<<endl;
    //cout<<"Here";

}

int main()
{

    while(true)
    {
        cout<<"[0] Test Correctness"<<endl<<"[1]Generate Performance Table"<<endl<<"[2] Exit"<<endl<<"********************************************"<<endl<<"Enter Your option : ";
        int option;
        cin>>option;
        if (option==0)
        {
            int l,j,m;
            cout<<endl<<"********************************************"<<endl<<"There are 3 parameters l, j , m"<<endl;
            cout<<"Enter value for l : ";
            cin>>l;
            cout<<"Enter value for j : ";
            cin>>j;
            cout<<"Enter value for m : ";
            cin>>m;
            cout<<"********************************************"<<endl;
            setup(l,j,m);
            cout<<"********************************************"<<endl;

            while (true)
            {
                cout<<"[0]Unit Test\n[1]Automatic Batch Test\n[2]Previous Menu"<<endl;
                cout<<"********************************************"<<endl<<"Select the option : ";
                cin>>option;

                if (option==0)
                {
                    vector<char> R = read_data("R.dp");
                    vector<char> I = read_data("I.dp");
                    vector<char> salt = read_data("salt.dp");
                    int identifier_length = 8;
                    int permutation_length = l*j*identifier_length;
                    int t = j*l;
                    cout<<"Done reading files"<<endl;

                    cout<<"Login Name : ";
                    std::string str1;
                    cin>>str1;
                    str1.resize(l/8);
                    vector<char> L(str1.begin(), str1.end());

                    cout<<"Password : ";
                    std::string str2;
                    cin>>str2;
                    str2.resize(l/8);
                    vector<char> P(str2.begin(), str2.end());

                    vector<char> tuple;
                    tuple.insert(tuple.end(), salt.begin(), salt.end());
                    tuple.insert(tuple.end(), L.begin(), L.end());
                    tuple.insert(tuple.end(), P.begin(), P.end());

                    vector<char> Hk = compute_hash(&tuple[0],tuple.size());
                    vector<char> hashcode(l/8,0);
                    for(int i=0; i<t; i++)
                    {
                        unsigned int temp = generate_identifier(I, permutation_length,Hk,i);
                        vector<char> ri = access_element_by_index(R, l, temp%m);
                        for(int j=0; j<hashcode.size(); j++)
                        {
                            hashcode[j] = hashcode[j] xor ri[j];
                        }
                    }

                    //output format is salt_len|login_len|htag_len|salt|login|htag

                    vector<char> output_vec;
                    output_vec.insert(output_vec.end(), salt.begin(), salt.end());
                    output_vec.insert(output_vec.end(), L.begin(), L.end());
                    output_vec.insert(output_vec.end(), hashcode.begin(), hashcode.end());

                    write_data(output_vec,"user_data.dp");
                    cout<<"Computing completed and the record has been stored in user_data.dp"<<endl;

                    cout<<"Login Name : ";
                    str1.clear();
                    cin>>str1;
                    str1.resize(l/8);
                    vector<char> L1(str1.begin(), str1.end());

                    cout<<"Password : ";
                    str2.clear();
                    cin>>str2;
                    str2.resize(l/8);
                    vector<char> P1(str2.begin(), str2.end());

                    vector<char> tuple1;
                    tuple1.insert(tuple1.end(), salt.begin(), salt.end());
                    tuple1.insert(tuple1.end(), L1.begin(), L1.end());
                    tuple1.insert(tuple1.end(), P1.begin(), P1.end());

                    vector<char> Hk1 = compute_hash(&tuple1[0],tuple1.size());
                    vector<char> hashcode1(l/8,0);
                    for(int i=0; i<t; i++)
                    {
                        unsigned int temp = generate_identifier(I, permutation_length,Hk1,i);
                        vector<char> ri = access_element_by_index(R, l, temp%m);
                        for(int j=0; j<hashcode1.size(); j++)
                        {
                            hashcode1[j] = hashcode1[j] xor ri[j];
                        }
                    }

                    //output format is salt_len|login_len|htag_len|salt|login|htag

                    vector<char> output_vec1;
                    output_vec1.insert(output_vec1.end(), salt.begin(), salt.end());
                    output_vec1.insert(output_vec1.end(), L1.begin(), L1.end());
                    output_vec1.insert(output_vec1.end(), hashcode1.begin(), hashcode1.end());
                    //std::string output_str( reinterpret_cast<char *>(&output_vec1[0]), output_vec1.size() );
                    //Now we check if the vector is contained in the file.
                    cout<<"Loading the user_data.dp file again."<<endl;
                    vector<char> original_data = read_data("user_data.dp");


                    string tup = convert_vector_string_ints(output_vec1);
                    string db  = convert_vector_string_ints(original_data);

                    if(db.find(tup)!=std::string::npos)
                    {
                        cout<<"User Re-Authenticated. Congratulations!!!"<<endl;
                    }
                    else
                    {
                        cout<<"User not Authenticated."<<endl;
                    }









                }
                else if (option==1)
                {
                    cout<<"Efficient way to search through log file to be implemented for better figures. Linear comparison will be slow and inaccurate measurement"<<endl;
                }
                else if (option==2)
                {
                    break;
                }
                else
                {
                    cout<<"Please Enter a valid value"<<endl;
                }
            }

        }
        else if (option==1)
        {
            int l = 128;
            std::string str1 = "darshan";
            std:string str2 = "password";
            str1.resize(l/8);
            str2.resize(l/8);
            vector<char> L(str1.begin(), str1.end());
            vector<char> P(str2.begin(), str2.end());
            int j_vals[3];
            j_vals[0]=1;j_vals[2]=3;j_vals[1]=2;
            ofstream myfile;

            myfile.open ("values.csv");
            for(int i=0; i<sizeof(j_vals); i++)
            {
                int m =500;
                while(m<=256000)
                {   int j=j_vals[i];
                    m=2*m;
                    setup(l,j,m);

                    vector<char> R = read_data("R.dp");
                    vector<char> I = read_data("I.dp");
                    vector<char> salt = read_data("salt.dp");

                    int identifier_length = 8;
                    int permutation_length = l*j*identifier_length;
                    int t = j*l;

                    vector<char> tuple;
                    tuple.insert(tuple.end(), salt.begin(), salt.end());
                    tuple.insert(tuple.end(), L.begin(), L.end());
                    tuple.insert(tuple.end(), P.begin(), P.end());
                    vector<char> Hk = compute_hash(&tuple[0],tuple.size());
                    vector<char> hashcode(l/8,0);
                    for(int i=0; i<t; i++)
                    {
                        unsigned int temp = generate_identifier(I, permutation_length,Hk,i);
                        vector<char> ri = access_element_by_index(R, l, temp%m);
                        for(int j=0; j<hashcode.size(); j++)
                        {
                            hashcode[j] = hashcode[j] xor ri[j];
                        }
                    }

                    //output format is salt_len|login_len|htag_len|salt|login|htag

                    vector<char> output_vec;
                    output_vec.insert(output_vec.end(), salt.begin(), salt.end());
                    output_vec.insert(output_vec.end(), L.begin(), L.end());
                    output_vec.insert(output_vec.end(), hashcode.begin(), hashcode.end());

                    write_data(output_vec,"user_data.dp");


                    //we want an average time to over a set of calls
                    clock_t start = clock();

                    for(int count=0; count<100; count++)
                    {
                        vector<char> tuple1;
                        tuple1.insert(tuple1.end(), salt.begin(), salt.end());
                        tuple1.insert(tuple1.end(), L.begin(), L.end());
                        tuple1.insert(tuple1.end(), P.begin(), P.end());

                        vector<char> Hk1 = compute_hash(&tuple1[0],tuple1.size());
                        vector<char> hashcode1(l/8,0);
                        for(int i=0; i<t; i++)
                        {
                            unsigned int temp = generate_identifier(I, permutation_length,Hk1,i);
                            vector<char> ri = access_element_by_index(R, l, temp%m);
                            for(int j=0; j<hashcode1.size(); j++)
                            {
                                hashcode1[j] = hashcode1[j] xor ri[j];
                            }
                        }

                        //output format is salt_len|login_len|htag_len|salt|login|htag

                        vector<char> output_vec1;
                        output_vec1.insert(output_vec1.end(), salt.begin(), salt.end());
                        output_vec1.insert(output_vec1.end(), L.begin(), L.end());
                        output_vec1.insert(output_vec1.end(), hashcode1.begin(), hashcode1.end());

                        vector<char> original_data = read_data("user_data.dp");


                        string tup = convert_vector_string_ints(output_vec1);
                        string db  = convert_vector_string_ints(original_data);

                        if(db.find(tup)!=std::string::npos)
                        {
                            ;
                        }
                        else
                        {
                            cout<<"User not Authenticated."<<endl;
                        }


                    }
                    clock_t stop = clock();
                    myfile <<j<<","<<m<< "," << ((stop-start)/(double)(CLOCKS_PER_SEC / 1000))/100 << endl;
                    cout   <<j<<","<<m<< "," << ((stop-start)/(double)(CLOCKS_PER_SEC / 1000))/100<< endl;

                }
            }
            ;
            //myfile.close();
        }
        else if (option==2)
        {
            break;
        }
        else
        {
            cout<<"Please Enter a valid value"<<endl;
        }
    }





}


