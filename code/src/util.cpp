#include "util.hpp"
#include "dtsw.hpp"
namespace dtsw{
    /*------------------------------------------*/
    void read_var_D(const char * filename,
                    std::vector< std::pair<uint32_t, uint32_t> > &idx,
                    std::vector< quad<double> > &data)
{
        FILE *f = fopen(filename,"rb");
        uint64_t  N,nnz;
        fread(&N,sizeof(uint64_t),1,f);
        fread(&nnz,sizeof(uint64_t),1,f);

        idx.resize(nnz);
        fread(&idx[0], sizeof(std::pair<uint32_t, uint32_t>), nnz, f) ;

        data.resize(nnz);
        fread(&data[0], sizeof(quad<double>), nnz, f) ;

        fclose(f);

    }
    /*------------------------------------------*/
    void read_var_D_block(const char * filename,
                    std::vector< std::pair<uint32_t, uint32_t> > &idx,
                    std::vector< quad<double> > &data,
                    size_t item_count, size_t block_idx)
{
        FILE *f = fopen(filename,"rb");
        uint64_t  N,nnz;
        fread(&N,sizeof(uint64_t),1,f);
        fread(&nnz,sizeof(uint64_t),1,f);
        long cur = ftell(f);
        size_t index_size = item_count * sizeof(std::pair<uint32_t, uint32_t>);
        idx.resize(item_count);
        fseek(f,block_idx * index_size, SEEK_CUR);
        cur=ftell(f);
        fread(&idx[0], sizeof(std::pair<uint32_t, uint32_t>), item_count, f) ;

        size_t data_size = item_count *sizeof(quad<double>);
        data.resize(item_count);
        fseek(f,sizeof(uint64_t)*2
               + nnz * sizeof(std::pair<uint32_t, uint32_t>)
               + block_idx * data_size ,SEEK_SET);
        cur=ftell(f);
        fread(&data[0], sizeof(quad<double>), item_count, f) ;

        fclose(f);
        (void)cur;
    }
    /*------------------------------------------*/
    void print_index(std::vector< std::pair<uint32_t, uint32_t> > &idx,int from,int N){
        for(int i=from;i<from+N;i++)
            std::cout << "Index ["<<i <<"]= " << idx[i].first <<"," << idx[i].second <<std::endl;
    }
    /*------------------------------------------*/
    void print_data(std::vector< quad<double> > &data,int from,int N){
        for(int i=from;i<from+N;i++){
            std::cout << "Data["<<i <<"] = " ;
            for(int j=0;j<4;j++){
                std::cout << data[i].v[j] << " ";
            }
            std::cout << std::endl;
        }
    }
    /*------------------------------------------*/
    void test_read_sparse(){
        std::vector< std::pair<uint32_t, uint32_t> > index,index2;
        std::vector< dtsw::quad_t > data,data2;
        const char *fn="C:\\Afshin\\DTShallowWater\\ShallowWater-master\\ShallowWater\\galew-6400-31-ep2.7-o4-gc-0.05\\D";

        dtsw::read_var_D(fn,index,data);
        int B=100;
        size_t K_MAX = 198400/B;
        for ( size_t K = 0;K < K_MAX; K++){

            dtsw::read_var_D_block(fn,index2,data2,B,K);
            for(int i=0;i < B; i++){
                for(int j=0;j<4;j++){
                    if(std::abs(data2[i].v[j] - data[K*B+i].v[j])>.0){
                        std::cout <<  "K = " << K << " " ;
                        std::cout << "error in Data " << i << "," << j << std::endl;
                        std::cout << std::setw(20) << std::setprecision(10) << (double)(data2[i].v[j] - data[K*B+i].v[j] )<< std::endl;
                    }
                }
                if(index2[i].first != index[K*B+i].first){
                    std::cout << "error in Index " << i << std::endl;
                }

            }
        }
    }
    /*------------------------------------------*/
    void read_var_H_block(const char *filename,
                    std::vector<double> &data,
                    int item_count,
                    int block_index){
        FILE *f = fopen(filename,"rb");
        uint64_t M,N;
        fread(&M,sizeof(uint64_t),1,f);
        fread(&N,sizeof(uint64_t),1,f);
        size_t item_size = sizeof(double) * 4; // each row of H has 4 elements;
        fseek(f,block_index * item_count * item_size  ,SEEK_CUR);
        data.resize(item_count*M);
        fread(&data[0],item_size,item_count,f);
        fclose(f);
    }
    /*------------------------------------------*/
    void read_var_H(const char *filename,
                    std::vector<double> &data){
        FILE *f = fopen(filename,"rb");
        uint64_t M,N;
        fread(&M,sizeof(uint64_t),1,f);
        fread(&N,sizeof(uint64_t),1,f);
        size_t item_size = sizeof(double) ;

        data.resize(M*N);
        fread(&data[0],item_size,M*N,f);
        fclose(f);
    }
    /*------------------------------------------*/
    void test_read_H(){
        std::vector< double > data,data2;
        const char *fn="C:\\Afshin\\DTShallowWater\\ShallowWater-master\\ShallowWater\\galew-6400-31-ep2.7-o4-gc-0.05\\H";

        dtsw::read_var_H(fn,data);
        int B=10;
        size_t K_MAX = 6400/B;
        for ( size_t K = 0;K < K_MAX; K++){
            dtsw::read_var_H_block(fn,data2,B,K);
            for(int i=0;i < B; i++){
                for(int j=0;j<4;j++){
                    if(std::abs(data2[i*4+j] - data[K*B*4+i*4+j])>0.0){
                        std::cout <<  "K = " << K << " " ;
                        std::cout << "error in H " << i << "," << j << std::endl;
                        std::cout << std::setw(20) << std::setprecision(10) << (double)(data2[i*4+j] - data[K*B*4+i*4+j])<< std::endl;
                    }
                }
            }
        }
    }
    /*------------------------------------------*/
    void read_var_Atm_block(const char *filename,
                            std::vector<double> &data,
                            int item_count, int block_index){
        FILE *f = fopen(filename,"rb");
        uint64_t M,N;
        fread(&M,sizeof(uint64_t),1,f);
        fread(&N,sizeof(uint64_t),1,f);
        size_t item_size = sizeof(atmdata_t) ;
        fseek(f,block_index * item_count * item_size  ,SEEK_CUR);
        data.resize(item_count*M);
        fread(&data[0],item_size,item_count,f);
        fclose(f);
    }
    /*------------------------------------------*/
    void read_var_Atm(const char *filename,
                        std::vector<double> &data){
        FILE *f = fopen(filename,"rb");
        uint64_t M,N;
        fread(&M,sizeof(uint64_t),1,f);
        fread(&N,sizeof(uint64_t),1,f);
        size_t item_size = sizeof(double) ;

        data.resize(N*M);
        fread(&data[0],item_size,M*N,f);
        fclose(f);
    }
    /*------------------------------------------*/
    void test_read_Atm(){
        std::vector< double > data,data2;
        const char *fn="C:\\Afshin\\DTShallowWater\\ShallowWater-master\\ShallowWater\\galew-6400-31-ep2.7-o4-gc-0.05\\atm";

        dtsw::read_var_Atm(fn,data);
        int B=10;
        size_t K_MAX = 6400/B;
        int M = sizeof(atmdata_t)/sizeof(double);
        for ( size_t K = 0;K < K_MAX; K++){
            dtsw::read_var_Atm_block(fn,data2,B,K);
            for(int i=0;i < B; i++){
                for(int j=0;j<M;j++){
                    if(std::abs(data2[i*M+j] - data[K*B*M+i*M+j])>0.0){
                        std::cout <<  "K = " << K << " " ;
                        std::cout << "error in ATM " << i << "," << j << std::endl;
                        std::cout << std::setw(20) << std::setprecision(10) << (double)(data2[i*M+j] - data[K*B*M+i*M+j])<< std::endl;
                    }
                }
            }
        }
    }
}
